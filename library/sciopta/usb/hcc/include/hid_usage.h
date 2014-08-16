/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef _HID_USAGE_H_
#define _HID_USAGE_H_

#define UV(v)   ((hcc_u16)(v))
/****************************************************/
/* HID 1.11 usage pages                             */
/****************************************************/

#define HIDUPG_UNDEFINED  UV(0x00)   /* Undefined */
/**** Top level pages */
#define HIDUPG_GEN_DES    UV(0x01)   /* Generic Desktop Controls*/
#define HIDUPG_SIM_CTR    UV(0x02)   /* Simulation Controls */
#define HIDUPG_VR_CTR     UV(0x03)   /* VR Controls */
#define HIDUPG_SPORT_CTR  UV(0x04)   /* Sport Controls */
#define HIDUPG_GAME_CTR   UV(0x05)   /* Game Controls */
#define HIDUPG_GEN_DEV    UV(0x06)   /* Generic Device Controls */
#define HIDUPG_KEYB       UV(0x07)   /* Keyboard/Keypad */
#define HIDUPG_LED        UV(0x08)   /* LEDs */
#define HIDUPG_BUTTON     UV(0x09)   /* Button */
#define HIDUPG_ORDINAL    UV(0x0A)   /* Ordinal */
#define HIDUPG_PHONE      UV(0x0B)   /* Telephony */
#define HIDUPG_CONSUMER   UV(0x0C)   /* Consumer */
#define HIDUPG_DIGITIZER  UV(0x0D)   /* Digitizer*/
/* 0E    Reserved */
#define HIDUPG_PID        UV(0x0F)   /* PID Page (force feedback and related devices) */
#define HIDUPG_UNICODE    UV(0x10)   /* Unicode */
/* 11-13 Reserved */
#define HIDUPG_ALNUM_DISP UV(0x14)   /* Alphanumeric Display */
/* 15-1f Reserved */
/**** END of top level pages */
/* 25-3f Reserved */
#define HIDUPG_MEDICAL    UV(0x40)   /* Medical Instruments */
/* 41-7F Reserved */
/*80-83 Monitor pages USB Device Class Definition for Monitor Devices
  84-87 Power pages USB Device Class Definition for Power Devices */
/* 88-8B Reserved */
#define HIDUPG_BARCODE    UV(0x8C)   /* Bar Code Scanner page */
#define HIDUPG_SCALE      UV(0x8D)   /* Scale page */
#define HIDUPG_MSR        UV(0x8E)   /* Magnetic Stripe Reading (MSR) Devices */
#define HIDUPG_POS        UV(0x8F)   /* Reserved Point of Sale pages */
#define HIDUPG_CAMERA_CTR UV(0x90)   /* Camera Control Page */
#define HIDUPG_ARCADE     UV(0x91)   /* Arcade Page */
#define HIDUPG_VENDOR     UV(0xff00)

