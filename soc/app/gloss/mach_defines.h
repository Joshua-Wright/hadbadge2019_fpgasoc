#include <stdint.h>
#pragma once

#define MACH_RAM_START	0x40000000
#define MACH_RAM_SIZE	(8*1024)

#define UART_OFFSET		0x10000000
#define UART_DATA_REG	0x0
#define UART_DIV_REG	0x4

#define LED_OFFSET 0x20000000
#define LED_DATA_REG 0x0
#define BUTTON_READ_REG 0x4
#define BUTTON_UP (1<<0)
#define BUTTON_DOWN (1<<1)
#define BUTTON_LEFT (1<<2)
#define BUTTON_RIGHT (1<<3)
#define BUTTON_A (1<<4)
#define BUTTON_B (1<<5)
#define BUTTON_SELECT (1<<6)
#define BUTTON_START (1<<7)

#define LCD_OFFSET 0x30000000
#define LCD_COMMAND_REG 0x0
#define LCD_DATA_REG 0x4
#define LCD_CONTROL_REG 0x8
#define LCD_CONTROL_BLEN (1<<0)
#define LCD_CONTROL_RST (1<<1)
#define LCD_CONTROL_CS (1<<2)
#define LCD_CONTROL_FBSTART (1<<3)
#define LCD_CONTROL_FBENA (1<<4)
#define LCD_STATUS_REG 0xC
#define LCD_STATUS_FMARK (1<<0)
#define LCD_STATUS_ID (1<<1)
#define LCD_FB_STARTCMD 0x10

#define GFX_OFFSET 0x50000000
#define GFX_PALDEF0_REG 0x0
#define GFX_PALDEF1_REG 0x4
#define GFX_PALDEF2_REG 0x8
#define GFX_PALDEF3_REG 0xC
#define GFX_PALDEF4_REG 0x10
#define GFX_PALDEF5_REG 0x14
#define GFX_PALDEF6_REG 0x18
#define GFX_PALDEF7_REG 0x1C
#define GFX_PALDEF8_REG 0x20
#define GFX_PALDEF9_REG 0x24
#define GFX_PALDEF10_REG 0x28
#define GFX_PALDEF11_REG 0x2C
#define GFX_PALDEF12_REG 0x30
#define GFX_PALDEF13_REG 0x34
#define GFX_PALDEF14_REG 0x38
#define GFX_PALDEF15_REG 0x3C
#define GFX_FBADDR_REG 0x40


