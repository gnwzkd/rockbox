/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 Robert E. Hak
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include <lcd.h>

#include "icons.h"
#ifndef SIMULATOR
#include "sprintf.h"
#endif
#ifdef HAVE_RTC
#include "rtc.h"
#endif

#ifdef HAVE_LCD_BITMAP

unsigned char slider_bar[] =
{
    0x38, 0x28, 0x28, 0x28, 0x28,
    0x7c, 0x28, 0x28, 0x28, 0x28,
    0x7c, 0x28, 0x28, 0x28, 0x28,
    0x7c, 0x28, 0x28, 0x28, 0x28,
    0x7c, 0x28, 0x28, 0x28, 0x28,
    0x7c, 0x28, 0x28, 0x28, 0x28,
    0x7c, 0x28, 0x28, 0x28, 0x28,
    0x7c, 0x28, 0x28, 0x28, 0x28, 0x38
};

static unsigned char bitmap_icon_5x8[][5] =
{
    /* Lock */
    {0x78,0x7f,0x49,0x7f,0x78}
};

unsigned char bitmap_icons_6x8[LastIcon][6] =
{
    /* Box_Filled */
    { 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f }, 
    /* Box_Empty */
    { 0x00, 0x7f, 0x41, 0x41, 0x41, 0x7f }, 
    /* Slider_Horizontal */
    { 0x00, 0x3e, 0x7f, 0x63, 0x7f, 0x3e }, 
    /* File */
    { 0x60, 0x7f, 0x03, 0x63, 0x7f, 0x00 },
    /* Folder */
    { 0x7e, 0x41, 0x41, 0x42, 0x7e, 0x00 },
    /* Directory */
    { 0x3e, 0x26, 0x26, 0x24, 0x3c, 0x00 },
    /* Playlist */
    { 0x55, 0x00, 0x55, 0x55, 0x55, 0x00 }, 
    /* Repeat */
    { 0x39, 0x43, 0x47, 0x71, 0x61, 0x4e }, 
    /* Selected */
    { 0x00, 0x1c, 0x3e, 0x3e, 0x3e, 0x1c },
    /* Cursor / Marker */
    { 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x00 },
};

static unsigned char bitmap_icon_7x8[][7] =
{
    /* Power plug */
    {0x08,0x1c,0x1c,0x3e,0x3e,0x14,0x14},
    /* Speaker */
    {0x00,0x1c,0x1c,0x3e,0x7f,0x00,0x00},
    /* Speaker mute */
    {0x01,0x1e,0x1c,0x3e,0x7f,0x20,0x40},
    /* Play */
    {0x00,0x7f,0x7f,0x3e,0x1c,0x08,0x00},
    /* Stop */
    {0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f},
    /* Pause */
    {0x00,0x7f,0x7f,0x00,0x7f,0x7f,0x00},
    /* Fast forward */
    {0x7f,0x3e,0x1c,0x7f,0x3e,0x1c,0x08},
    /* Fast backward */
    {0x08,0x1c,0x3e,0x7f,0x1c,0x3e,0x7f},
    /* Record */
    {0x1c,0x3e,0x7f,0x7f,0x7f,0x3e,0x1c},
    /* Record pause */
    {0x1c,0x3e,0x7f,0x00,0x7f,0x3e,0x1c},
    /* Normal playmode */
    {0x08,0x08,0x08,0x08,0x3e,0x1c,0x08},
    /* Repeat playmode */
    {0x38,0x44,0x44,0x4e,0x5f,0x44,0x38},
    /* Shuffle playmode (dice) */
    {0x3e,0x41,0x51,0x41,0x45,0x41,0x3e}
};