/****************************************************/
/* Usage definitions for the "Generic Decktop" page */
/****************************************************/
#define HIDU_UNDEFINED     UV(0x00)   /* Undefined */
#define HIDU_POINTER       UV(0x01)   /* Pointer (Physical Collection) */
#define HIDU_MOUSE         UV(0x02)   /* Mouse (Application Collection) */
/* 03 Reserved */
#define HIDU_JOYSTICK      UV(0x04)   /* Joystick (Application Collection) */
#define HIDU_GAMEPAD       UV(0x05)   /* Game Pad (Application Collection) */
#define HIDU_KBD           UV(0x06)   /* Keyboard (Application Collection) */
#define HIDU_KEYPAD        UV(0x07)   /* Keypad (Application Collection) */
#define HIDU_MAX_CTR       UV(0x08)   /* Multi-axis Controller (Application Collection) */
/* 09-2F Reserved */
#define HIDU_X             UV(0x30)   /* X (Dynamic Value) */
#define HIDU_Y             UV(0x31)   /* Y (Dynamic Value) */
#define HIDU_Z             UV(0x32)   /* Z (Dynamic Value) */
#define HIDU_RX            UV(0x33)   /* Rx (Dynamic Value) */
#define HIDU_RY            UV(0x34)   /* Ry (Dynamic Value) */
#define HIDU_RZ            UV(0x35)   /* Rz (Dynamic Value) */
#define HIDU_SLIDER        UV(0x36)   /* Slider (Dynamic Value) */
#define HIDU_DIAL          UV(0x37)   /* Dial (Dynamic Value) */
#define HIDU_WHEEL         UV(0x38)   /* Wheel (Dynamic Value) */
#define HIDU_HATSW         UV(0x39)   /* Hat switch (Dynamic Value) */
#define HIDU_COUNTEDBUF    UV(0x3A)   /* Counted Buffer (Logical Collection) */
#define HIDU_BYTECOUNT     UV(0x3B)   /* Byte Count (Dynamic Value) */
#define HIDU_MOTIONWAKE    UV(0x3C)   /* Motion Wakeup (One Shot Control) */
#define HIDU_START         UV(0x3D)   /* Start (On/Off Control) */
#define HIDU_SELECT        UV(0x3E)   /* Select (On/Off Control) */
/* 3F Reserved */
#define HIDU_VX            UV(0x40)   /* Vx (Dynamic Value) */
#define HIDU_VY            UV(0x41)   /* Vy (Dynamic Value) */
#define HIDU_VZ            UV(0x42)   /* Vz (Dynamic Value) */
#define HIDU_VBRX          UV(0x43)   /* Vbrx (Dynamic Value) */
#define HIDU_VBRY          UV(0x44)   /* Vbry (Dynamic Value) */
#define HIDU_VBRZ          UV(0x45)   /* Vbrz (Dynamic Value) */
#define HIDU_VNO           UV(0x46)   /* Vno (Dynamic Value) */
#define HIDU_FEATNOTIF     UV(0x47)   /* Feature Notification (Dynamic Value),(Dynamic Flag) */
/* 48-7F Reserved */
#define HIDU_SYSCTL        UV(0x80)   /* System Control (Application Collection) */
#define HIDU_PWDOWN        UV(0x81)   /* System Power Down (One Shot Control) */
#define HIDU_SLEEP         UV(0x82)   /* System Sleep (One Shot Control) */
#define HIDU_WAKEUP        UV(0x83)   /* System Wake Up (One Shot Control)  */
#define HIDU_CONTEXTM      UV(0x84)   /* System Context Menu (One Shot Control) */
#define HIDU_MAINM         UV(0x85)   /* System Main Menu (One Shot Control) */
#define HIDU_APPM          UV(0x86)   /* System App Menu (One Shot Control) */
#define HIDU_MENUHELP      UV(0x87)   /* System Menu Help (One Shot Control) */
#define HIDU_MENUEXIT      UV(0x88)   /* System Menu Exit (One Shot Control) */
#define HIDU_MENUSELECT    UV(0x89)   /* System Menu Select (One Shot Control) */
#define HIDU_SYSM_RIGHT    UV(0x8A)   /* System Menu Right (Re-Trigger Control) */
#define HIDU_SYSM_LEFT     UV(0x8B)   /* System Menu Left (Re-Trigger Control) */
#define HIDU_SYSM_UP       UV(0x8C)   /* System Menu Up (Re-Trigger Control) */
#define HIDU_SYSM_DOWN     UV(0x8D)   /* System Menu Down (Re-Trigger Control) */
#define HIDU_COLDRESET     UV(0x8E)   /* System Cold Restart (One Shot Control) */
#define HIDU_WARMRESET     UV(0x8F)   /* System Warm Restart (One Shot Control) */
#define HIDU_DUP           UV(0x90)   /* D-pad Up (On/Off Control) */
#define HIDU_DDOWN         UV(0x91)   /* D-pad Down (On/Off Control) */
#define HIDU_DRIGHT        UV(0x92)   /* D-pad Right (On/Off Control) */
#define HIDU_DLEFT         UV(0x93)   /* D-pad Left (On/Off Control) */
/* 94-9F Reserved */
#define HIDU_SYS_DOCK      UV(0xA0)   /* System Dock (One Shot Control) */
#define HIDU_SYS_UNDOCK    UV(0xA1)   /* System Undock (One Shot Control) */
#define HIDU_SYS_SETUP     UV(0xA2)   /* System Setup (One Shot Control) */
#define HIDU_SYS_BREAK     UV(0xA3)   /* System Break (One Shot Control) */
#define HIDU_SYS_DBGBRK    UV(0xA4)   /* System Debugger Break (One Shot Control) */
#define HIDU_APP_BRK       UV(0xA5)   /* Application Break (One Shot Control) */
#define HIDU_APP_DBGBRK    UV(0xA6)   /* Application Debugger Break (One Shot Control) */
#define HIDU_SYS_SPKMUTE   UV(0xA7)   /* System Speaker Mute (One Shot Control) */
#define HIDU_SYS_HIBERN    UV(0xA8)   /* System Hibernate (One Shot Control) */
/* A9-AF Reserved */
#define HIDU_SYS_SIDPINV   UV(0xB0)   /* System Display Invert (One Shot Control) */
#define HIDU_SYS_DISPINT   UV(0xB1)   /* System Display Internal (One Shot Control) */
#define HIDU_SYS_DISPEXT   UV(0xB2)   /* System Display External (One Shot Control) */
#define HIDU_SYS_DISPBOTH  UV(0xB3)   /* System Display Both (One Shot Control) */
#define HIDU_SYS_DISPDUAL  UV(0xB4)   /* System Display Dual (One Shot Control) */
#define HIDU_SYS_DISPTGLIE UV(0xB5)   /* System Display Toggle Int/Ext (One Shot Control) */
#define HIDU_SYS_DISP_SWAP UV(0xB6)   /* System Display Swap Primary/Secondary (One Shot Control) */
#define HIDU_SYS_DIPS_LCDA UV(0xB7)   /* System Display LCD Autoscale (One Shot Control) */
/* B8-FFFF Reserved */

