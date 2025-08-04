# GenV PlayStation Hardware Support Notes
GenV supports the Sony PlayStation and derivatives as a target for running games on.
When making applications for the PlayStation however, there are some important notes to look out for.

## Video
Video on the PlayStation and derivatives use the same general hardware, consisting of a custom Sony branded but Toshiba designed GPU, Geometry Transform Engine (GTE) and Motion Decoder (MDEC). For indepth information on these parts, please see ([psx-spx GPU](https://psx-spx.consoledev.net/graphicsprocessingunitgpu/)).

### Colour Output
Outside of FMVs using the MDEC, the maximum colour depth that the PS1 supports is 16-bit colour. This means 64k colours but gradients will be either dithered or striped. It is theoretically possible to render to the screen buffer at 24-bits of colour, but this requires software rendering and is not supported by GenV.

### Video Resolution Support
The PS1 GPU does not support resolutions higher than 640x480 in interlaced mode, and only as high as 640x240 in progressive mode. Also, as the base PS1 hardware only ships with 1MiB of Video RAM (VRAM), use of a full 640x480 screen will eat into most of the VRAM. It is also not possible to have double buffering (in the traditional sense) when using this high resolution mode either. It's recommended to use a lower resolution such as 320x240 (or up to 640x240) in progressive mode if you want greater horizontal detail, but the game must account for this stretched image when rendering (unless 16:9 is your goal). Use of 480i is good for menus and title screens where there is little happening and not much VRAM is needed.

On arcade platforms based on the PS1, it is possible for them to have up to 2MiB of VRAM, which along with allowing for more textures at a higher bitdepth, also means you can double buffer higher resolution screens. But be aware that the higher the resolution, the less time you will have to render.

## Audio
The PlayStation 1 Sound Processing Unit (SPU) is an evolution of the Super Nintendo Entertainment System APU, in that it is a sample based audio processor which supports 32 channels of sound. For indepth information on this parts, please see ([psx-spx SPU](https://psx-spx.consoledev.net/graphicsprocessingunitgpu/)).

### Audio Format Support
The SPU only supports playing ADPCM encoded file formats. Other options for audio include CD Red Book audio and CD-XA streaming, however of these two, the last option is specific to the PS1 and should not be used when making cross platform GenV applications. CD-Audio is also not supported on all derivatives, and some derivatives have their own extensions or replacements to the sound system.

For ADPCM files, which is going to be used for things like sound effects, streamed music or sequenced music, GenV ships with tools for converting common audio formats to the ADPCM format. The audio files must be in ADPCM format at runtime or the engine will reject them. Converting file formats at runtime, whilst theoretically possible, is entirely impractical and is not supported in GenV.