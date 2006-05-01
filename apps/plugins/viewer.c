/***************************************************************************
 *
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 *
 *
 * Copyright (C) 2002 Gilles Roux, 2003 Garrett Derner
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "plugin.h"
#include <ctype.h>
#include "playback_control.h"

PLUGIN_HEADER

#define SETTINGS_FILE   "/.rockbox/viewers/viewer.cfg"

#define WRAP_TRIM          44  /* Max number of spaces to trim (arbitrary) */
#define MAX_COLUMNS        64  /* Max displayable string len (over-estimate) */
#define MAX_WIDTH         910  /* Max line length in WIDE mode */
#define READ_PREV_ZONE    910  /* Arbitrary number less than SMALL_BLOCK_SIZE */
#define SMALL_BLOCK_SIZE  0x1000 /* 4k: Smallest file chunk we will read */
#define LARGE_BLOCK_SIZE  0x2000 /* 8k: Preferable size of file chunk to read */
#define BUFFER_SIZE       0x3000 /* 12k: Mem reserved for buffered file data */
#define TOP_SECTOR     buffer
#define MID_SECTOR     (buffer + SMALL_BLOCK_SIZE)
#define BOTTOM_SECTOR  (buffer + 2*(SMALL_BLOCK_SIZE))

/* Out-Of-Bounds test for any pointer to data in the buffer */
#define BUFFER_OOB(p)    ((p) < buffer || (p) >= buffer_end)

/* Does the buffer contain the beginning of the file? */
#define BUFFER_BOF()     (file_pos==0)

/* Does the buffer contain the end of the file? */
#define BUFFER_EOF()     (file_size-file_pos <= BUFFER_SIZE)

/* Formula for the endpoint address outside of buffer data */
#define BUFFER_END() \
 ((BUFFER_EOF()) ? (file_size-file_pos+buffer) : (buffer+BUFFER_SIZE))

/* Is the entire file being shown in one screen? */
#define ONE_SCREEN_FITS_ALL() \
 (next_screen_ptr==NULL && screen_top_ptr==buffer && BUFFER_BOF())

/* Is a scrollbar called for on the current screen? */
#define NEED_SCROLLBAR() \
 ((!(ONE_SCREEN_FITS_ALL())) && (prefs.scrollbar_mode==SB_ON))

/* variable button definitions */

/* Recorder keys */
#if CONFIG_KEYPAD == RECORDER_PAD
#define VIEWER_QUIT BUTTON_OFF
#define VIEWER_PAGE_UP BUTTON_UP
#define VIEWER_PAGE_DOWN BUTTON_DOWN
#define VIEWER_SCREEN_LEFT BUTTON_LEFT
#define VIEWER_SCREEN_RIGHT BUTTON_RIGHT
#define VIEWER_MENU BUTTON_F1
#define VIEWER_AUTOSCROLL BUTTON_PLAY
#define VIEWER_LINE_UP (BUTTON_ON | BUTTON_UP)
#define VIEWER_LINE_DOWN (BUTTON_ON | BUTTON_DOWN)
#define VIEWER_COLUMN_LEFT (BUTTON_ON | BUTTON_LEFT)
#define VIEWER_COLUMN_RIGHT (BUTTON_ON | BUTTON_RIGHT)

/* Ondio keys */
#elif CONFIG_KEYPAD == ONDIO_PAD
#define VIEWER_PAGE_UP BUTTON_UP
#define VIEWER_PAGE_DOWN BUTTON_DOWN
#define VIEWER_SCREEN_LEFT BUTTON_LEFT
#define VIEWER_SCREEN_RIGHT BUTTON_RIGHT
#define VIEWER_MENU BUTTON_OFF
#define VIEWER_AUTOSCROLL BUTTON_MENU

/* Player keys */
#elif CONFIG_KEYPAD == PLAYER_PAD
#define VIEWER_QUIT BUTTON_STOP
#define VIEWER_PAGE_UP BUTTON_LEFT
#define VIEWER_PAGE_DOWN BUTTON_RIGHT
#define VIEWER_MENU BUTTON_MENU
#define VIEWER_AUTOSCROLL BUTTON_ON

/* iRiver H1x0 && H3x0 keys */
#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#define VIEWER_QUIT BUTTON_OFF
#define VIEWER_PAGE_UP BUTTON_UP
#define VIEWER_PAGE_DOWN BUTTON_DOWN
#define VIEWER_SCREEN_LEFT BUTTON_LEFT
#define VIEWER_SCREEN_RIGHT BUTTON_RIGHT
#define VIEWER_MENU BUTTON_MODE
#define VIEWER_AUTOSCROLL BUTTON_SELECT
#define VIEWER_LINE_UP (BUTTON_ON | BUTTON_UP)
#define VIEWER_LINE_DOWN (BUTTON_ON | BUTTON_DOWN)

/* iPods with the 4G pad */
#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || \
      (CONFIG_KEYPAD == IPOD_3G_PAD)
