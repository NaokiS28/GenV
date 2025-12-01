# Dev Blog for NaokiS

### 1th December 2025

It's storming, my windows are leaking and it turns out I've also been causing GVSS to be squished width wise. Take a look at the image here:

![](assets/20251201_204806_image.png)

You'll noticed that the image looks... off. A bit egg shaped. I have been using PCSX-Redux to debug the code and it always seemed to make things look strange on the output so I didn't think much of it. But it also looked off-centre too...

![](assets/20251201_204945_image.png)

This is the GPU driver initialising, which for debug reasons I show as being a blue screen, but yet there is a black bar to the right of it. Again, I thought it was just an artifact so I continued on and at some point ran it on the PS1 officially:

![](assets/20251201_205224_IMG_0538.jpg)

Ignore the messy desk, please! I was actually quite happy to see the logo rendering on the real hardware, really felt like progress! It even looks rounder still!... But.. I could tell, something wasn't right still. Why is the fill screen command not drawing a full screen's size? Well, to cut a long story short:

```diff
int PSXGPU::setResolution(int w, int h, bool updateWindow)
    {
         ...
-        GP1HorizontalRes horizontalRes = GP1HorizontalResList[mode % 5];
-        GP1VerticalRes verticalRes     = GP1VerticalResList[((mode & 0x7F) >= 10)]; 
+        GP1HorizontalRes horizontalRes = GP1HorizontalResList[mode & 0x05];
+        GP1VerticalRes verticalRes = GP1VerticalResList[(vidMode.height > 240)];
         ...
}
```

....yeah. It looked squished because of how I was setting the horizontal resolution. I'm not sure what I was thinking with the modulo operator when I wrote it. I guess my thinking was along the lines of making sure it was only in the range of 0-5... But completely forgetting that doing a modulo doesn't clamp the variable but has the result being the remainder after the operation. So yeah, thats a small but annoying bug. But now the logo is not only centred but also not egg shaped, which is nice:

![](assets/20251201_210314_image.png)

It still looks a bit funky, but this time I think it really *is* PCSX-Redux causing it. Oh, and the fill screen works properly too. And the funny thing is that the only reason I looked into it was because of the next minor change I made which *did* fill the screen correctly. I now present to you, a Blue Screen of Death (BSoD) for GenV on PS1:

![](assets/20251201_210823_image.png)

This is really quite simple and using inherited code from spicyjpeg's 573-in-1 tool for the exception handler to get the stack trace. But the reason I made it wasn't just for the laughs; whenever I would get an `assert` or `halt` or even an `exception`, I would often stare at Redux with a blank stare and wonder why the code isn't getting to my breakpoints. I do ofcourse use SIO1/UART debugging, but well, the way it's set up is that I have a python script to connect to Redux's SIO1 server, and sometimes it doesn't get focus. And yes, I could just give it focus but I often forget to. Besides, nothing says "error" like a BSoD. And now I won't have to guess why it hung. There's a similar one for assertions and any time the code will expect to hang.

It was also pretty easy to implement the hooks for it. The "halt screen" is set up like [genv_terminal](../../src/genv_common/terminal/terminal.h) is, where there is a struct of function pointers that are called when an event happens, but in this case, it just passes a string to the halt screen function to draw and it is down to the system/gpu to draw it. Of course, when you encounter this severe of an error, you have to assume that nothing at all works. So [the PSX halt screen](../../src/genv_hw/psx/psx/video/halt/halt.c) completely reinit's the whole GPU and will draw it out in pure C code. The only shared thing (that is a TODO to change) is the font which is used. Really speaking there needs to be a hard coded font that is garunteed to be there and work but for a quick and dirty implementation, this'll work for now.

