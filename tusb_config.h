#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

// --- MCU Configuration ---
#define CFG_TUSB_MCU            OPT_MCU_RP2040
#define CFG_TUSB_OS             OPT_OS_PICO

// --- Device Configuration ---
#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#define CFG_TUD_ENABLED         1
#define CFG_TUD_MAX_SPEED       OPT_MODE_DEFAULT_SPEED

// --- HID Class Configuration ---
#define CFG_TUD_HID             1
#define CFG_TUD_HID_EP_BUFSIZE  16  // Matches the 16-byte size in your descriptor

#endif // _TUSB_CONFIG_H_