unsigned char rockbox112x37[]={
 0x00, 0x00, 0x02, 0xff, 0x02, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa,
 0xf8, 0xf8, 0xf0, 0xe0, 0x80, 0x00, 0x00, 0x80, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc,
 0x7c, 0x7d, 0xfd, 0xfa, 0xfa, 0xf4, 0xe8, 0x90, 0x60, 0x80, 0xe0, 0x10, 0xc8,
 0xe4, 0xf2, 0xfa, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfa, 0xfa, 0xf4, 0x02, 0xfa,
 0xfa, 0xfa, 0xfa, 0x02, 0xff, 0x02, 0x00, 0x80, 0xe2, 0xfa, 0xfa, 0xfa, 0xfa,
 0x3a, 0x0e, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

 0x60, 0x90, 0x20, 0xc0, 0x00, 0xff, 0xff, 0xff, 0xff, 0x05, 0x05, 0x05, 0xf9,
 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0xfc, 0xff, 0xff, 0xff, 0x0f, 0x01, 0x00,
 0xff, 0x01, 0x0e, 0xf1, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x03, 0xf8, 0xff, 0xff,
 0xff, 0x0f, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x0f, 0x00, 0xff,
 0xff, 0xff, 0xff, 0x80, 0xff, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0x07, 0x07, 0x04,
 0x04, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

 0xc0, 0x38, 0x07, 0x9d, 0x60, 0xbf, 0xbf, 0xff, 0xff, 0xfc, 0xff, 0xfd, 0xfe,
 0xff, 0xff, 0x9f, 0x0f, 0x03, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x08, 0x08,
 0x3f, 0x08, 0x08, 0xff, 0x08, 0xff, 0xff, 0xff, 0xff, 0x08, 0xff, 0xff, 0xff,
 0xff, 0x08, 0x08, 0x08, 0x08, 0xfe, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0xff,
 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xf3, 0xc0, 0xff, 0x00, 0x00, 0x00,
 0x00, 0x03, 0x82, 0x41, 0x41, 0xa1, 0xa1, 0x41, 0x41, 0x81, 0x02, 0x02, 0x04,
 0x08, 0x30, 0x08, 0x04, 0x02, 0x02, 0x81, 0x41, 0x41, 0xa1, 0xa1, 0x41, 0x41,
 0x81, 0x01, 0x03, 0x05, 0x01, 0x01, 0x01, 0x01, 0x02, 0x0c, 0x12, 0x0d, 0x02,
 0x01, 0x01, 0xc1, 0x31, 0xc9, 0x35, 0x0b, 0x04, 

 0x01, 0x07, 0x0c, 0x09, 0x18, 0xe3, 0x1b, 0xfc, 0xff, 0x00, 0xff, 0x03, 0x1f,
 0x7f, 0xff, 0xff, 0xfc, 0xf0, 0x80, 0x0f, 0x7f, 0xff, 0xff, 0xfc, 0xe0, 0xc0,
 0xa0, 0xa0, 0xdc, 0xe3, 0xfc, 0xff, 0xff, 0x7f, 0x0f, 0x00, 0x07, 0x3f, 0xff,
 0xff, 0xfc, 0xf0, 0xe0, 0xc0, 0xff, 0xc0, 0xc0, 0xe0, 0xf0, 0xfc, 0x00, 0xff,
 0xff, 0xff, 0xff, 0x06, 0x19, 0x67, 0x9f, 0x7f, 0xff, 0xff, 0xfc, 0xf0, 0xc0,
 0x00, 0x06, 0x19, 0x20, 0x20, 0x50, 0x50, 0x29, 0x26, 0x19, 0x06, 0x00, 0x00,
 0x00, 0xc0, 0x00, 0x00, 0x00, 0x06, 0x19, 0x20, 0x20, 0x50, 0x50, 0x29, 0x26,
 0x19, 0x06, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x06, 0x00, 0x00, 0x80, 0x00, 0x00,
 0x06, 0x09, 0x36, 0xc9, 0x30, 0xc0, 0x00, 0x00, 

 0x20, 0xa0, 0x00, 0x40, 0x83, 0xec, 0x0c, 0x0f, 0x0f, 0xe8, 0xff, 0xa8, 0x08,
 0x00, 0x01, 0x0f, 0x0f, 0x0f, 0x0f, 0x0e, 0x58, 0xc9, 0x03, 0x47, 0x07, 0xef,
 0xef, 0xaf, 0x0f, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x08, 0x48, 0xe8, 0xe8,
 0xa9, 0x4b, 0xef, 0xef, 0xaf, 0xaf, 0xaf, 0x07, 0x47, 0x27, 0xc3, 0x00, 0x4f,
 0x8f, 0xef, 0x0f, 0x00, 0x00, 0xe8, 0xe9, 0xae, 0x19, 0x0f, 0x0f, 0x0f, 0x0f,
 0x0f, 0x0c, 0x04, 0x48, 0xc8, 0x08, 0x48, 0x68, 0x48, 0x08, 0x04, 0x24, 0xe2,
 0xe1, 0xa0, 0x61, 0x42, 0x04, 0x04, 0x88, 0x28, 0x08, 0x08, 0x28, 0xe8, 0xe8,
 0xa8, 0xc8, 0xec, 0xea, 0xa8, 0x28, 0xa8, 0x08, 0x44, 0xeb, 0x24, 0x03, 0x04,
 0x08, 0xe8, 0xe8, 0xa8, 0x09, 0x0a, 0x0d, 0x02, 

};

