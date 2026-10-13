#pragma once

#include <cstdint>

// Condemned 2 action IDs, as passed to Input_GetButton / Input_GetAxis / Input_KeyDown /
// Input_KeyHold / Input_KeyUp / Input_PlayerInput (valid range 0..201).
//
// The game has no name table for these (names live in the database and are not
// stored as plain strings), so the identifiers below are ours. Each entry's comment says how
// the meaning was established:
//   [XEX]    visible in the executable (HUD prompt table, PlayerInput switch, string)
//   [pad]    only the gamepad binding is known; the name is inferred from it
//   [guess]  inferred from surrounding code; verify before relying on it
//
// Pad bindings use XInput names. Several IDs share one pad input, because the game binds
// different IDs to the same input for different contexts.

namespace condemned2 {

enum class Action : uint32_t {
  // Digital movement (polled by the movement code next to the MOVE_/STRAFE_ strings)
  MoveForward = 0x00,   // [guess]
  MoveBackward = 0x01,  // [guess]
  MoveAxisY = 0x02,     // [XEX] Left stick Y
  MoveLeft = 0x03,      // [guess] also HUD <%move%>
  MoveRight = 0x04,     // [guess]
  StrafeAxisX = 0x05,   // [XEX] Left stick X

  // Dev/unused mouse-look axes (not bound to any pad input). Used by the native driver.
  MouseLookY = 0x0B,    // [XEX] GetAxis only, read by the mouse-look code
  MouseLookX = 0x0C,    // [XEX] GetAxis only, read by the mouse-look code (HUD <%aim%>)

  MovementToggle0E = 0x0E,  // [XEX] PlayerMovement::OnEventPlayerCommandOn toggles a movement flag (only if the player option is enabled)
  MovementToggle0F = 0x0F,  // [XEX] PlayerMovement::OnEventPlayerCommandOn (sets/clears a movement flag)
  MovementToggle12 = 0x12,  // [XEX] PlayerMovement::OnEventPlayerCommandOn toggles a movement flag
  Start = 0x0D,         // [pad] Start, pause
  Sprint = 0x10,        // [XEX] LB, HUD <%sprint%>
  Attack = 0x11,        // [XEX] LT, CS_Attack, HUD <%fire%>
  MenuLeft = 0x14,      // [pad] D-pad left (dialogs)
  MenuRight = 0x15,     // [pad] D-pad right (dialogs)
  LookAxisY = 0x16,     // [XEX] Right stick Y
  LookAxisX = 0x17,     // [XEX] Right stick X
  MovementSet19 = 0x19, // [XEX] PlayerMovement::OnEventPlayerCommandOn clears the same flag 0x1A sets
  MovementSet1A = 0x1A, // [XEX] PlayerMovement::OnEventPlayerCommandOn
  Block = 0x1C,         // [XEX] RT, CS_Block, HUD <%block%>

  // PlayerWeaponMgr::OnEventPlayerCommandOn: weapon slot 0..9 (keys 1..0 in the original design),
  // then three more quick slots, then last-weapon, grenade cycle and previous/next weapon.
  WeaponSlot0 = 0x1E, WeaponSlot1 = 0x1F, WeaponSlot2 = 0x20, WeaponSlot3 = 0x21, WeaponSlot4 = 0x22,
  WeaponSlot5 = 0x23, WeaponSlot6 = 0x24, WeaponSlot7 = 0x25, WeaponSlot8 = 0x26, WeaponSlot9 = 0x27, // [XEX]
  QuickItem0 = 0x28, QuickItem1 = 0x29, QuickItem2 = 0x2A, // [XEX] a, a+1, a+2 looked up in the item registry
  LastWeapon = 0x32,    // [XEX] swaps to the previously held weapon (the "CanLastWeapon" feature)

  ToggleMelee = 0x3C,   // [XEX] HUD <%togglemelee%>
  AmmoCheck = 0x3D,     // [XEX] Y, HUD <%ammocheck%>
  Taser = 0x3E,         // [XEX] D-pad up, HUD <%taser%>
  Throw = 0x3F,         // [pad] RB
  Grab = 0x40,          // [XEX] CS_Grab / CS_DoubleGrab
  BlockHold = 0x42,     // [pad] Left stick click, polled for block-hold

