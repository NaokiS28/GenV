# GenV engine architecture

Status: DRAFT for discussion. Nothing here is implemented yet.

## Overview

Three pieces of the engine that are not pinned down yet:

1. **Engine stack** - which layer may talk to which, and where the app boundary sits.
2. **Event delivery** - how input and other edges reach game code (psyqo style).
3. **Resource layers** - how level data, characters and other assets are loaded, owned and freed.

Already settled and only restated here where it matters: apps are separate loadable images bound
through a fixed function table (one game app loaded at a time), scenes live inside apps (App > Scene >
render Layer), and render layers are ordered and typed 2D/3D.

## 1. Engine stack

### Bullets

- Four layers, top to bottom: **App** -> **Facades** (`GenV::Video`, `IO`, `Audio`, `Files`,
  `System`, `Scene`) -> **Services** (managers: scene, player, output, storage, asset) -> **Drivers**
  (`GenV::HW::<platform>::...`).
- Calls go down only. A layer never includes or calls anything above it.
- The app sees only `genv.hpp`. Driver and service headers are not on the app's include path.
- The function table is the app contract on every platform. How the app is delivered is a build
  option: an overlay loaded from disc on PS1/573, or linked into the executable on PC for debugging.
  Same app source, same header, same table.

### Context

The table exists so one compiled app can run on the PS1 and 573 engine builds. Linking against engine
symbols cannot do that, because the two builds share no internal addresses. On PC a DLL per game buys
little beyond hot-reload, so static linking with the same table is the easier default there.

### Details

- Anything that crosses the table is frozen: append only, never reorder or remove, version field in the
  table header.
- Hot per-frame data (input state) may be a frozen struct. Everything else (files, sounds, screens,
  textures) is an opaque handle plus accessor calls, so object layouts stay free to change.
- Drivers report upwards only through the event queue (section 2), never by calling into services or
  the app.

## 2. Event delivery

### Bullets

- Continuous state is **polled**: stick position, is a button held.
- Changes are **events**: button pressed/released, device connected/disconnected, coin inserted,
  file read finished, scene change requested.
- Drivers detect changes and **queue** events. The engine drains the queue at one fixed point in the
  frame, never from an interrupt.
- The **scene manager** delivers events: top scene first; if it does not consume the event, it goes
  to the scene below.
- Scene changes requested while events are being delivered are **deferred** until delivery ends.

### Context

This is the psyqo model. psyqo's `SimplePad` compares this frame's button bits against last frame's
and calls a callback for each change, between frames, from the main loop. `isButtonPressed()` still
exists for polling. GenV keeps that and fixes two things psyqo leaves to the user:

- psyqo has one callback per pad driver, so each scene sets it in `start` and must clear it in
  `teardown`. In GenV scenes receive events from the scene manager and register nothing.
- psyqo warns that pushing or popping a scene inside the callback can delete the callback while it is
  running. GenV queues `push`/`pop`/`replace` during delivery and applies them afterwards.

### Details

- Queue is a fixed-size ring buffer, no allocation. Size and overflow policy (drop oldest vs drop
  newest, plus a counter) to be decided.
- Event record: type, source (player index / device / file handle), payload word. Small and fixed.
- Scene hook: `bool onEvent(const Event&)`, returning true to consume. Default returns false.
- Whether events reach scenes below follows the same boundary as `UpdateBelow`: a frozen scene does
  not receive events.
- Frame order: poll drivers -> drain events into scenes -> apply deferred scene changes ->
  `update(dt)` -> `render`.

## 3. Resource layers

### Bullets

- Resources live in a **stack of lifetime layers**: engine -> app -> level -> entities.
- Each layer allocates from its own region, on top of the layer below.
- **A layer may reference anything below it, never above.** Enemies may use level tiles; the level
  never holds an enemy.
- Unloading is popping: leaving a level pops entities, then level, and the memory returns in one step.
- A scene's `enter()` pushes its layer and `exit()` pops it, so the scene stack and the resource stack
  are one mechanism.
- Resources are handles (index + generation), never raw pointers. A stale handle fails the lookup.

### Context

Level data first, then characters and enemies loaded on top. A stack keeps PS1 RAM and VRAM free of
fragmentation and makes unloading trivial. It fits GenV's scope (2D-first, level-based, one game at a
time). It does not suit open-world streaming, which is out of scope.

### Details

- **Shared assets** (player sprite, common SFX, fonts) belong in the app layer, or they are reloaded
  every level. Each app decides what goes there.
- Each layer declares its RAM/VRAM/SPU budget when pushed; a load that does not fit fails at scene
  entry, not mid-frame.
- The asset manager owns the stack and refcounts assets shared within a layer. It plugs in behind the
  existing `Video::createTexture/uploadTexture/releaseTexture` seam, so object code does not move.
- On PC the same API may sit on ordinary allocation, but the reference rule still applies.

## Open questions

1. Event queue size and overflow policy.
2. Does a pushed overlay scene (pause menu) get its own resource layer, or share its parent's?
3. Where do engine-owned system screens (error/info) take memory from: a reserved engine region, as the
   halt screen does today?