/****************************************************/
/* Usage definitions for the "Keyboard" page */
/****************************************************/
#define KEY_NONE                               0x00
#define KEY_ERRORROLLOVER                      0x01
#define KEY_POSTFAIL                           0x02
#define KEY_ERRORUNDEFINED                     0x03
#define KEY_A                                  0x04
#define KEY_B                                  0x05
#define KEY_C                                  0x06
#define KEY_D                                  0x07
#define KEY_E                                  0x08
#define KEY_F                                  0x09
#define KEY_G                                  0x0A
#define KEY_H                                  0x0B
#define KEY_I                                  0x0C
#define KEY_J                                  0x0D
#define KEY_K                                  0x0E
#define KEY_L                                  0x0F
#define KEY_M                                  0x10
#define KEY_N                                  0x11
#define KEY_O                                  0x12
#define KEY_P                                  0x13
#define KEY_Q                                  0x14
#define KEY_R                                  0x15
#define KEY_S                                  0x16
#define KEY_T                                  0x17
#define KEY_U                                  0x18
#define KEY_V                                  0x19
#define KEY_W                                  0x1A
#define KEY_X                                  0x1B
#define KEY_Y                                  0x1C
#define KEY_Z                                  0x1D
#define KEY_1_EXCLAMATION_MARK                 0x1E
#define KEY_2_AT                               0x1F
#define KEY_3_NUMBER_SIGN                      0x20
#define KEY_4_DOLLAR                           0x21
#define KEY_5_PERCENT                          0x22
#define KEY_6_CARET                            0x23
#define KEY_7_AMPERSAND                        0x24
#define KEY_8_ASTERISK                         0x25
#define KEY_9_OPARENTHESIS                     0x26
#define KEY_0_CPARENTHESIS                     0x27
#define KEY_ENTER                              0x28
#define KEY_ESCAPE                             0x29
#define KEY_BACKSPACE                          0x2A
#define KEY_TAB                                0x2B
#define KEY_SPACEBAR                           0x2C
#define KEY_MINUS_UNDERSCORE                   0x2D
#define KEY_EQUAL_PLUS                         0x2E
#define KEY_OBRACKET_AND_OBRACE                0x2F
#define KEY_CBRACKET_AND_CBRACE                0x30
#define KEY_BACKSLASH_VERTICAL_BAR             0x31
#define KEY_NONUS_NUMBER_SIGN_TILDE            0x32
#define KEY_SEMICOLON_COLON                    0x33
#define KEY_SINGLE_AND_DOUBLE_QUOTE            0x34
#define KEY_GRAVE ACCENT AND TILDE             0x35
#define KEY_COMMA_AND_LESS                     0x36
#define KEY_DOT_GREATER                        0x37
#define KEY_SLASH_QUESTION                     0x38
#define KEY_CAPS LOCK                          0x39
#define KEY_F1                                 0x3A
#define KEY_F2                                 0x3B
#define KEY_F3                                 0x3C
#define KEY_F4                                 0x3D
#define KEY_F5                                 0x3E
#define KEY_F6                                 0x3F
#define KEY_F7                                 0x40
#define KEY_F8                                 0x41
#define KEY_F9                                 0x42
#define KEY_F10                                0x43
#define KEY_F11                                0x44
#define KEY_F12                                0x45
#define KEY_PRINTSCREEN                        0x46
#define KEY_SCROLL LOCK                        0x47
#define KEY_PAUSE                              0x48
#define KEY_INSERT                             0x49
#define KEY_HOME                               0x4A
#define KEY_PAGEUP                             0x4B
#define KEY_DELETE                             0x4C
#define KEY_END1                               0x4D
#define KEY_PAGEDOWN                           0x4E
#define KEY_RIGHTARROW                         0x4F
#define KEY_LEFTARROW                          0x50
#define KEY_DOWNARROW                          0x51
#define KEY_UPARROW                            0x52
#define KEY_KEYPAD_NUM_LOCK_AND_CLEAR          0x53
#define KEY_KEYPAD_SLASH                       0x54
#define KEY_KEYPAD_ASTERIKS                    0x55
#define KEY_KEYPAD_MINUS                       0x56
#define KEY_KEYPAD_PLUS                        0x57
#define KEY_KEYPAD_ENTER                       0x58
#define KEY_KEYPAD_1_END                       0x59
#define KEY_KEYPAD_2_DOWN_ARROW                0x5A
#define KEY_KEYPAD_3_PAGEDN                    0x5B
#define KEY_KEYPAD_4_LEFT_ARROW                0x5C
#define KEY_KEYPAD_5                           0x5D
#define KEY_KEYPAD_6_RIGHT_ARROW               0x5E
#define KEY_KEYPAD_7_HOME                      0x5F
#define KEY_KEYPAD_8_UP_ARROW                  0x60
#define KEY_KEYPAD_9_PAGEUP                    0x61
#define KEY_KEYPAD_0_INSERT                    0x62
#define KEY_KEYPAD_DECIMAL_SEPARATOR_DELETE    0x63
#define KEY_NONUS_BACK_SLASH_VERTICAL_BAR      0x64
#define KEY_APPLICATION                        0x65
#define KEY_POWER                              0x66
#define KEY_KEYPAD_EQUAL                       0x67
#define KEY_F13                                0x68
#define KEY_F14                                0x69
#define KEY_F15                                0x6A
#define KEY_F16                                0x6B
#define KEY_F17                                0x6C
#define KEY_F18                                0x6D
#define KEY_F19                                0x6E
#define KEY_F20                                0x6F
#define KEY_F21                                0x70
#define KEY_F22                                0x71
#define KEY_F23                                0x72
#define KEY_F24                                0x73
#define KEY_EXECUTE                            0x74
#define KEY_HELP                               0x75
#define KEY_MENU                               0x76
#define KEY_SELECT                             0x77
#define KEY_STOP                               0x78
#define KEY_AGAIN                              0x79
#define KEY_UNDO                               0x7A
#define KEY_CUT                                0x7B
#define KEY_COPY                               0x7C
#define KEY_PASTE                              0x7D
#define KEY_FIND                               0x7E
#define KEY_MUTE                               0x7F
#define KEY_VOLUME_UP                          0x80
#define KEY_VOLUME_DOWN                        0x81
#define KEY_LOCKING_CAPS_LOCK                  0x82
#define KEY_LOCKING_NUM_LOCK                   0x83
#define KEY_LOCKING_SCROLL_LOCK                0x84
#define KEY_KEYPAD_COMMA                       0x85
#define KEY_KEYPAD_EQUAL_SIGN                  0x86
#define KEY_INTERNATIONAL1                     0x87
#define KEY_INTERNATIONAL2                     0x88
#define KEY_INTERNATIONAL3                     0x89
#define KEY_INTERNATIONAL4                     0x8A
#define KEY_INTERNATIONAL5                     0x8B
#define KEY_INTERNATIONAL6                     0x8C
#define KEY_INTERNATIONAL7                     0x8D
#define KEY_INTERNATIONAL8                     0x8E
#define KEY_INTERNATIONAL9                     0x8F
#define KEY_LANG1                              0x90
#define KEY_LANG2                              0x91
#define KEY_LANG3                              0x92
#define KEY_LANG4                              0x93
#define KEY_LANG5                              0x94
#define KEY_LANG6                              0x95
#define KEY_LANG7                              0x96
#define KEY_LANG8                              0x97
#define KEY_LANG9                              0x98
#define KEY_ALTERNATE_ERASE                    0x99
#define KEY_SYSREQ                             0x9A
#define KEY_CANCEL                             0x9B
#define KEY_CLEAR                              0x9C
#define KEY_PRIOR                              0x9D
#define KEY_RETURN                             0x9E
#define KEY_SEPARATOR                          0x9F
#define KEY_OUT                                0xA0
#define KEY_OPER                               0xA1
#define KEY_CLEAR_AGAIN                        0xA2
#define KEY_CRSEL                              0xA3
#define KEY_EXSEL                              0xA4
#define KEY_KEYPAD_00                          0xB0
#define KEY_KEYPAD_000                         0xB1
#define KEY_THOUSANDS_SEPARATOR                0xB2
#define KEY_DECIMAL_SEPARATOR                  0xB3
#define KEY_CURRENCY_UNIT                      0xB4
#define KEY_CURRENCY_SUB_UNIT                  0xB5
#define KEY_KEYPAD_OPARENTHESIS                0xB6
#define KEY_KEYPAD_CPARENTHESIS                0xB7
#define KEY_KEYPAD_OBRACE                      0xB8
#define KEY_KEYPAD_CBRACE                      0xB9
#define KEY_KEYPAD_TAB                         0xBA
#define KEY_KEYPAD_BACKSPACE                   0xBB
#define KEY_KEYPAD_A                           0xBC
#define KEY_KEYPAD_B                           0xBD
#define KEY_KEYPAD_C                           0xBE
#define KEY_KEYPAD_D                           0xBF
#define KEY_KEYPAD_E                           0xC0
#define KEY_KEYPAD_F                           0xC1
#define KEY_KEYPAD_XOR                         0xC2
#define KEY_KEYPAD_CARET                       0xC3
#define KEY_KEYPAD_PERCENT                     0xC4
#define KEY_KEYPAD_LESS                        0xC5
#define KEY_KEYPAD_GREATER                     0xC6
#define KEY_KEYPAD_AMPERSAND                   0xC7
#define KEY_KEYPAD_LOGICAL_AND                 0xC8
#define KEY_KEYPAD_VERTICAL_BAR                0xC9
#define KEY_KEYPAD_LOGIACL_OR                  0xCA
#define KEY_KEYPAD_COLON                       0xCB
#define KEY_KEYPAD_NUMBER_SIGN                 0xCC
#define KEY_KEYPAD_SPACE                       0xCD
#define KEY_KEYPAD_AT                          0xCE
#define KEY_KEYPAD_EXCLAMATION_MARK            0xCF
#define KEY_KEYPAD_MEMORY_STORE                0xD0
#define KEY_KEYPAD_MEMORY_RECALL               0xD1
#define KEY_KEYPAD_MEMORY_CLEAR                0xD2
#define KEY_KEYPAD_MEMORY_ADD                  0xD3
#define KEY_KEYPAD_MEMORY_SUBTRACT             0xD4
#define KEY_KEYPAD_MEMORY_MULTIPLY             0xD5
#define KEY_KEYPAD_MEMORY_DIVIDE               0xD6
#define KEY_KEYPAD_PLUSMINUS                   0xD7
#define KEY_KEYPAD_CLEAR                       0xD8
#define KEY_KEYPAD_CLEAR_ENTRY                 0xD9
#define KEY_KEYPAD_BINARY                      0xDA
#define KEY_KEYPAD_OCTAL                       0xDB
#define KEY_KEYPAD_DECIMAL                     0xDC
#define KEY_KEYPAD_HEXADECIMAL                 0xDD
#define KEY_LEFTCONTROL                        0xE0
#define KEY_LEFTSHIFT                          0xE1
#define KEY_LEFTALT                            0xE2
#define KEY_LEFT_GUI                           0xE3
#define KEY_RIGHTCONTROL                       0xE4
#define KEY_RIGHTSHIFT                         0xE5
#define KEY_RIGHTALT                           0xE6
#define KEY_RIGHT_GUI                          0xE7

