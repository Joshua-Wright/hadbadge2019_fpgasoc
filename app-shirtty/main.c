#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "mach_defines.h"
#include "sdk.h"
#include "gfx_load.h"
#include "cache.h"

extern char _binary_tv_png_start;
extern char _binary_tv_png_end;
extern char _binary_laser_png_start;
extern char _binary_laser_png_end;

//Pointer to the framebuffer memory.
uint8_t *fbmem;


#define FB_WIDTH 512
#define FB_HEIGHT 320
#define FB_PAL_OFFSET 256

struct shirtty_mode {
    uint16_t *button_a_pulses;
    uint16_t *button_b_pulses;
    void *png_start;
    void *png_end;
};

uint16_t team_1[] = {2957, 5996, 2974, 2009, 991, 1987, 987, 2008, 992, 1987, 983, 2008, 1962, 2012, 987, 2008, 966, 0};
uint16_t team_2[] = {2935, 6037, 2957, 2038, 940, 2047, 961, 2039, 914, 2073, 1957, 2042, 910, 2094, 940, 2042, 936, 0};
uint16_t vizio_off[] = {
    9024, 4574, 536, 542, 566, 655, 479, 1780, 535, 625, 509, 573, 509, 655, 484, 650, 509, 651, 483,
    1729, 509, 1750, 535, 625, 484, 1776, 513, 1750, 510, 1724, 509, 1776, 565, 1673, 509, 599, 565,
    599, 535, 573, 565, 1694, 566, 573, 561, 598, 536, 629, 483, 599, 561, 1724, 535, 1698, 566, 1698,
    587, 577, 587, 1642, 565, 1725, 535, 1724, 539, 1694, 587, 0
};
uint16_t pulses_nop[] = {0};

struct shirtty_mode modes[] = {
    // TV B Gone mode
    {
        // Vizio Off signal
        .button_a_pulses = vizio_off,
        .button_b_pulses = pulses_nop,
        .png_start = &_binary_tv_png_start,
        .png_end = &_binary_tv_png_end

    },
    // Nerf Gun - Laser Tag mode
    {
        // Team 1
        .button_a_pulses =team_1,
        // Team 2
        .button_b_pulses = team_2,
        .png_start = &_binary_laser_png_start,
        .png_end = &_binary_laser_png_end
    },
};


// more fine grained sleep
static void delay_us(int us)
{
    // Our best scientists figured out this constant via rigorous theoretical work
    us = us * 1.16;
    for (volatile int t=0; t<us; t++);
}

/*
 * Send an IR command provided in the form of a list of pulses duration (in uS).
 * Look at the irremote circuitpython module to easily measure/generate new patterns.
 *
 * Note that this assumes that you have the ir_modulator module in your FPGA.
 * If you don't have it, nothing will happen. If you have it, the 38kHZ carrier
 * is continuously generated and we modulate the SHUTDOWN pin of the IR
 * transceiver to form the pulses.
 */
void send_ir_pulses(uint16_t pulses[]) {
    MISC_REG(MISC_GPEXT_W2S_REG) = 0xff00;

    for (int i = 0; pulses[i] > 0; i++) {
        // Start by clearing (to turn on) and then alternate
        MISC_REG(i%2 == 0 ? MISC_GPEXT_W2C_REG:MISC_GPEXT_W2S_REG) = MISC_GPEXT_OUT_REG_IRDA_SD;
        delay_us(pulses[i]);
    }
    // Set the POWERDOWN signal HIGH to shutdown the IR transmission.
    MISC_REG(MISC_GPEXT_W2S_REG) = MISC_GPEXT_OUT_REG_IRDA_SD;
}

void main(int argc, char **argv) {
    fbmem=calloc(FB_WIDTH,FB_HEIGHT);
    //Set up the framebuffer address
    GFX_REG(GFX_FBADDR_REG)=((uint32_t)fbmem);

    //We're going to use a pitch of 512 pixels, and the fb palette will start at 256.
    GFX_REG(GFX_FBPITCH_REG)=(FB_PAL_OFFSET<<GFX_FBPITCH_PAL_OFF)|(512<<GFX_FBPITCH_PITCH_OFF);

    GFX_REG(GFX_LAYEREN_REG)=0; //disable all gfx layers


    FILE *f;
    f=fopen("/dev/console", "w");
    setvbuf(f, NULL, _IONBF, 0); //make console line unbuffered

    //Now, use a library function to load the image into the framebuffer memory. This function will also set up the palette entries,
    //we tell it to start writing from entry 128.
    int png_size=(&_binary_tv_png_end-&_binary_tv_png_start);
    gfx_load_fb_mem(fbmem, &GFXPAL[FB_PAL_OFFSET], 4, 512, &_binary_tv_png_start, png_size);

    //Flush the memory region to psram so the GFX hw can stream it from there.
    cache_flush(fbmem, fbmem+FB_WIDTH*FB_HEIGHT);

    //The IPL leaves us with a tileset that has tile 0 to 127 map to ASCII characters, so we do not need to
    //load anything specific for this.

    GFX_REG(GFX_LAYEREN_REG)=GFX_LAYEREN_FB|GFX_LAYEREN_TILEA;

    // fprintf(f, "\033C");


    int current_mode_index = 0;
    struct shirtty_mode current_mode = modes[current_mode_index];

    // Press LEFT (BACK) to exit the program and go back to main menu
    while ((MISC_REG(MISC_BTN_REG) & BUTTON_LEFT)==0) {
        if (MISC_REG(MISC_BTN_REG) & BUTTON_A) {
            fprintf(f, "\033C\0335X\0335Y");
            fprintf(f, "SENDING!");
            send_ir_pulses(current_mode.button_a_pulses);
            fprintf(f, "\033C");
            delay_us(50000);//debounce
        }
        else if (MISC_REG(MISC_BTN_REG) & BUTTON_B) {
            fprintf(f, "\033C\0335X\0335Y");
            fprintf(f, "SENDING!");
            send_ir_pulses(current_mode.button_b_pulses);
            fprintf(f, "\033C");
            delay_us(50000);//debounce
        }
        else if (MISC_REG(MISC_BTN_REG) & BUTTON_SELECT) {
            current_mode_index = (current_mode_index + 1) % (sizeof(modes)/sizeof(modes[0]));
            current_mode = modes[current_mode_index];

            GFX_REG(GFX_LAYEREN_REG)=0;
            gfx_load_fb_mem(fbmem, &GFXPAL[FB_PAL_OFFSET], 4, 512, current_mode.png_start, current_mode.png_end - current_mode.png_start);
            GFX_REG(GFX_LAYEREN_REG)=GFX_LAYEREN_FB|GFX_LAYEREN_TILEA;

            //Flush the memory region to psram so the GFX hw can stream it from there.
            cache_flush(fbmem, fbmem+FB_WIDTH*FB_HEIGHT);
        }

    }
}