#define VIEWER_QUIT_PRE BUTTON_SELECT
#define VIEWER_QUIT (BUTTON_SELECT | BUTTON_MENU)
#define VIEWER_MENU BUTTON_MENU
#define VIEWER_AUTOSCROLL BUTTON_PLAY
#define VIEWER_PAGE_UP BUTTON_SCROLL_BACK
#define VIEWER_PAGE_DOWN BUTTON_SCROLL_FWD
#define VIEWER_SCREEN_LEFT BUTTON_LEFT
#define VIEWER_SCREEN_RIGHT BUTTON_RIGHT

/* iFP7xx keys */
#elif CONFIG_KEYPAD == IRIVER_IFP7XX_PAD
#define VIEWER_QUIT BUTTON_PLAY
#define VIEWER_PAGE_UP BUTTON_UP
#define VIEWER_PAGE_DOWN BUTTON_DOWN
#define VIEWER_SCREEN_LEFT BUTTON_LEFT
#define VIEWER_SCREEN_RIGHT BUTTON_RIGHT
#define VIEWER_MENU BUTTON_MODE
#define VIEWER_AUTOSCROLL BUTTON_SELECT

/* iAudio X5 keys */
#elif CONFIG_KEYPAD == IAUDIO_X5_PAD
#define VIEWER_QUIT BUTTON_POWER
#define VIEWER_PAGE_UP BUTTON_UP
#define VIEWER_PAGE_DOWN BUTTON_DOWN
#define VIEWER_SCREEN_LEFT BUTTON_LEFT
#define VIEWER_SCREEN_RIGHT BUTTON_RIGHT
#define VIEWER_MENU BUTTON_SELECT
#define VIEWER_AUTOSCROLL BUTTON_PLAY

/* GIGABEAT keys */
#elif CONFIG_KEYPAD == GIGABEAT_PAD
#define VIEWER_QUIT BUTTON_POWER
#define VIEWER_PAGE_UP BUTTON_UP
#define VIEWER_PAGE_DOWN BUTTON_DOWN
#define VIEWER_SCREEN_LEFT BUTTON_LEFT
#define VIEWER_SCREEN_RIGHT BUTTON_RIGHT
#define VIEWER_MENU BUTTON_MENU
#define VIEWER_AUTOSCROLL BUTTON_A

#endif

struct preferences {
    enum {
        WRAP=0,
        CHOP,
        WORD_MODES
    } word_mode;

    enum {
        NORMAL=0,
        JOIN,
#ifdef HAVE_LCD_BITMAP
        REFLOW, /* Makes no sense for the player */
#endif
        EXPAND,
        LINE_MODES,
#ifndef HAVE_LCD_BITMAP
        REFLOW  /* Sorting it behind LINE_MODES effectively disables it. */
#endif
    } line_mode;

    enum {
        NARROW=0,
        WIDE,
        VIEW_MODES
    } view_mode;

#ifdef HAVE_LCD_BITMAP
    enum {
        SB_OFF=0,
        SB_ON,
        SCROLLBAR_MODES
    } scrollbar_mode;
    bool need_scrollbar;

    enum {
        NO_OVERLAP=0,
        OVERLAP,
        PAGE_MODES
    } page_mode;

#endif /* HAVE_LCD_BITMAP */

    enum {
        PAGE=0,
        LINE,
        SCROLL_MODES
    } scroll_mode;

    int autoscroll_speed;
} prefs;

static unsigned char buffer[BUFFER_SIZE + 1];
static unsigned char line_break[] = {0,0x20,'-',9,0xB,0xC};
static int display_columns; /* number of (pixel) columns on the display */
static int display_lines; /* number of lines on the display */
static int draw_columns; /* number of (pixel) columns available for text */
static int par_indent_spaces; /* number of spaces to indent first paragraph */
static int fd;
static char *file_name;
static long file_size;
static bool mac_text;
static long file_pos; /* Position of the top of the buffer in the file */
static unsigned char *buffer_end; /*Set to BUFFER_END() when file_pos changes*/
static int max_line_len;
static unsigned char *screen_top_ptr;
static unsigned char *next_screen_ptr;
static unsigned char *next_screen_to_draw_ptr;
static unsigned char *next_line_ptr;
static struct plugin_api* rb;

static unsigned char glyph_width[256];

bool done = false;
int col = 0;

#define ADVANCE_COUNTERS(c) do { width += glyph_width[c]; k++; } while(0)
#define LINE_IS_FULL ((k>MAX_COLUMNS-1) || (width > draw_columns))
static unsigned char* crop_at_width(const unsigned char* p)
{
    int k,width;

    k=width=0;
    while (!LINE_IS_FULL)
        ADVANCE_COUNTERS(p[k]);

    return (unsigned char*) p+k-1;
}

static unsigned char* find_first_feed(const unsigned char* p, int size)
{
    int i;

    for (i=0; i < size; i++)
        if (p[i] == 0)
            return (unsigned char*) p+i;

    return NULL;
}

static unsigned char* find_last_feed(const unsigned char* p, int size)
{
    int i;

    for (i=size-1; i>=0; i--)
        if (p[i] == 0)
            return (unsigned char*) p+i;

    return NULL;
}

static unsigned char* find_last_space(const unsigned char* p, int size)
{
    int i, j, k;

    k = (prefs.line_mode==JOIN) || (prefs.line_mode==REFLOW) ? 0:1;

    for (i=size-1; i>=0; i--)
        for (j=k; j < (int) sizeof(line_break); j++)
            if (p[i] == line_break[j])
                return (unsigned char*) p+i;

    return NULL;
}

