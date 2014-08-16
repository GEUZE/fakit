/*SOC
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S05124BS1                                                    **
** +Revision: 1.3 +                                                 **
** +Date: 2006/11/16 14:34:53 +                                     **
** Header for simple gfx                                            **
**********************************************************************
EOC*/

#ifndef _GFX_H_
#define _GFX_H_

typedef struct gfx_rawFont_s {
  __u8 lineOffset;
  __u8 h;
  __u8 first;
  __u8 last;
  __u8 data[];
} gfx_rawFont_t;

typedef struct gfx_font_s {
  __u8 first,last;
  __u8 h,w;
  __u32 lineOffset;
  __u8 *bitLen;
  __u8 *byteOffset;
  __u8 *bitOffset;
  __u8 *data;
} gfx_font_t;

typedef struct gfxraw_screenInfo_s {
  int width;
  int height;
  int colors;
  char *screen;
} gfxraw_screenInfo_t;

typedef struct gfx_screenInfo_s {
  int width;
  int height;
  int colors;
  int trans_color;
  int bg,fg;
  int inv;
  gfx_rawFont_t *rawFont;
  gfx_font_t font;
  int char_x,char_y;
  const gfxraw_screenInfo_t *rawscreen;
} gfx_screenInfo_t;

const gfx_screenInfo_t * gfx_Init(void);
void gfx_clrScr(const gfx_screenInfo_t *scr);
void gfx_paletteSet(const gfx_screenInfo_t *scr,
		    __u8 *palette,int first,int last);
void gfx_colorSet(const gfx_screenInfo_t *scr,int bg,int fg);
void gfx_fontSet(const gfx_screenInfo_t *_scr,int fnt);
int gfx_fontHeightGet(const gfx_screenInfo_t *_scr,int *h);
int gfx_charDraw(const gfx_screenInfo_t *scr,int x,int y,char ch);
int gfx_charInfo(const gfx_screenInfo_t *scr,int *w,int *h,char ch);
void gfx_plot(const gfx_screenInfo_t *scr,int x,int y);
void gfx_hline(const gfx_screenInfo_t *scr,int x,int y,int w);
void gfx_vline(const gfx_screenInfo_t *scr,int x,int y,int h);
void gfx_rectangle(const gfx_screenInfo_t *scr,int x,int y,int w,int h);
void gfx_frectangle(const gfx_screenInfo_t *scr,int x,int y,int w,int h);
void gfx_circle(const gfx_screenInfo_t *scr,int x,int y,int r);
void gfx_fcircle(const gfx_screenInfo_t *scr,int x,int y,int r);
void gfx_ellipse (const gfx_screenInfo_t *scr,int x0, int y0,int rx,int ry);
void gfx_fellipse (const gfx_screenInfo_t *scr,int x0, int y0,int rx,int ry);
void gfx_line(const gfx_screenInfo_t *scr,int x1,int y1,int x2,int y2);
void gfx_scrollUp(const gfx_screenInfo_t *scr,int n);
void gfx_putchar(const gfx_screenInfo_t *scr,int c);

#define LITTLE_FNT 0
#define SMALL_FNT 1
#define BIG_FNT 2

/*
** HW interface
*/
#ifdef __cplusplus
extern "C" {
#endif
const gfxraw_screenInfo_t *raw_init(void);
void raw_clrScr(const gfxraw_screenInfo_t *scr,int fill);
void raw_penSet(const gfxraw_screenInfo_t *scr,int pen,__u8 r,__u8 g,__u8 b);
void raw_plot(const gfxraw_screenInfo_t *scr,int x,int y,int c);
void raw_hline(const gfxraw_screenInfo_t *scr,int x,int y,int w,int c);
void raw_vline(const gfxraw_screenInfo_t *scr,int x,int y,int h,int c);
void raw_drawChar(const gfxraw_screenInfo_t *scr,
		  const gfx_font_t *font,
		  int x,int y,
		  int bg,int fg,
		  char ch);

void raw_scrollUp(const gfxraw_screenInfo_t *scr,int n,int fill);


#ifdef __cplusplus
}
#endif
#endif
