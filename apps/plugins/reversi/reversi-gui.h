/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (c) 2006 Alexander Levin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#ifndef _REVERSI_GUI_H
#define _REVERSI_GUI_H

#include "plugin.h"

#define GAME_FILE         PLUGIN_GAMES_DIR "/reversi.rev"

/* variable button definitions */
#if CONFIG_KEYPAD == RECORDER_PAD
#define REVERSI_BUTTON_QUIT BUTTON_OFF
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_PLAY
#define REVERSI_BUTTON_MENU BUTTON_F1

#elif CONFIG_KEYPAD == ONDIO_PAD
#define REVERSI_BUTTON_QUIT BUTTON_OFF
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_MENU
#define REVERSI_BUTTON_MAKE_MOVE_SHORTPRESS 
#define REVERSI_BUTTON_MENU_LONGPRESS 
#define REVERSI_BUTTON_MENU BUTTON_MENU 

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#define REVERSI_BUTTON_QUIT BUTTON_OFF
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MENU BUTTON_MODE

#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || \
      (CONFIG_KEYPAD == IPOD_3G_PAD) || \
      (CONFIG_KEYPAD == IPOD_1G2G_PAD)
#define REVERSI_BUTTON_UP BUTTON_MENU
#define REVERSI_BUTTON_DOWN BUTTON_PLAY
#define REVERSI_BUTTON_LEFT (BUTTON_LEFT | BUTTON_SCROLL_BACK)
#define REVERSI_BUTTON_RIGHT (BUTTON_RIGHT | BUTTON_SCROLL_FWD)
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MAKE_MOVE_SHORTPRESS 
#define REVERSI_BUTTON_MENU BUTTON_SELECT
#define REVERSI_BUTTON_MENU_LONGPRESS 

#elif (CONFIG_KEYPAD == IAUDIO_X5M5_PAD)
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MENU BUTTON_PLAY

#elif (CONFIG_KEYPAD == GIGABEAT_PAD)
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MENU BUTTON_MENU

#elif (CONFIG_KEYPAD == GIGABEAT_S_PAD)
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MENU BUTTON_MENU

#elif (CONFIG_KEYPAD == IRIVER_H10_PAD)
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_SCROLL_UP
#define REVERSI_BUTTON_DOWN BUTTON_SCROLL_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_REW
#define REVERSI_BUTTON_MENU BUTTON_PLAY

#elif (CONFIG_KEYPAD == SANSA_E200_PAD) || \
(CONFIG_KEYPAD == SANSA_C200_PAD) || \
(CONFIG_KEYPAD == SANSA_CLIP_PAD) || \
(CONFIG_KEYPAD == SANSA_M200_PAD)
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MAKE_MOVE_SHORTPRESS 
#define REVERSI_BUTTON_MENU BUTTON_SELECT
#define REVERSI_BUTTON_MENU_LONGPRESS 

#elif (CONFIG_KEYPAD == SANSA_FUZE_PAD)
#define REVERSI_BUTTON_QUIT (BUTTON_HOME|BUTTON_REPEAT)
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MAKE_MOVE_SHORTPRESS 
#define REVERSI_BUTTON_MENU BUTTON_SELECT
#define REVERSI_BUTTON_MENU_LONGPRESS 

#elif CONFIG_KEYPAD == MROBE500_PAD
#define REVERSI_BUTTON_QUIT BUTTON_POWER

#elif (CONFIG_KEYPAD == MROBE100_PAD)
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MENU BUTTON_MENU

#elif CONFIG_KEYPAD == IAUDIO_M3_PAD
#define REVERSI_BUTTON_QUIT BUTTON_RC_REC
#define REVERSI_BUTTON_UP BUTTON_RC_VOL_UP
#define REVERSI_BUTTON_DOWN BUTTON_RC_VOL_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_RC_REW
#define REVERSI_BUTTON_RIGHT BUTTON_RC_FF
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_RC_PLAY
#define REVERSI_BUTTON_MENU BUTTON_RC_MENU

#elif CONFIG_KEYPAD == COWON_D2_PAD
#define REVERSI_BUTTON_QUIT         BUTTON_POWER
#define REVERSI_BUTTON_MENU         BUTTON_MENU