static unsigned char* find_next_line(const unsigned char* cur_line, bool *is_short)
{
    const unsigned char *next_line = NULL;
    int size, i, j, k, width, search_len, spaces, newlines;
    bool first_chars;
    unsigned char c;

    if (is_short != NULL)
        *is_short = true;

    if BUFFER_OOB(cur_line)
        return NULL;

    if (prefs.view_mode == WIDE) {
        search_len = MAX_WIDTH;
    }
    else {   /* prefs.view_mode == NARROW */
        search_len = crop_at_width(cur_line) - cur_line;
    }

    size = BUFFER_OOB(cur_line+search_len) ? buffer_end-cur_line : search_len;

    if ((prefs.line_mode == JOIN) || (prefs.line_mode == REFLOW)) {
        /* Need to scan ahead and possibly increase search_len and size,
         or possibly set next_line at second hard return in a row. */
        next_line = NULL;
        first_chars=true;
        for (j=k=width=spaces=newlines=0; ; j++) {
            if (BUFFER_OOB(cur_line+j))
                return NULL;
            if (LINE_IS_FULL) {
                size = search_len = j;
                break;
            }

            c = cur_line[j];
            switch (c) {
                case ' ':
                    if (prefs.line_mode == REFLOW) {
                        if (newlines > 0) {
                            size = j;
                            next_line = cur_line + size;
                            return (unsigned char*) next_line;
                        }
                        if (j==0) /* i=1 is intentional */
                            for (i=0; i<par_indent_spaces; i++)
                                ADVANCE_COUNTERS(' ');
                    }
                    if (!first_chars) spaces++;
                    break;

                case 0:
                    if (newlines > 0) {
                        size = j;
                        next_line = cur_line + size - spaces;
                        if (next_line != cur_line)
                            return (unsigned char*) next_line;
                        break;
                    }

                    newlines++;
                    size += spaces -1;
                    if (BUFFER_OOB(cur_line+size) || size > 2*search_len)
                        return NULL;
                    search_len = size;
                    spaces = first_chars? 0:1;
                    break;

                default:
                    if (prefs.line_mode==JOIN || newlines>0) {
                        while (spaces) {
                            spaces--;
                            ADVANCE_COUNTERS(' ');
                            if (LINE_IS_FULL) {
                                size = search_len = j;
                                break;
                            }
                        }
                        newlines=0;
                   } else if (spaces) {
                        /* REFLOW, multiple spaces between words: count only
                         * one. If more are needed, they will be added
                         * while drawing. */
                        search_len = size;
                        spaces=0;
                        ADVANCE_COUNTERS(' ');
                        if (LINE_IS_FULL) {
                            size = search_len = j;
                            break;
                        }
                    }
                    first_chars = false;
                    ADVANCE_COUNTERS(c);
                    break;
            }
        }
    }
    else {
        /* find first hard return */
        next_line = find_first_feed(cur_line, size);
    }

    if (next_line == NULL)
        if (size == search_len) {
            if (prefs.word_mode == WRAP)  /* Find last space */
                next_line = find_last_space(cur_line, size);

            if (next_line == NULL)
                next_line = crop_at_width(cur_line);
            else
                if (prefs.word_mode == WRAP)
                    for (i=0;
                    i<WRAP_TRIM && isspace(next_line[0]) && !BUFFER_OOB(next_line);
                    i++)
                        next_line++;
        }

    if (prefs.line_mode == EXPAND)
        if (!BUFFER_OOB(next_line))  /* Not Null & not out of bounds */
            if (next_line[0] == 0)
                if (next_line != cur_line)
                    return (unsigned char*) next_line;

    /* If next_line is pointing to a zero, increment it; i.e.,
     leave the terminator at the end of cur_line. If pointing
     to a hyphen, increment only if there is room to display
     the hyphen on current line (won't apply in WIDE mode,
     since it's guarenteed there won't be room). */
    if (!BUFFER_OOB(next_line))  /* Not Null & not out of bounds */
        if (next_line[0] == 0 ||
        (next_line[0] == '-' && next_line-cur_line < draw_columns))
            next_line++;

    if (BUFFER_OOB(next_line))
        return NULL;

    if (is_short)
        *is_short = false;

    return (unsigned char*) next_line;
}

static unsigned char* find_prev_line(const unsigned char* cur_line)
{
    const unsigned char *prev_line = NULL;
    const unsigned char *p;

    if BUFFER_OOB(cur_line)
        return NULL;

    /* To wrap consistently at the same places, we must
     start with a known hard return, then work downwards.
     We can either search backwards for a hard return,
     or simply start wrapping downwards from top of buffer.
       If current line is not near top of buffer, this is
     a file with long lines (paragraphs). We would need to
     read earlier sectors before we could decide how to
     properly wrap the lines above the current line, but
     it probably is not worth the disk access. Instead,
     start with top of buffer and wrap down from there.
     This may result in some lines wrapping at different
     points from where they wrap when scrolling down.
       If buffer is at top of file, start at top of buffer. */

    if ((prefs.line_mode == JOIN) || (prefs.line_mode == REFLOW))
        prev_line = p = NULL;
    else
        prev_line = p = find_last_feed(buffer, cur_line-buffer-1);
        /* Null means no line feeds in buffer above current line. */

    if (prev_line == NULL)
        if (BUFFER_BOF() || cur_line - buffer > READ_PREV_ZONE)
            prev_line = p = buffer;
        /* (else return NULL and read previous block) */

    /* Wrap downwards until too far, then use the one before. */
    while (p < cur_line && p != NULL) {
        prev_line = p;
        p = find_next_line(prev_line, NULL);
    }

    if (BUFFER_OOB(prev_line))
        return NULL;

    return (unsigned char*) prev_line;
}

