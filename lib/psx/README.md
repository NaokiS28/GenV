
# PlayStation 1 bare-metal C examples

This library was originally downloaded from https://www.github.com/spicyjpeg/ps1-bare-metal.

## Background

I have been occasionally asked if I could provide an example of PS1 homebrew
programming that is completely self-contained, permissively licensed and does
not depend on an external SDK. While there are a number of PS1 SDK options
around (including some I have contributed to), their workflows may not suit
everyone and some of the most popular options are additionally encumbered with
legal issues that make them a non-starter for commercial homebrew games, and/or
limitations that are hard to work around. As I have been moving away from using
such libraries myself, I set out to take what I am currently building for my
projects, clean it up and turn it into a tutorial series for other people to
follow.

I want this repo to be an introduction to bare-metal platforms and the PS1 for
anybody who already has some experience with C/C++ but not necessarily with the
process of linking and compiling, the internals of a standard library, the way
threads and IRQs work at the kernel level and so on. I strongly believe that
demystifying the inner workings of a platform can go a long way when it comes to
helping people understand it. Most 8-bit and 16-bit consoles have received a lot
of attention and excellent bare-metal tutorials have been written for them, so I
don't get why people shall just give up and use ancient proprietary SDKs from
the 1990s when it comes to the PS1.

## License

Everything in this repository, including the vendored copy of
[Marco Paland's printf library](https://github.com/mpaland/printf), is licensed
under the MIT license (or the functionally equivalent ISC license). The only
"hard" requirements are attribution and preserving the license notice; you may
otherwise freely use any of the code for both non-commercial and commercial
purposes (such as a paid homebrew game or a book or course).

## See also

- If you are just getting started with PS1 development, Rodrigo Copetti's
  [PlayStation Architecture](https://copetti.org/writings/consoles/playstation)
  is a great overview of the console's hardware and capabilities.
- The [PlayStation specifications (psx-spx)](https://psx-spx.consoledev.net/)
  page, adapted and expanded from no$psx's documentation, is the main hardware
  reference for bare-metal PS1 programming and emulation.
- [573in1](https://github.com/spicyjpeg/573in1) is a real world example of a
  moderately complex project built on top of the scripts and support library
  provided in this repository.
- If you need help or wish to discuss PS1 homebrew development more in general,
  you may want to check out the
  [PSX.Dev Discord server](https://discord.gg/QByKPpH).
