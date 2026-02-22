# PlayerManager

`IO::PlayerManager` (`playerman.hpp/cpp`) is the central player registry for GenV. It replaces the earlier `Input::VPad` class and expands its scope to cover both input and output devices.

## Responsibilities

- Maintains a static array of 9 player slots (players 0–7 plus `ARCADE_CABINET`).
- Assigns `IInputDevice` and `IOutputDevice` instances to player slots on behalf of `InputManager` and `OutputManager`.
- Provides the game-facing input read API (digital, analog, rotary).
- Tracks which player slots are active via a bitmask.

## Player slots

| Index | `Player` flag       | Meaning                              |
|-------|---------------------|--------------------------------------|
| 0–7   | `PLAYER_1`–`PLAYER_8` | Game players                       |
| 8     | `ARCADE_CABINET`    | Cabinet-level inputs/outputs (service panel, lamps, etc.) |

Each slot holds up to `maxInputDevicesPerPlayer` (4) input devices and `maxOutputDevicesPerPlayer` (4) output devices. A slot is marked available in `m_playersAvailable` as soon as it has at least one device of either type.

## Device registration

Registration is performed by `InputManager` and `OutputManager` via private methods — game code does not register devices directly.

| Method | Called by |
|---|---|
| `m_registerInputDevice(device, player)` | `InputManager::attachDevice()` |
| `m_unregisterInputDevice(device)` | `InputManager::detachDevice()` |
| `m_registerOutputDevice(device, player)` | `OutputManager::attachDevice()` |
| `m_unregisterOutputDevice(device)` | `OutputManager::detachDevice()` |

The `player` argument follows the same suggestion rules as the old `VPad`:

- `Player::ANY` — assign to the slot with the fewest devices of that type.
- `Player::PLAYER_n` — assign to that specific slot.
- `Player::PLAYER_n | Player::PLAYER_m` — assign to whichever of the listed slots has fewer devices.
- `Player::ARCADE_CABINET` — assign to the cabinet slot.

## Game-facing API

### Player availability

```cpp
int   playerCount();                        // number of slots with at least one device
Player getPlayersAvailable();               // bitmask of active slots
bool  isPlayerAvailable(Player player);     // test one slot
bool  devicesChanged();                     // true (once) if any assignment changed
void  setMaximumPlayers(uint8_t players);   // cap auto-assignment to N slots (1-8)
```

### Device lists

```cpp
PlayerInputDeviceList  getPlayerInputDevices(Player player);
PlayerOutputDeviceList getPlayerOutputDevices(Player player);
```

Returns a pointer/count pair into the slot's device arrays. Use for diagnostics or UI.

### Input reads

All calls aggregate across every input device assigned to the player (digital: OR, analog/rotary: sum).

```cpp
int getPlayerDigitalInputs(uint32_t &inputs, Player player, uint8_t bank = 0);
int getPlayerAnalogInputs (int16_t  &analog, Player player, uint8_t bank = 0);
int getPlayerRotaryInputs (int16_t  &rotary, Player player, uint8_t bank = 0);
int getPlayerDigitalCount (Player player);  // minimum digital inputs across devices
int getPlayerAnalogCount  (Player player);  // minimum analog inputs across devices
int getPlayerRotaryCount  (Player player);  // minimum rotary inputs across devices
```

Returns `GV_OK` on success, or a `GV_ERROR` code if the slot has no input devices.

## Relationship to other systems

```
InputManager  ──► PlayerManager ◄── OutputManager
                      │
              ┌───────┴───────┐
           VPad            VOutput   (future)
        (input reads)   (output writes)
```

`PlayerManager` is owned by `ServiceManager` and accessed via `getServiceManager()->getPlayerManager()`. Neither `InputManager` nor `OutputManager` owns it.