static void fill_buffer(long pos, unsigned char* buf, unsigned size)
{
    /* Read from file and preprocess the data */
    /* To minimize disk access, always read on sector boundaries */
    unsigned numread, i;
    bool found_CR = false;

    rb->lseek(fd, pos, SEEK_SET);
    numread = rb->read(fd, buf, size);
    rb->button_clear_queue(); /* clear button queue */

    for(i = 0; i < numread; i++) {
        switch(buf[i]) {
            case '\r':
                if (mac_text) {
                    buf[i] = 0;
                }
                else {
                    buf[i] = ' ';
                    found_CR = true;
                }
                break;

            case '\n':
                buf[i] = 0;
                found_CR = false;
                break;

            case 0:  /* No break between case 0 and default, intentionally */
                buf[i] = ' ';
            default:
                if (found_CR) {
                    buf[i - 1] = 0;
                    found_CR = false;
                    mac_text = true;
                }
                break;
        }
    }
}

static int read_and_synch(int direction)
{
/* Read next (or prev) block, and reposition global pointers. */
/* direction: 1 for down (i.e., further into file), -1 for up */
    int move_size, move_vector, offset;
    unsigned char *fill_buf;

    if (direction == -1) /* up */ {
        move_size = SMALL_BLOCK_SIZE;
        offset = 0;
        fill_buf = TOP_SECTOR;
        rb->memcpy(BOTTOM_SECTOR, MID_SECTOR, SMALL_BLOCK_SIZE);
        rb->memcpy(MID_SECTOR, TOP_SECTOR, SMALL_BLOCK_SIZE);
    }
    else /* down */ {
        if (prefs.view_mode == WIDE) {
            /* WIDE mode needs more buffer so we have to read smaller blocks */
            move_size = SMALL_BLOCK_SIZE;
            offset = LARGE_BLOCK_SIZE;
            fill_buf = BOTTOM_SECTOR;
            rb->memcpy(TOP_SECTOR, MID_SECTOR, SMALL_BLOCK_SIZE);
            rb->memcpy(MID_SECTOR, BOTTOM_SECTOR, SMALL_BLOCK_SIZE);
        }
        else {
            move_size = LARGE_BLOCK_SIZE;
            offset = SMALL_BLOCK_SIZE;
            fill_buf = MID_SECTOR;
            rb->memcpy(TOP_SECTOR, BOTTOM_SECTOR, SMALL_BLOCK_SIZE);
        }
    }
    move_vector = direction * move_size;
    screen_top_ptr -= move_vector;
    file_pos += move_vector;
    buffer_end = BUFFER_END();  /* Update whenever file_pos changes */
    fill_buffer(file_pos + offset, fill_buf, move_size);
    return move_vector;
}

static void viewer_scroll_up(void)
{
    unsigned char *p;

    p = find_prev_line(screen_top_ptr);
    if (p == NULL && !BUFFER_BOF()) {
        read_and_synch(-1);
        p = find_prev_line(screen_top_ptr);
    }
    if (p != NULL)
        screen_top_ptr = p;
}

static void viewer_scroll_down(void)
{
    if (next_screen_ptr != NULL)
        screen_top_ptr = next_line_ptr;
}

#ifdef HAVE_LCD_BITMAP
static void viewer_scrollbar(void) {
    int w, h, items, min_shown, max_shown;

    rb->lcd_getstringsize("o", &w, &h);
    items = (int) file_size;  /* (SH1 int is same as long) */
    min_shown = (int) file_pos + (screen_top_ptr - buffer);

    if (next_screen_ptr == NULL)
        max_shown = items;
    else
        max_shown = min_shown + (next_screen_ptr - screen_top_ptr);

    rb->scrollbar(0, 0, w-2, LCD_HEIGHT, items, min_shown, max_shown, VERTICAL);
}
#endif