(If you're curious, the halt screen above is a divide by 0 error)

Any way, other than that I have been working on a lot more behind the systems, mainly the inputs and trying to animate the GVSS logo screen, but so far it's only at this point:

![](assets/20251201_212950_011225.gif)


Needs work. Obvs. But working on it 😜

### 4th November 2025

Well, a day later and I managed to get some other parts working enough to show the generic error message screen. Why is this notable? Well it was written and coded for Windows! Well... ok. It was coded on Windows and when being coded GenV, or DXUX as it was called then, ran only on Windows. So this is technically the first "Application" that ran almost perfectly the first time I tried to run it, outside of back-end bugs. The only things that needed to be changed were some of the font alignment values since they were hard coded to look good at 800x600 with a different font. This is to say, it would have translated pretty well if I used the font sizing and spacing back then, but now I have to API to do so!

![](assets/20251104_133327_genv-error-040925.gif)

So some points about whats going on here:

This is the generic error screen in GenV. When something goes wrong that requires the user to be aware (think more arcade operations than anything), this fades the background out and highlights the error message to the user. It is also supposed to pause the application that was running (or rather stop updates) but that isn't yet working. You can see the following:

* Title
* Error Severity - The default error screen supports Info, Warning, Error and as you see here, Critical Error.
  * Info screens - This displays a solid blue colour border and serves more to inform the user of an action (IE, machine needs to restart). The fact it's part of the error message class is a bit misleading but it's almost all the same code so there we go.
  * Warning screens - These instead show a flashing yellow border and are for when there is non-critical trouble. An example case might be a stuck control or failure to read a memory device.
  * Error screens/Critical Errors - These are severe errors with a flashing red border that cannot be ignored, i.e. the disc drive is missing the media, a required file cannot load or such. Critical errors differ in that they will halt the program execution until reboot.
* Error cause - Self explanatory really, the caller of the error code function passes a reason to the error screen to display, in this case "Both app pointers are null". In this example, the foregroundApp and backgroundApp Application class pointers are both nulled out and there is nothing to run. This is a critical error because there must always be an application running in order for execution flow to happen normally as loading screens (which GVSS is a special kind of loading screen) are only initiated from the running foregroundApp. So with nothing to run, a critical error occurs.
* Button options - In reality, in the case of a critical error this would be blank as there is no continuing from a critical error, but for testing these were enabled. This field can be customised by the caller to have different buttons and actions, but the error screen does not process these inputs, only handing them to the running app. In the case of an error screen, it could give an operator the chance to try again or go into the test mode of the game.

One fun note is how the text uses the same bitmap graphics and modifies the colour lookup table to use when rendering. On the PS1, each call to render text will check to see if the colour requested has been used before (outside of white), if the colour has not been used then it will create and upload a new colour pallete to use and store that in a table of previous colours, up to 8. I have also been working on a popularity list object that will keep the most frequently used items in memory so if there is no more space in the colour list, it will remove the least used colour.

There are some changes I wish I could make such as having a global colour table that can be used across several objects but... thats a later problem.

Anyway a small update but something more betterer than yesterdays images.

Naoki

---

### 3rd November 2025

This is the first entry in a crude dev blog that I am starting to mostly track changes for myself. Also because I feel some of the things I dont explain well in git commits or comments in code could fall here.

#### What is GenV?

A quick note for those who have stumbled on this repo and want to learn more:
GenV (or General ENgine for V(5th gen), Generation 5) is a game engine system designed for those who want to develop for various consoles and computers but want to get an easy way in to start with. There's been several SDKs and engines for various platforms but they all tend to be specific, such as PsyqO in the PS1 case where it is a very good SDK but only for making PS1 games. GenV then follows a theory that you don't have to know any of the specifics of the consoles or PCs so you can code up an app with no care as to how the platform is. But it can have more specific bits of code for such (i.e. Arcade systems needing coin-up code).

So basically Unity/Unreal... ***kind-of*** but where the minimum common platform is 5th gen platforms, IE PS1. In theory N64 and Saturn could be targets too but I've not written nor intend to make backends for those 😉

#### Updates

Anyways, I pushed a *small* update today mostly to keep the git repo up to date, but now the code base is at least at a very early alpha stage. It can render to the screen! I know, such an amazing achievement. At the moment it will show a crudely drawn GenV logo with a MegaDrive/Geni-sus TMSS style screen (altough it looks more like the Dreamcast TMSS than anything), then load the example app to display "This is an example string.". None of it looks particularly great (well the font does but spicyjpeg did that), but it's trivially fixed. I'm going to update the GenV logo loaded to be a 16-bit image for proper anti-aliasing and render it as a flat quad to do some scaling effects for an intro animation, cuz ya gotta have the fancies.

Other pitfalls I am going to encouter in doing this and making the GenV logo look nice is how to abstract the PS1's specific quirks into an API that tries to not care for quirks of a specific system. Specficically, the PS1's implementation of alpha blending is... interesting. Either a pixel is truly alpha (#0000) or partly transparent (bit 15 set high), then uses either additive, subtractive or quater blend modes. And understanding these modes from purely reading [https://psx-spx.consoledev.net](https://psx-spx.consoledev.net) is difficult. I'm sure a better solution will present itself in due course but my current thinking is to use the 8-bit alpha value stored in most formats and GenV's colour struct to aid in this, picking the best blend mode that it can. Invariably at some point textures would have to be optimised for PS1 rendering which would require specific textures to avoid alpha-bended regions outside of simple shadows.

Anywho as of [commit 8136a1c](https://github.com/NaokiS28/GenV/commit/8136a1c3808ff3dd0f12fcb0c071eda1fc942008), we currently show this:

![](assets/genv-progress-030925.gif)

**A few notes about this image:**

The blue screen is shown on debug builds to visually indicate that the GPU core has started and finished initialisation. On release builds this will render as black to clear the screen instead.

The GenV logo will always be show first, though I'm planning to make it be overwrittable perhaps so it can be customised as part of the programmers start up sequence, but I would like to make it a part of the requirements to show the logo when the engine is used - This is going to be an item I think about in the long run because I dont want it to be a hindrence or detraction, but equally I want to bring attention to the project so it can encourage more game developers to work on classic consoles!

The example app (which is only compiled when no other app is built with GenV, theoretically anyways) will show case the various engine features as they get added and to show how the same apps can run on multiple platforms. There's a long way to go before that but small steps.

#### Next steps

Once I've tidied a few things I'll probably work more on the GenV logo just to make it look nicer even if its still a static image and text, then start working on controller code. After all, I bought a big bunch of controllers and adaptors over the previous months, I need to start actually testing them out or I'm going to have to find another excuse to use when the significant other asks!

I'll see if I can keep a monthly update on this going. Knowing my progress with these kinds of things, probably not, but ah well.

Naoki