/*
 * Wipe statusbar
 */
void statusbar_wipe(void)
{
    int x;

    for (x = 0; x < LCD_WIDTH; x++)
        lcd_framebuffer[x][STATUSBAR_Y_POS/8]=0x00; 
}

/*
 * Print battery icon to status bar
 */
void statusbar_icon_battery(int percent, bool charging)
{
    int i,j;
    int fill;

    /* draw battery */
    for(i=0;i<17;i++) {
        DRAW_PIXEL((ICON_BATTERY_X_POS+i),STATUSBAR_Y_POS);
        DRAW_PIXEL((ICON_BATTERY_X_POS+i),(STATUSBAR_Y_POS+6));
    }
    for(i=1;i<6;i++) {
        DRAW_PIXEL(ICON_BATTERY_X_POS,(STATUSBAR_Y_POS+i));
        DRAW_PIXEL((ICON_BATTERY_X_POS+16),(STATUSBAR_Y_POS+i));
    }
    for(i=2;i<5;i++)
        DRAW_PIXEL((ICON_BATTERY_X_POS+17),(STATUSBAR_Y_POS+i));

    /* fill battery */
    fill=percent;
    if(fill<0)
        fill=0;
    if(fill>100)
        fill=100;
    fill=fill*15/100;

    for(i=1;i<=fill;i++)
        for(j=1;j<6;j++)
            DRAW_PIXEL((ICON_BATTERY_X_POS+i),(STATUSBAR_Y_POS+j));

    if(charging)
        lcd_bitmap(bitmap_icon_7x8[Icon_Plug], ICON_PLUG_X_POS, STATUSBAR_Y_POS, ICON_PLUG_WIDTH, STATUSBAR_HEIGHT, false);
};

/*
 * Print volume gauge to status bar
 */
void statusbar_icon_volume(int percent)
{
    int i,j;
    int volume;
    int step=0;

    volume=percent;
    if(volume<0)
        volume=0;
    if(volume>100)
        volume=100;

    if(volume==0)
        lcd_bitmap(bitmap_icon_7x8[Icon_Mute], ICON_VOLUME_X_POS+ICON_VOLUME_WIDTH/2-4, STATUSBAR_Y_POS, 7, STATUSBAR_HEIGHT, false);
    else {
        volume=volume*14/100;
        for(i=0;i<volume;i++) {
            if(i%2 == 0)
                step++;
            for(j=1;j<=step;j++)
                DRAW_PIXEL((ICON_VOLUME_X_POS+i),(STATUSBAR_Y_POS+7-j));
        }
    }
}

/*
 * Print play state to status bar
 */
void statusbar_icon_play_state(int state)
{
    lcd_bitmap(bitmap_icon_7x8[state], ICON_PLAY_STATE_X_POS, STATUSBAR_Y_POS, ICON_PLAY_STATE_WIDTH, STATUSBAR_HEIGHT, false);
}

/*
 * Print play mode to status bar
 */
void statusbar_icon_play_mode(int mode)
{
    lcd_bitmap(bitmap_icon_7x8[mode], ICON_PLAY_MODE_X_POS, STATUSBAR_Y_POS, ICON_PLAY_MODE_WIDTH, STATUSBAR_HEIGHT, false);
}

/*
 * Print shuffle mode to status bar
 */
void statusbar_icon_shuffle(void)
{
    lcd_bitmap(bitmap_icon_7x8[Icon_Shuffle], ICON_SHUFFLE_X_POS, STATUSBAR_Y_POS, ICON_SHUFFLE_WIDTH, STATUSBAR_HEIGHT, false);
}

/*
 * Print lock when keys are locked
 */
void statusbar_icon_lock(void)
{
    lcd_bitmap(bitmap_icon_5x8[Icon_Lock], LOCK_X_POS, STATUSBAR_Y_POS, 5, 8, false);
}

#ifdef HAVE_RTC
/*
 * Print time to status bar
 */
void statusbar_time(void)
{
    int hour,minute;
    unsigned char buffer[6];

    hour = rtc_read(0x03);
    minute = rtc_read(0x02);

    snprintf(buffer, sizeof(buffer), "%d%d:%d%d",
             (hour & 0x30) >> 4,
             hour & 0x0f,
             (minute & 0xf0) >> 4,
             minute & 0x0f);
    lcd_putsxy(TIME_X_POS, STATUSBAR_Y_POS, buffer, 0);
}
#endif
#endif