static void viewer_draw(int col)
{
    int i, j, k, line_len, resynch_move, spaces, left_col=0;
    int width, extra_spaces, indent_spaces, spaces_per_word;
    bool multiple_spacing, line_is_short;
    unsigned char *line_begin;
    unsigned char *line_end;
    unsigned char c;
    unsigned char scratch_buffer[MAX_COLUMNS + 1];
    unsigned char utf8_buffer[MAX_COLUMNS*4 + 1];
    int len;
    unsigned char *endptr;

    /* If col==-1 do all calculations but don't display */
    if (col != -1) {
#ifdef HAVE_LCD_BITMAP
        left_col = prefs.need_scrollbar? 1:0;
#else
        left_col = 0;
#endif
        rb->lcd_clear_display();
    }
    max_line_len = 0;
    line_begin = line_end = screen_top_ptr;

    for (i = 0; i < display_lines; i++) {
        if (BUFFER_OOB(line_end))
            break;  /* Happens after display last line at BUFFER_EOF() */

        line_begin = line_end;
        line_end = find_next_line(line_begin, &line_is_short);

        if (line_end == NULL) {
            if (BUFFER_EOF()) {
                if (i < display_lines - 1 && !BUFFER_BOF()) {
                    if (col != -1)
                        rb->lcd_clear_display();

                    for (; i < display_lines - 1; i++)
                        viewer_scroll_up();

                    line_begin = line_end = screen_top_ptr;
                    i = -1;
                    continue;
                }
                else {
                    line_end = buffer_end;
                }
            }
            else {
                resynch_move = read_and_synch(1); /* Read block & move ptrs */
                line_begin -= resynch_move;
                if (i > 0)
                    next_line_ptr -= resynch_move;

                line_end = find_next_line(line_begin, NULL);
                if (line_end == NULL)  /* Should not really happen */
                    break;
            }
        }
        line_len = line_end - line_begin;

        if (prefs.line_mode == JOIN) {
            if (line_begin[0] == 0) {
                line_begin++;
                if (prefs.word_mode == CHOP)
                    line_end++;
                else
                    line_len--;
            }
            for (j=k=spaces=0; j < line_len; j++) {
                if (k == MAX_COLUMNS)
                    break;

                c = line_begin[j];
                switch (c) {
                    case ' ':
                        spaces++;
                        break;
                    case 0:
                        spaces = 0;
                        scratch_buffer[k++] = ' ';
                        break;
                    default:
                        while (spaces) {
                            spaces--;
                            scratch_buffer[k++] = ' ';
                            if (k == MAX_COLUMNS - 1)
                                break;
                        }
                        scratch_buffer[k++] = c;
                        break;
                }
            }

            if (col != -1)
                if (k > col) {
                    scratch_buffer[k] = 0;
                    endptr = rb->iso_decode(scratch_buffer + col, utf8_buffer,
                                            -1, k-col);
                    *endptr = 0;
                    len = rb->utf8length(utf8_buffer);
                    rb->lcd_puts(left_col, i, utf8_buffer);
                }
        }
        else if (prefs.line_mode == REFLOW) {
            if (line_begin[0] == 0) {
                line_begin++;
                if (prefs.word_mode == CHOP)
                    line_end++;
                else
                    line_len--;
            }

            indent_spaces = 0;
            if (!line_is_short) {
                multiple_spacing = false;
                for (j=width=spaces=0; j < line_len; j++) {
                    c = line_begin[j];
                    switch (c) {
                        case ' ':
                        case 0:
                            if ((j==0) && (prefs.word_mode==WRAP))
                                /* special case: indent the paragraph,
                                 * don't count spaces */
                                indent_spaces = par_indent_spaces;
                            else if (!multiple_spacing)
                                spaces++;
                            multiple_spacing = true;
                            break;
                        default:
                            multiple_spacing = false;
                            width += glyph_width[c];
                            k++;
                            break;
                    }
                }
                if (multiple_spacing) spaces--;

                if (spaces) {
                    /* total number of spaces to insert between words */
                    extra_spaces = (draw_columns-width) / glyph_width[' ']
                            - indent_spaces;
                    /* number of spaces between each word*/
                    spaces_per_word = extra_spaces / spaces;
                    /* number of words with n+1 spaces (to fill up) */
                    extra_spaces = extra_spaces % spaces;
                    if (spaces_per_word > 2) { /* too much spacing is awful */
                        spaces_per_word = 3;
                        extra_spaces = 0;
                    }
                } else { /* this doesn't matter much... no spaces anyway */
                    spaces_per_word = extra_spaces = 0;
                }
            } else { /* end of a paragraph: don't fill line */
                spaces_per_word = 1;
                extra_spaces = 0;
            }

            multiple_spacing = false;
            for (j=k=spaces=0; j < line_len; j++) {
                if (k == MAX_COLUMNS)
                    break;

                c = line_begin[j];
                switch (c) {
                    case ' ':
                    case 0:
                        if (j==0 && prefs.word_mode==WRAP) { /* indent paragraph */
                            for (j=0; j<par_indent_spaces; j++)
                                scratch_buffer[k++] = ' ';
                            j=0;
                        }
                        else if (!multiple_spacing) {
                            for (width = spaces<extra_spaces ? -1:0; width < spaces_per_word; width++)
                                    scratch_buffer[k++] = ' ';
                            spaces++;
                        }
                        multiple_spacing = true;
                        break;
                    default:
                        scratch_buffer[k++] = c;
                        multiple_spacing = false;
                        break;
                }
            }

            if (col != -1)
                if (k > col) {
                    scratch_buffer[k] = 0;
                    endptr = rb->iso_decode(scratch_buffer + col, utf8_buffer,
                                            -1, k-col);
                    *endptr = 0;
                    len = rb->utf8length(utf8_buffer);
                    rb->lcd_puts(left_col, i, utf8_buffer);
                }
        }
        else { /* prefs.line_mode != JOIN && prefs.line_mode != REFLOW */
            if (col != -1)
                if (line_len > col) {
                    c = line_end[0];
                    line_end[0] = 0;
                    endptr = rb->iso_decode(line_begin + col, utf8_buffer,
                                            -1, line_end-line_begin);
                    *endptr = 0;
                    len = rb->utf8length(utf8_buffer);
                    rb->lcd_puts(left_col, i, utf8_buffer);
                    line_end[0] = c;
                }
        }
        if (line_len > max_line_len)
            max_line_len = line_len;

        if (i == 0)
            next_line_ptr = line_end;
    }
    next_screen_ptr = line_end;
    if (BUFFER_OOB(next_screen_ptr))
        next_screen_ptr = NULL;

#ifdef HAVE_LCD_BITMAP
    next_screen_to_draw_ptr = prefs.page_mode==OVERLAP? line_begin: next_screen_ptr;

    if (prefs.need_scrollbar)
        viewer_scrollbar();

    if (col != -1)
        rb->lcd_update();
#else
    next_screen_to_draw_ptr = next_screen_ptr;
#endif
}