  CycleGrenade = 0x49,  // [XEX] PlayerInput case 73: next throwable (uses "IsGrenade" / "AmmoName")
  WeaponPrev = 0x4C,    // [XEX] PlayerInput case 76 / PlayerWeaponMgr: previous weapon slot
  WeaponNext = 0x4D,    // [XEX] PlayerInput case 77 / PlayerWeaponMgr: next weapon slot
  FirearmTrigger = 0x47,  // [XEX] RT; PlayerAiming CommandOn/Off(71) and the axis read set/clear the aiming flag. A hold: send OFF when released
  ObjectivesA = 0x4E,   // [pad] Back
  ObjectivesB = 0x4F,   // [pad] Back

  Use = 0x57,           // [XEX] A, HUD <%action%>
  Kick = 0x5A,          // [XEX] Right stick click, HUD <%kick%>
  MenuTabY = 0x5B,      // [pad] Y
  MenuTabX = 0x5C,      // [pad] X
  MenuUse = 0x5D,       // [XEX] A; CDecisionObjectFX::OnEventPlayerCommandOn confirms a dialogue/decision object

  CancelTask = 0x65,    // [XEX] LT; aborts the current forensic task if CanAbort
  Holster = 0x66,       // [XEX] D-pad down, "Holster" control
  Slowmo = 0x6A,        // [XEX] COMMAND_ID_SLOWMO
  ForensicsReturn = 0x70, // [XEX] PlayerInput case 112 -> PlayerForensics (sent when the tool wheel closes)
  InventoryPanel = 0x71,  // [XEX] PlayerInput case 113
  Flashlight = 0x72,    // [XEX] B, HUD <%flashlight%>
  ForensicTools = 0x74, // [XEX] X, HUD <%tools%>
  DpadUp = 0x75,        // [XEX] HUD <%dpad%>
  DpadDown = 0x76,
  DpadLeft = 0x77,
  DpadRight = 0x78,

  ComboRight = 0x79,    // [pad] RT, button form (driver uses as "left click pressed")
  ComboLeft = 0x7A,     // [pad] LT, button form (driver uses as "right click pressed")
  TuneAxisX = 0x7B,     // [XEX] Right stick X, antenna tuning (TvAntennaAdjust)
  TuneAxisY = 0x7C,     // [XEX] Right stick Y, antenna tuning

  // Menu / UI
  MenuNavUp = 0x82,     // [pad] D-pad up
  MenuNavDown = 0x83,
  MenuNavLeft = 0x84,   // also LB
  MenuNavRight = 0x85,  // also RB
  MenuStickY = 0x86,    // [XEX] Left stick Y (CMenuUI / CPlayerUI GetAxis)
  MenuConfirm = 0x87,   // [XEX] A
  MenuBack = 0x88,      // [XEX] B
  MenuStickX = 0x89,    // [XEX] Left stick X (CMenuUI / CPlayerUI GetAxis)
  MenuRtAlt = 0x8B,     // [XEX] RT; PlayerMapGPS::OnCommandOn toggles a GPS state
  MenuBAlt = 0x8C,      // [pad] B
  MenuCategoryLeft = 0x8D,   // [XEX] LB; PlayerMapGPS previous page
  MenuCategoryRight = 0x8E,  // [XEX] RB; PlayerMapGPS next page
  AltRightStickY = 0x8F,  // [pad] Right stick Y (PlayerMapGPS reads it)
  AltLeftStickY = 0x90,   // [pad] Left stick Y (PlayerMapGPS reads it). The driver also sends it for the UV quick-swap (empirical).
  AltLeftStickX = 0x91,   // [pad] Left stick X (PlayerMapGPS reads it)

