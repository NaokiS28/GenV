# GenV PlayStation Hardware Support Notes
GenV supports the Sony PlayStation and derivatives as a target for running games on.
When making applications for the PlayStation however, there are some important notes to look out for.

## Video
Video on the PlayStation and derivatives use the same general hardware, consisting of a custom Sony branded but Toshiba designed GPU, Geometry Transform Engine (GTE) and Motion Decoder (MDEC). For indepth information on these parts, please see ([psx-spx GPU](https://psx-spx.consoledev.net/graphicsprocessingunitgpu/)).

### Colour Output
Outside of FMVs using the MDEC, the maximum colour depth that the PS1 supports is 24-bit colour, however more often than not you are limited to only 16-bit color. This means 64k colours but gradients will be either dithered or striped. Note this is only 2D, 3D operations are always only 16-bit. It is theoretically possible to render to the screen buffer at 24-bits of colour, but this requires software rendering and is not supported by GenV.

### Video Resolution Support
The PS1 GPU does not support resolutions higher than 640x480 in interlaced mode, and only as high as 640x240 in progressive mode. Also, as the base PS1 hardware only ships with 1MiB of Video RAM (VRAM), use of a full 640x480 screen will eat into most of the VRAM. It is also not possible to have double buffering (in the traditional sense) when using this high resolution mode either. It's recommended to use a lower resolution such as 320x240 (or up to 640x240) in progressive mode if you want greater horizontal detail, but the game must account for this stretched image when rendering (unless 16:9 is your goal). Use of 480i is good for menus and title screens where there is little happening and not much VRAM is needed.

On arcade platforms based on the PS1, it is possible for them to have up to 2MiB of VRAM, which along with allowing for more textures at a higher bitdepth, also means you can double buffer higher resolution screens. But be aware that the higher the resolution, the less time you will have to render.

## Audio
The PlayStation 1 Sound Processing Unit (SPU) is an evolution of the Super Nintendo Entertainment System APU, in that it is a sample based audio processor which supports 32 channels of sound. For indepth information on this parts, please see ([psx-spx SPU](https://psx-spx.consoledev.net/graphicsprocessingunitgpu/)).

### Audio Format Support
The SPU only supports playing ADPCM encoded file formats. Other options for audio include CD Red Book audio and CD-XA streaming, however of these two, the last option is specific to the PS1 and should not be used when making cross platform GenV applications. CD-Audio is also not supported on all derivatives, and some derivatives have their own extensions or replacements to the sound system.

For ADPCM files, which is going to be used for things like sound effects, streamed music or sequenced music, GenV ships with tools for converting common audio formats to the ADPCM format. The audio files must be in ADPCM format at runtime or the engine will reject them. Converting file formats at runtime, whilst theoretically possible, is entirely impractical and is not supported in GenV.

## IO
### Multitap
The multitap for the PS1 is a funny beast to put it nicely. It has the following known issues:
* Currently in GenV, a controller must always be present in port A for direct access to work
* PS1 mouse will not work in port D when using direct port access. You must use long mode for this.
* In direct access, putting the mouse into port D will cause the entire multitap to go offline as the multitap ACK is held low and it will not respond correctly.
* PS2 DualShock 2s can be glitchy and might not show up if there is not another PS1 device connected. When one is present, it seems to work *alright*.
* PS2 DVD Remote IR receiver just does not work at all in the multitap.
* Some GunCon clones might have issues with the multitap. A 4gamers Justifier/GunCon/GunCon2 clone will just not work in GunCon mode over the multitap (it also requires C-sync to work at all)
* All Justifiers share the IRQ10 line, which makes using more than 2 pretty much impossible.
* Reading from memory cards is already painfully slow. Reading from 8 at the same time is not recomended to say the least.