static void viewer_top(void)
{
    /* Read top of file into buffer
      and point screen pointer to top */
    file_pos = 0;
    buffer_end = BUFFER_END();  /* Update whenever file_pos changes */
    screen_top_ptr = buffer;
    fill_buffer(0, buffer, BUFFER_SIZE);
}

#ifdef HAVE_LCD_BITMAP
static void viewer_bottom(void)
{
    /* Read bottom of file into buffer
      and point screen pointer to bottom */
    long last_sectors;

    if (file_size > BUFFER_SIZE) {
        /* Find last buffer in file, round up to next sector boundary */
        last_sectors = file_size - BUFFER_SIZE + SMALL_BLOCK_SIZE;
        last_sectors /= SMALL_BLOCK_SIZE;
        last_sectors *= SMALL_BLOCK_SIZE;
    }
    else {
        last_sectors = 0;
    }
    file_pos = last_sectors;
    buffer_end = BUFFER_END();  /* Update whenever file_pos changes */
    screen_top_ptr = buffer_end-1;
    fill_buffer(last_sectors, buffer, BUFFER_SIZE);
}

static void init_need_scrollbar(void) {
    /* Call viewer_draw in quiet mode to initialize next_screen_ptr,
     and thus ONE_SCREEN_FITS_ALL(), and thus NEED_SCROLLBAR() */
    viewer_draw(-1);
    prefs.need_scrollbar = NEED_SCROLLBAR();
    draw_columns = prefs.need_scrollbar? display_columns-glyph_width['o'] : display_columns;
    par_indent_spaces = draw_columns/(5*glyph_width[' ']);
}
#else
#define init_need_scrollbar()
#endif

static bool viewer_init(void)
{
#ifdef HAVE_LCD_BITMAP
    int idx, ch;
    struct font *pf;

    pf = rb->font_get(FONT_UI);

    if (pf->width != NULL)
    {   /* variable pitch font -- fill structure from font width data */
        ch = pf->defaultchar - pf->firstchar;
        rb->memset(glyph_width, pf->width[ch], 256);
        idx = pf->firstchar;
        rb->memcpy(&glyph_width[idx], pf->width, pf->size);
        idx += pf->size;
        rb->memset(&glyph_width[idx], pf->width[ch], 256-idx);
    }
    else /* fixed pitch font -- same width for all glyphs */
        rb->memset(glyph_width, pf->maxwidth, 256);

    display_lines = LCD_HEIGHT / pf->height;
    display_columns = LCD_WIDTH;
#else
    /* REAL fixed pitch :) all chars use up 1 cell */
    display_lines = 2;
    draw_columns = display_columns = 11;
    par_indent_spaces = 2;
    rb->memset(glyph_width, 1, 256);
#endif

    fd = rb->open(file_name, O_RDONLY);
    if (fd==-1)
        return false;

    file_size = rb->filesize(fd);
    if (file_size==-1)
        return false;

    /* Init mac_text value used in processing buffer */
    mac_text = false;

    /* Read top of file into buffer;
      init file_pos, buffer_end, screen_top_ptr */
    viewer_top();

    /* Init prefs.need_scrollbar value */
    init_need_scrollbar();

    return true;
}

static void viewer_reset_settings(void)
{
    prefs.word_mode = WRAP;
    prefs.line_mode = NORMAL;
    prefs.view_mode = NARROW;
    prefs.scroll_mode = PAGE;
#ifdef HAVE_LCD_BITMAP
    prefs.page_mode = NO_OVERLAP;
    prefs.scrollbar_mode = SB_OFF;
#endif
}

static void viewer_load_settings(void) /* same name as global, but not the same file.. */
{
    int settings_fd;

    settings_fd=rb->open(SETTINGS_FILE, O_RDONLY);
    if (settings_fd < 0)
    {
        rb->splash(HZ*2, true, "No Settings File");
        return;
    }
    if (rb->filesize(settings_fd) != sizeof(struct preferences))
    {
        rb->splash(HZ*2, true, "Settings File Invalid");
        return;
    }

    rb->read(settings_fd, &prefs, sizeof(struct preferences));
    rb->close(settings_fd);

    init_need_scrollbar();

    file_pos=0;
    buffer_end = BUFFER_END();  /* Update whenever file_pos changes */

    screen_top_ptr = buffer;
    if (BUFFER_OOB(screen_top_ptr)) {
        screen_top_ptr = buffer;
    }

    fill_buffer(file_pos, buffer, BUFFER_SIZE);
}