#elif CONFIG_KEYPAD == IAUDIO67_PAD
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_STOP
#define REVERSI_BUTTON_DOWN BUTTON_PLAY
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_VOLUP
#define REVERSI_BUTTON_MENU BUTTON_MENU

#elif CONFIG_KEYPAD == CREATIVEZVM_PAD
#define REVERSI_BUTTON_QUIT BUTTON_BACK
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MENU BUTTON_MENU

#elif CONFIG_KEYPAD == PHILIPS_HDD1630_PAD
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_SELECT
#define REVERSI_BUTTON_MENU BUTTON_MENU

#elif CONFIG_KEYPAD == PHILIPS_SA9200_PAD
#define REVERSI_BUTTON_QUIT BUTTON_POWER
#define REVERSI_BUTTON_UP BUTTON_UP
#define REVERSI_BUTTON_DOWN BUTTON_DOWN
#define REVERSI_BUTTON_LEFT BUTTON_PREV
#define REVERSI_BUTTON_RIGHT BUTTON_NEXT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_PLAY
#define REVERSI_BUTTON_MENU BUTTON_MENU

#elif CONFIG_KEYPAD == ONDAVX747_PAD
#define REVERSI_BUTTON_QUIT         BUTTON_POWER
#define REVERSI_BUTTON_MENU         BUTTON_MENU

#elif CONFIG_KEYPAD == ONDAVX777_PAD
#define REVERSI_BUTTON_QUIT         BUTTON_POWER

#elif CONFIG_KEYPAD == SAMSUNG_YH_PAD
#define REVERSI_BUTTON_QUIT      BUTTON_REC
#define REVERSI_BUTTON_UP        BUTTON_UP
#define REVERSI_BUTTON_DOWN      BUTTON_DOWN
#define REVERSI_BUTTON_LEFT      BUTTON_LEFT
#define REVERSI_BUTTON_RIGHT     BUTTON_RIGHT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_FFWD
#define REVERSI_BUTTON_MENU      BUTTON_PLAY

#elif CONFIG_KEYPAD == PBELL_VIBE500_PAD
#define REVERSI_BUTTON_QUIT      BUTTON_REC
#define REVERSI_BUTTON_UP        BUTTON_UP
#define REVERSI_BUTTON_DOWN      BUTTON_DOWN
#define REVERSI_BUTTON_LEFT      BUTTON_PREV
#define REVERSI_BUTTON_RIGHT     BUTTON_NEXT
#define REVERSI_BUTTON_MAKE_MOVE BUTTON_OK
#define REVERSI_BUTTON_MENU      BUTTON_MENU

#else
#error No keymap defined!
#endif

#ifdef HAVE_TOUCHSCREEN
#ifndef REVERSI_BUTTON_QUIT
#define REVERSI_BUTTON_QUIT         BUTTON_TOPLEFT
#endif
#ifndef REVERSI_BUTTON_UP
#define REVERSI_BUTTON_UP           BUTTON_TOPMIDDLE
#endif
#ifndef REVERSI_BUTTON_DOWN
#define REVERSI_BUTTON_DOWN         BUTTON_BOTTOMMIDDLE
#endif
#ifndef REVERSI_BUTTON_LEFT
#define REVERSI_BUTTON_LEFT         BUTTON_MIDLEFT
#endif
#ifndef REVERSI_BUTTON_RIGHT
#define REVERSI_BUTTON_RIGHT        BUTTON_MIDRIGHT
#endif
#ifndef REVERSI_BUTTON_MAKE_MOVE
#define REVERSI_BUTTON_MAKE_MOVE    BUTTON_CENTER
#endif
#ifndef REVERSI_BUTTON_MENU
#define REVERSI_BUTTON_MENU         BUTTON_TOPRIGHT
#endif
#endif

/* Modes for the cursor behaviour at the board edges  */
typedef enum _cursor_wrap_mode_t {
    WRAP_FLAT,   /* No wrapping */
    WRAP_SPHERE, /* (7,7) > right > (7,0); (7,7) > down > (0,7) */
    WRAP_TORUS,  /* (7,7) > right > (0,0); (7,7) > down > (0,0) */
} cursor_wrap_mode_t;


#endif