  // Forensic tool slots. [XEX] PlayerUILogic::OnCommandOn maps 0x98 -> tool 1, 0x96 -> tool 2,
  // 0x97 -> tool 3, 0x99 -> tool 4 (only while the tool wheel is open). The UV/spectrometer/camera/GPS
  // names follow the driver's quick-swap.
  ToolSlotUV = 0x96,
  ToolSlotSpectrometer = 0x97,
  ToolSlotCamera = 0x98,
  ToolSlotGps = 0x99,

  ToolSlotUp = 0x9E,    // [pad] D-pad up
  ToolSlotDown = 0x9F,
  ToolSlotLeft = 0xA0,
  ToolSlotRight = 0xA1,
  MenuA = 0xA3,         // [pad] A
  MenuLt = 0xA4,        // [pad] LT
  TaskAction = 0xA6,    // [XEX] Y, PlayerInput case 166
  InGameConfirm = 0xA7, // [XEX] A; PlayerMapGPS::OnCommandOn confirm
  MenuLsClick = 0xA8,   // [XEX] Left stick click; PlayerMapGPS::OnCommandOn
  MenuY = 0xAA,         // [pad] Y
  MenuX = 0xAB,         // [pad] X
  DialogConfirmA = 0xB5,  // [pad] A / Start
  DialogConfirmB = 0xB7,  // [XEX] A; CLeaderboardChallengeFX (fight club) confirm
  DialogBack = 0xB8,    // [XEX] B; CLeaderboardChallengeFX cancel (while in its 3rd state)
  CameraShutter = 0xB9, // [XEX] A; WeaponDisplayDigitalCamera::OnEventPlayerCommandOn takes the photo on this ID
  DialogClose = 0xBA,   // [pad] B, closes tutorial dialogs
  NoteInputTime = 0xBC, // [guess] PlayerInput case 188 only stores a timestamp
};

constexpr uint32_t kMaxActionId = 201;

// Name of an action ID for logging, or "?" when the ID has no entry above.
inline const char* ActionName(uint32_t id) {
  switch (static_cast<Action>(id)) {
    case Action::MoveForward: return "MoveForward";
    case Action::MoveBackward: return "MoveBackward";
    case Action::MoveAxisY: return "MoveAxisY";
    case Action::MoveLeft: return "MoveLeft";
    case Action::MoveRight: return "MoveRight";
    case Action::StrafeAxisX: return "StrafeAxisX";
    case Action::MouseLookY: return "MouseLookY";
    case Action::MouseLookX: return "MouseLookX";
    case Action::MovementToggle0E: return "MovementToggle0E";
    case Action::MovementToggle0F: return "MovementToggle0F";
    case Action::MovementToggle12: return "MovementToggle12";
    case Action::Start: return "Start";
    case Action::Sprint: return "Sprint";
    case Action::Attack: return "Attack";
    case Action::MenuLeft: return "MenuLeft";
    case Action::MenuRight: return "MenuRight";
    case Action::LookAxisY: return "LookAxisY";
    case Action::LookAxisX: return "LookAxisX";
    case Action::MovementSet19: return "MovementSet19";
    case Action::MovementSet1A: return "MovementSet1A";
    case Action::Block: return "Block";
    case Action::WeaponSlot0: return "WeaponSlot0";
    case Action::WeaponSlot1: return "WeaponSlot1";
    case Action::WeaponSlot2: return "WeaponSlot2";
    case Action::WeaponSlot3: return "WeaponSlot3";
    case Action::WeaponSlot4: return "WeaponSlot4";
    case Action::WeaponSlot5: return "WeaponSlot5";
    case Action::WeaponSlot6: return "WeaponSlot6";
    case Action::WeaponSlot7: return "WeaponSlot7";
    case Action::WeaponSlot8: return "WeaponSlot8";
    case Action::WeaponSlot9: return "WeaponSlot9";
    case Action::QuickItem0: return "QuickItem0";
    case Action::QuickItem1: return "QuickItem1";
    case Action::QuickItem2: return "QuickItem2";
    case Action::LastWeapon: return "LastWeapon";
    case Action::ToggleMelee: return "ToggleMelee";
    case Action::AmmoCheck: return "AmmoCheck";
    case Action::Taser: return "Taser";
    case Action::Throw: return "Throw";
    case Action::Grab: return "Grab";
    case Action::BlockHold: return "BlockHold";
    case Action::CycleGrenade: return "CycleGrenade";
    case Action::WeaponPrev: return "WeaponPrev";
    case Action::WeaponNext: return "WeaponNext";
    case Action::FirearmTrigger: return "FirearmTrigger";
    case Action::ObjectivesA: return "ObjectivesA";
    case Action::ObjectivesB: return "ObjectivesB";
    case Action::Use: return "Use";
    case Action::Kick: return "Kick";
    case Action::MenuTabY: return "MenuTabY";
    case Action::MenuTabX: return "MenuTabX";
    case Action::MenuUse: return "MenuUse";
    case Action::CancelTask: return "CancelTask";
    case Action::Holster: return "Holster";
    case Action::Slowmo: return "Slowmo";
    case Action::ForensicsReturn: return "ForensicsReturn";
    case Action::InventoryPanel: return "InventoryPanel";
    case Action::Flashlight: return "Flashlight";
    case Action::ForensicTools: return "ForensicTools";
    case Action::DpadUp: return "DpadUp";
    case Action::DpadDown: return "DpadDown";
    case Action::DpadLeft: return "DpadLeft";
    case Action::DpadRight: return "DpadRight";
    case Action::ComboRight: return "ComboRight";
    case Action::ComboLeft: return "ComboLeft";
    case Action::TuneAxisX: return "TuneAxisX";
    case Action::TuneAxisY: return "TuneAxisY";
    case Action::MenuNavUp: return "MenuNavUp";
    case Action::MenuNavDown: return "MenuNavDown";
    case Action::MenuNavLeft: return "MenuNavLeft";
    case Action::MenuNavRight: return "MenuNavRight";
    case Action::MenuStickY: return "MenuStickY";
    case Action::MenuConfirm: return "MenuConfirm";
    case Action::MenuBack: return "MenuBack";
    case Action::MenuStickX: return "MenuStickX";
    case Action::MenuRtAlt: return "MenuRtAlt";
    case Action::MenuBAlt: return "MenuBAlt";
    case Action::MenuCategoryLeft: return "MenuCategoryLeft";
    case Action::MenuCategoryRight: return "MenuCategoryRight";
    case Action::AltRightStickY: return "AltRightStickY";
    case Action::AltLeftStickY: return "AltLeftStickY";
    case Action::AltLeftStickX: return "AltLeftStickX";
    case Action::ToolSlotUV: return "ToolSlotUV";
    case Action::ToolSlotSpectrometer: return "ToolSlotSpectrometer";
    case Action::ToolSlotCamera: return "ToolSlotCamera";
    case Action::ToolSlotGps: return "ToolSlotGps";
    case Action::ToolSlotUp: return "ToolSlotUp";
    case Action::ToolSlotDown: return "ToolSlotDown";
    case Action::ToolSlotLeft: return "ToolSlotLeft";
    case Action::ToolSlotRight: return "ToolSlotRight";
    case Action::MenuA: return "MenuA";
    case Action::MenuLt: return "MenuLt";
    case Action::TaskAction: return "TaskAction";
    case Action::InGameConfirm: return "InGameConfirm";
    case Action::MenuLsClick: return "MenuLsClick";
    case Action::MenuY: return "MenuY";
    case Action::MenuX: return "MenuX";
    case Action::DialogConfirmA: return "DialogConfirmA";
    case Action::DialogConfirmB: return "DialogConfirmB";
    case Action::DialogBack: return "DialogBack";
    case Action::CameraShutter: return "CameraShutter";
    case Action::DialogClose: return "DialogClose";
    case Action::NoteInputTime: return "NoteInputTime";
    default: return "?";
  }
}

constexpr uint32_t ToId(Action a) { return static_cast<uint32_t>(a); }

}  // namespace condemned2