/****************************************************/
/* Usage definitions for the "LED" page */
/****************************************************/
#define LED_UNDEFINED                          0x00
#define NUM_LOCK                               0x01
#define CAPS_LOCK                              0x02
#define SCROLL_LOCK                            0x03
#define COMPOSE                                0x04
#define KANA                                   0x05
#define POWER                                  0x06
#define SHIFT                                  0x07
#define DO_NOT_DISTURB                         0x08
#define MUTE                                   0x09
#define TONE_ENABLE                            0x0A
#define HIGH_CUT_FILTER                        0x0B
#define LOW_CUT_FILTER                         0x0C
#define EQUALIZER_ENABLE                       0x0D
#define SOUND_FIELD_ON                         0x0E
#define SURROUND_FIELD_ON                      0x0F
#define REPEAT                                 0x10
#define STEREO                                 0x11
#define SAMPLING_RATE_DETECT                   0x12
#define SPINNING                               0x13
#define CAV                                    0x14
#define CLV                                    0x15
#define RECORDING_FORMAT_DETECT                0x16
#define OFF_HOOK                               0x17
#define RING                                   0x18
#define MESSAGE_WAITING                        0x19
#define DATA_MODE                              0x1A
#define BATTERY_OPERATION                      0x1B
#define BATTERY_OK                             0x1C
#define BATTERY_LOW                            0x1D
#define SPEAKER                                0x1E
#define HEAD_SET                               0x1F
#define HOLD                                   0x20
#define MICROPHONE                             0x21
#define COVERAGE                               0x22
#define NIGHT_MODE                             0x23
#define SEND_CALLS                             0x24
#define CALL_PICKUP                            0x25
#define CONFERENCE                             0x26
#define STAND_BY                               0x27
#define CAMERA_ON                              0x28
#define CAMERA_OFF                             0x29
#define ON_LINE                                0x2A
#define OFF_LINE                               0x2B
#define BUSY                                   0x2C
#define READY                                  0x2D
#define PAPER_OUT                              0x2E
#define PAPER_JAM                              0x2F
#define REMOTE                                 0x30
#define FORWARD                                0x31
#define REVERSE                                0x32
#define STOP                                   0x33
#define REWIND                                 0x34
#define FAST_FORWARD                           0x35
#define PLAY                                   0x36
#define PAUSE                                  0x37
#define RECORD                                 0x38
#define ERROR                                  0x39
#define USAGE_SELECTED_INDICATOR               0x3A
#define USAGE_IN_USE_INDICATOR                 0x3B
#define USAGE_MULTI_MODE INDICATOR             0x3C
#define INDICATOR_ON                           0x3D
#define INDICATOR_FLASH                        0x3E
#define INDICATOR_SLOW BLINK                   0x3F
#define INDICATOR_FAST BLINK                   0x40
#define INDICATOR_OFF                          0x41
#define FLASH_ON_TIME                          0x42
#define SLOW_BLINK_ON_TIME                     0x43
#define SLOW_BLINK_OFF_TIME                    0x44
#define FAST_BLINK_ON_TIME                     0x45
#define FAST_BLINK_OFF_TIME                    0x46
#define USAGE_INDICATOR_COLOR                  0x47
#define RED                                    0x48
#define GREEN                                  0x49
#define AMBER                                  0x4A
#define GENERIC_INDICATOR                      0x4B
/*4C-FFFF	Reserved*/

/****************************************************/
/* Usage definitions for the "Consumer" page */
/****************************************************/
#define CONSUMER_MUTE							0xe2
#define CONSUMER_VOLUME_INCREMENT				0xe9
#define CONSUMER_VOLUME_DECREMENT				0xea

#endif