static void viewer_save_settings(void)/* same name as global, but not the same file.. */
{
    int settings_fd;

    settings_fd = rb->creat(SETTINGS_FILE, O_WRONLY); /* create the settings file */

    rb->write (settings_fd, &prefs, sizeof(struct preferences));
    rb->close(settings_fd);
}

static void viewer_exit(void *parameter)
{
    (void)parameter;

    viewer_save_settings();
    rb->close(fd);
}

#ifdef HAVE_LCD_BITMAP
static int col_limit(int col)
{
    if (col < 0)
        col = 0;
    else
        if (col > max_line_len - 2)
            col = max_line_len - 2;

    return col;
}
#endif

static void change_options_menu(void)
{
    int m, result;
    bool done = false;

    static const struct menu_item items[] = {
        {"Word Wrap", NULL },
        {"Line Mode", NULL },
        {"Wide View", NULL },
        {"Overlap Pages", NULL },
        {"Scroll Mode", NULL},
#ifdef HAVE_LCD_BITMAP
        {"Show Scrollbar", NULL },
#endif
        {"Auto-Scroll Speed", NULL },
    };
    static const struct opt_items opt_word_mode[2] = {
        {"On",NULL},{"Off (Chop Words)",NULL},
    };
#ifdef HAVE_LCD_BITMAP
    static const struct opt_items opt_line_mode[4] = {
        {"Normal",NULL},{"Join Lines",NULL},
        {"Reflow Lines",NULL},{"Expand Lines",NULL},
#else
    static const struct opt_items opt_line_mode[3] = {
        {"Normal",NULL},{"Join Lines",NULL},
        {"Expand Lines",NULL},
#endif
    };
    static const struct opt_items opt_view_mode[2] = {
        {"No (Narrow)",NULL},{"Yes",NULL}
    };
    static const struct opt_items opt_scroll_mode[2] = {
        {"Scroll by Page",NULL},{"Scroll by Line",NULL}
    };
#ifdef HAVE_LCD_BITMAP
    static const struct opt_items opt_scrollbar_mode[2] = {
        {"Off",NULL},{"On",NULL}
    };
    static const struct opt_items opt_page_mode[2] = {
        {"No",NULL},{"Yes",NULL}
    };
#endif
    static const struct opt_items opt_autoscroll_speed[10] = {
        { "1", NULL },{ "2", NULL },{ "3", NULL },{ "4", NULL },{ "5", NULL },
        { "6", NULL },{ "7", NULL },{ "8", NULL },{ "9", NULL },{ "10", NULL }
    };
    m = rb->menu_init(items, sizeof(items) / sizeof(*items),
                      NULL, NULL, NULL, NULL);

    while(!done)
    {
        result=rb->menu_show(m);
        switch (result)
        {
            case MENU_SELECTED_EXIT:
                done = true;
                break;

            case 0: /* word mode */
                rb->set_option("Word Wrap", &prefs.word_mode, INT,
                            opt_word_mode , 2, NULL);
                break;
            case 1: /* line mode */
                rb->set_option("Line Mode", &prefs.line_mode, INT, opt_line_mode,
                            sizeof(opt_line_mode) / sizeof(*opt_line_mode), NULL);
                break;
            case 2: /* view mode */
                rb->set_option("Wide View", &prefs.view_mode, INT,
                            opt_view_mode , 2, NULL);
                break;
#ifdef HAVE_LCD_BITMAP
            case 3:
                rb->set_option("Overlap Pages", &prefs.page_mode, INT,
                            opt_page_mode , 2, NULL);
                break;
#endif
            case 4:
                rb->set_option("Scroll Mode", &prefs.scroll_mode, INT,
                               opt_scroll_mode , 2, NULL);
                break;
    #ifdef HAVE_LCD_BITMAP
            case 5:
                rb->set_option("Show Scrollbar", &prefs.scrollbar_mode, INT,
                            opt_scrollbar_mode , 2, NULL);
                /* Show-scrollbar mode for current view-width mode */
                if (!(ONE_SCREEN_FITS_ALL())) {
                    if (prefs.scrollbar_mode == true)
                        init_need_scrollbar();
                }
                break;
    #endif
            case 6:
                rb->set_option("Auto-Scroll Speed", &prefs.autoscroll_speed, INT,
                            opt_autoscroll_speed, sizeof(opt_autoscroll_speed) /
                            sizeof(*opt_autoscroll_speed), NULL);
                break;
        } /* switch() */
    }
    rb->menu_exit(m);
#ifdef HAVE_LCD_BITMAP
    rb->lcd_setmargins(0,0);
#endif
}

static void show_menu(void)
{
    int m;
    int result;
    static const struct menu_item items[] = {
        {"Quit", NULL },
        {"Viewer Options", NULL },
        {"Show Playback Menu", NULL },
        {"Return", NULL },
    };

    m = rb->menu_init(items, sizeof(items) / sizeof(*items), NULL, NULL, NULL, NULL);
    result=rb->menu_show(m);
    switch (result)
    {
        case 0: /* quit */
            rb->splash(1, true, "Saving Settings");
            rb->menu_exit(m);
            viewer_exit(NULL);
            done = true;
            break;
        case 1: /* change settings */
            change_options_menu();
            break;
        case 2: /* playback control */
            playback_control(rb);
            break;
        case 3: /* return */
            break;
    }
    rb->menu_exit(m);
#ifdef HAVE_LCD_BITMAP
    rb->lcd_setmargins(0,0);
#endif
    viewer_draw(col);
}

enum plugin_status plugin_start(struct plugin_api* api, void* file)
{
    int button, i, ok;
    bool autoscroll = false;
    int old_tick = *rb->current_tick;

    rb = api;

    if (!file)
        return PLUGIN_ERROR;

    file_name = file;
    ok = viewer_init();
    if (!ok) {
        rb->splash(HZ, false, "Error");
        viewer_exit(NULL);
        return PLUGIN_OK;
    }

    viewer_reset_settings(); /* load defaults first */
    viewer_load_settings(); /* .. then try to load from disk */

    viewer_draw(col);

    while (!done) {

        if(autoscroll)
        {
            if(old_tick <= *rb->current_tick - (110-prefs.autoscroll_speed*10))
            {
                viewer_scroll_down();
                viewer_draw(col);
                old_tick = *rb->current_tick;
            }
        }

        button = rb->button_get_w_tmo(HZ/10);
        switch (button) {
            case VIEWER_MENU:
                show_menu();
                break;

            case VIEWER_AUTOSCROLL:
                autoscroll = !autoscroll;
                break;

            case VIEWER_PAGE_UP:
            case VIEWER_PAGE_UP | BUTTON_REPEAT:
                if (prefs.scroll_mode == PAGE)
                {
                    /* Page up */
#ifdef HAVE_LCD_BITMAP
                    for (i = prefs.page_mode==OVERLAP? 1:0; i < display_lines; i++)
#else
                    for (i = 0; i < display_lines; i++)
#endif
                        viewer_scroll_up();
                }
                else
                    viewer_scroll_up();
                old_tick = *rb->current_tick;
                viewer_draw(col);
                break;

            case VIEWER_PAGE_DOWN:
            case VIEWER_PAGE_DOWN | BUTTON_REPEAT:
                if (prefs.scroll_mode == PAGE)
                {
                    /* Page down */
                    if (next_screen_ptr != NULL)
                        screen_top_ptr = next_screen_to_draw_ptr;
                }
                else
                    viewer_scroll_down();
                old_tick = *rb->current_tick;
                viewer_draw(col);
                break;

#ifdef VIEWER_SCREEN_LEFT
            case VIEWER_SCREEN_LEFT:
            case VIEWER_SCREEN_LEFT | BUTTON_REPEAT:
                if (prefs.view_mode == WIDE) {
                    /* Screen left */
                    col -= draw_columns/glyph_width['o'];
                    col = col_limit(col);
                }
                else {   /* prefs.view_mode == NARROW */
                    /* Top of file */
                    viewer_top();
                }

                viewer_draw(col);
                break;
#endif

#ifdef VIEWER_SCREEN_LEFT
            case VIEWER_SCREEN_RIGHT:
            case VIEWER_SCREEN_RIGHT | BUTTON_REPEAT:
                if (prefs.view_mode == WIDE) {
                    /* Screen right */
                    col += draw_columns/glyph_width['o'];
                    col = col_limit(col);
                }
                else {   /* prefs.view_mode == NARROW */
                    /* Bottom of file */
                    viewer_bottom();
                }

                viewer_draw(col);
                break;
#endif

#ifdef VIEWER_LINE_UP
            case VIEWER_LINE_UP:
            case VIEWER_LINE_UP | BUTTON_REPEAT:
                /* Scroll up one line */
                viewer_scroll_up();
                old_tick = *rb->current_tick;
                viewer_draw(col);
                break;

            case VIEWER_LINE_DOWN:
            case VIEWER_LINE_DOWN | BUTTON_REPEAT:
                /* Scroll down one line */
                if (next_screen_ptr != NULL)
                    screen_top_ptr = next_line_ptr;
                old_tick = *rb->current_tick;
                viewer_draw(col);
                break;
#endif
#ifdef VIEWER_COLUMN_LEFT
            case VIEWER_COLUMN_LEFT:
            case VIEWER_COLUMN_LEFT | BUTTON_REPEAT:
                /* Scroll left one column */
                col--;
                col = col_limit(col);
                viewer_draw(col);
                break;

            case VIEWER_COLUMN_RIGHT:
            case VIEWER_COLUMN_RIGHT | BUTTON_REPEAT:
                /* Scroll right one column */
                col++;
                col = col_limit(col);
                viewer_draw(col);
                break;
#endif

#ifdef VIEWER_QUIT
            case VIEWER_QUIT:
                viewer_exit(NULL);
                done = true;
                break;
#endif

            default:
                if (rb->default_event_handler_ex(button, viewer_exit, NULL)
                    == SYS_USB_CONNECTED)
                    return PLUGIN_USB_CONNECTED;
                break;
        }
    }
    return PLUGIN_OK;
}

