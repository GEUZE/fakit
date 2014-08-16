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
** +Revision: 1.8 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Simple GFX library                                               **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <bin2const.h>
#include <string.h>
#include "gfx/simple.h"

#ifndef ABS
#define ABS(a) (a) < 0 ? -(a) : (a)
#endif
#define SWAP(a,b) do{ int h = a; a = b; b = h; }while(0)

#ifdef __cplusplus
extern __u8 big_fnt[];
extern __u8 small_fnt[];
extern __u8 little_fnt[];
#else
extern gfx_rawFont_t little_fnt;
extern gfx_rawFont_t small_fnt;
extern gfx_rawFont_t big_fnt;
#endif

gfx_rawFont_t *font_tab[] = {
  (gfx_rawFont_t *)&little_fnt,
  (gfx_rawFont_t *)&small_fnt,
  (gfx_rawFont_t *)&big_fnt
};
    

static gfx_screenInfo_t info = {
  320,240,           // w,h
  256,               // # color
  -1,                // trans_color
  0,                 // bg
  0xff,              // fg
  0,                 // inv
  0,                 // rawfont
  { 0 },             // font
  0,0,               // char_x,char_y
  0                  // raw-screen info
};

const gfx_screenInfo_t * gfx_Init(void)
{
  const gfxraw_screenInfo_t *raw;

  raw = raw_init();
  
  info.rawscreen = raw;
  info.width = raw->width;
  info.height = raw->height;
  info.colors = raw->colors;
  return &info;
}

void gfx_clrScr(const gfx_screenInfo_t *_scr)
{
  gfx_screenInfo_t *scr = (gfx_screenInfo_t *)_scr;
  raw_clrScr(scr->rawscreen,scr->bg);
  scr->char_x = scr->char_y = 0;
}

void gfx_paletteSet(const gfx_screenInfo_t *scr,
		    __u8 *palette,int first,int last)
{
  __u8 r,g,b;
  if ( first > last || first < 0 || last > scr->colors ) return;

  while( first <= last ){
    r = *palette++;
    g = *palette++;
    b = *palette++;
    raw_penSet(scr->rawscreen,first++,r,g,b);
  }
}

void gfx_colorSet(const gfx_screenInfo_t *_scr,int bg,int fg)
{
  gfx_screenInfo_t * scr = (gfx_screenInfo_t *)_scr;
  scr->fg = fg;
  scr->bg = bg;
}
  
void gfx_fontSet(const gfx_screenInfo_t *_scr,int fnt)
{
  gfx_screenInfo_t * scr = (gfx_screenInfo_t *)_scr;
  gfx_font_t *font = &scr->font;
  gfx_rawFont_t *rawFont;
  int nch;
  if ( fnt < 0 || fnt >= (int)(sizeof(font_tab)/sizeof(gfx_font_t *)) ) return;
  rawFont = scr->rawFont = font_tab[fnt];  
  font->first = rawFont->first;  
  font->last = rawFont->last;
  nch = rawFont->last - rawFont->first + 1;
  font->h = rawFont->h;
  font->lineOffset = (__u32)rawFont->lineOffset;
  font->bitLen = &rawFont->data[0];
  font->byteOffset = &rawFont->data[nch];
  font->bitOffset = &rawFont->data[nch*2];
  font->data = &rawFont->data[nch*3];
}

int gfx_fontHeightGet(const gfx_screenInfo_t *scr,int *h)
{
  if ( ! scr->rawFont ) return 0;
  *h = scr->font.h;
  return 1;
}

int gfx_charDraw(const gfx_screenInfo_t *scr,int x,int y,char ch)
{
  int w;
  int fg, bg;
  if ( ! scr->rawFont ) return 0;
  if ( ch < scr->font.first || ch > scr->font.last ) return 0;
  ch -= scr->font.first;
  w = scr->font.bitLen[(int)ch];
  if ( x < 0 || x+w > scr->width-1 || 
       y < 0 || y+scr->font.h > scr->height-1 ) return -w;

  if ( scr->inv ){
    fg = scr->bg;
    bg = scr->fg;
  } else {
    fg = scr->fg;
    bg = scr->bg;
  } 

  raw_drawChar(scr->rawscreen,&scr->font,x,y,bg,fg,ch);

  return w;
}

int gfx_charInfo(const gfx_screenInfo_t *scr,int *w,int *h,char ch)
{
  if ( ! scr->rawFont ) return 0;
  if ( ch < scr->font.first || ch > scr->font.last ) return 0;
  ch -= scr->font.first;
  *w = scr->font.bitLen[(int)ch];
  *h = scr->font.h;
  return 1;
}


void gfx_vline(const gfx_screenInfo_t *scr,int x,int y,int h)
{
  if ( !h ) return;
  if ( h < 0 ){
    h = -h;
    y -= h - 1;
  }	     
  if ( y < 0 ){
    h += y;
    y = 0;
  }
  if ( y > scr->height ) return;
  if ( y + h > scr->height+1 ){
    h -= (y + h) - scr->height - 1;
  }
  raw_vline(scr->rawscreen,x,y,h,scr->inv ? scr->bg : scr->fg);
}

void gfx_hline(const gfx_screenInfo_t *scr,int x,int y,int w)
{
  if ( !w ) return;
  if ( w < 0 ){
    w = -w;
    x -= w - 1;
  }
  if ( x < 0 ){
    w += x;
    x  = 0;
  }
  if ( x > scr->width-1 ) return;
  if ( x + w > scr->width ){
    w -= (x+w)-scr->width;
  }
  raw_hline(scr->rawscreen,x,y,w,scr->inv ? scr->bg : scr->fg);
}

void gfx_plot(const gfx_screenInfo_t *scr,int x,int y)
{
  if ( x < 0 || x > scr->width-1 || y < 0 || y > scr->height-1 ) return;  
  raw_plot(scr->rawscreen,x,y, scr->inv ? scr->bg : scr->fg );
}

void gfx_rectangle(const gfx_screenInfo_t *scr,int x,int y,int w,int h)
{
  int c;
  int q = B8(1111);

  if ( !w || !h ) return;

  c = scr->inv ? scr->bg : scr->fg;

  if ( w < 0 ){
    w = -w;
    x -= w - 1;
  }
  if ( h < 0 ){
    h = -h;
    y -= h - 1;
  }
  if ( x >= scr->width ){
    return;
  }
  if ( y >= scr->height ){
    return;
  }

  if ( x < 0 ){
    w += x;
    x = 0;
    q &= B8(0111);
  }
  if ( x+w > scr->width ){
    w -= (x + w) - scr->width;
    q &= B8(1011);
  }

  if ( y < 0 ){
    h += y;
    y = 0;
    q &= B8(1101);
  }
  if ( y+h > scr->height ){
    h -= (y + h) - scr->height;
    q &= B8(1110);
  }
  
  if ( q & B8(1000) ){
    raw_vline(scr->rawscreen,x,y,h,c);
  }
  if ( q & B8(0100) ){
    raw_vline(scr->rawscreen,x+w-1,y,h,c);
  }
  if ( q & B8(0010) ){
    raw_hline(scr->rawscreen,x,y,w,c);
  }
  if ( q & B8(0001) ){
    raw_hline(scr->rawscreen,x,y+h-1,w,c);
  }
}

void gfx_frectangle(const gfx_screenInfo_t *scr,int x,int y,int w,int h)
{
  int c;

  if ( !w || !h ) return;

  c = scr->inv ? scr->bg : scr->fg;

  if ( w < 0 ){
    w = -w;
    x -= w - 1;
  }
  if ( h < 0 ){
    h = -h;
    y -= h - 1;
  }
  if ( x < 0 ){
    w += x;
    x = 0;
  }
  if ( x+w > scr->width ){
    w -= (x + w) - scr->width;
  }
  if ( y < 0 ){
    h += y;
    y = 0;
  }
  if ( y+h > scr->height ){
    h -= (y + h) - scr->height;
  }
  
  while( h-- ){
    raw_hline(scr->rawscreen,x,y,w,c);
    ++y;
  }
}

void gfx_line(const gfx_screenInfo_t *scr,int x1,int y1,int x2,int y2)
{
  int dx;
  int dy;
  int incr;
  int d;
  int aincr,bincr;
  int x,y;
  int c;
  c = scr->inv ? scr->bg : scr->fg;

  if (x1 == x2){
    if ( y1 < y2 ){
      gfx_vline(scr,x1,y1,y2-y1+1);
    } else {
      gfx_vline(scr,x1,y2,y1-y2+1);
    }
    return;
  }
  if (y1 == y2){
    if ( x1 < x2 ){
      gfx_hline(scr,x1,y1,x2-x1+1);
    } else {
      gfx_hline(scr,x2,y1,x1-x2+1);
    }
    return;
  }
  dx = ABS(x2-x1);
  dy = ABS(y2-y1);
  incr = 1;
  if (dx >= dy){
    if (x1>x2){
      SWAP(x1, x2);
      SWAP(y1, y2);
    }
    if (y2<y1){
      incr = -1;
    }
    d = 2 * dy - dx;
    aincr = 2 * (dy - dx);
    bincr = 2 * dy;
    x = x1;
    y = y1;
    gfx_plot(scr,x, y);
    while (++x <= x2){
      if (d >= 0){
	y += incr;
	d += aincr;
      } else {
	d += bincr;
      }
      if ( x >= scr->width || y >= scr->height ){
	break;
      }
      raw_plot(scr->rawscreen,x,y, c );
    }
  } else {
    if (y1>y2){
      SWAP(x1, x2);
      SWAP(y1, y2);
    }                           
    if (x2<x1){
      incr = -1;
    }
    d = 2*dx - dy;
    aincr = 2 * (dx - dy);
    bincr = 2 * dx;
    x = x1;
    y = y1;
    gfx_plot(scr,x, y);
    while (++y <= y2){
      if (d >= 0){
	x += incr;
	d += aincr;
      } else {
	d += bincr;
      }
      if ( x >= scr->width || y >= scr->height ){
	break;
      }
      raw_plot(scr->rawscreen,x,y, c );
    }
  }
}

void gfx_scrollUp(const gfx_screenInfo_t *scr,int n)
{
  raw_scrollUp(scr->rawscreen,n,scr->bg);
}

void gfx_putchar(const gfx_screenInfo_t *_scr,int c)
{
  gfx_screenInfo_t *scr = (gfx_screenInfo_t *)_scr;
  int w,h;
  int fg,bg;

  if ( !scr ){
    return;
  }

  switch( c ) {
  case '\n':			/* Newline found		*/
    scr->char_y += scr->font.h;
    if ( scr->char_y >= scr->height ){
      gfx_scrollUp(scr,scr->font.h);
      scr->char_y -= scr->font.h;
    } 

    /* fall thru .. */
  case '\r':			/* Carriage return found	*/
    scr->char_x = 0;
    break;
    
  case '\t':
    scr->char_x = (scr->char_x+16) & ~15;
    break;
    
  case 8:/* Delete */
    break;    
  default:			/* Normal character             */
    if ( gfx_charInfo(scr,&w,&h,c) ){
      bg = scr->bg;
      fg = scr->fg;
      if ( scr->char_x + w >= scr->width ){
	scr->char_x = 0;
	scr->char_y += h;
      }
      while ( scr->char_y + h >= scr->height ){
	gfx_scrollUp(scr,h);
	scr->char_y -= h;
      } 

      scr->char_x += gfx_charDraw(scr,scr->char_x,scr->char_y,c);
    }
  }
}

#define CIRCLE_PTS() do {\
    gfx_plot(scr,x0+x,y0+y); \
    gfx_plot(scr,x0+x,y0-y); \
    gfx_plot(scr,x0+y,y0+x); \
    gfx_plot(scr,x0+y,y0-x); \
    gfx_plot(scr,x0-x,y0+y); \
    gfx_plot(scr,x0-x,y0-y); \
    gfx_plot(scr,x0-y,y0+x); \
    gfx_plot(scr,x0-y,y0-x); \
    } while (0)


void gfx_circle(const gfx_screenInfo_t *scr,int x0, int y0, int r)
{
  int x,y;
  int deltaE;
  int deltaSE;
  int d;

  x = 0;
  y = r;
  deltaE = 3;
  deltaSE = 5-r*2;
  d = 1-r;
  while( x <= y ){
    CIRCLE_PTS();
    ++x;
    if ( d < 0 ){
      d += deltaE;
      deltaE += 2;
      deltaSE += 2;
    } else {
      d += deltaSE;
      deltaE += 2;
      deltaSE += 4;
      --y;
    }
  }    
}

void gfx_fcircle(const gfx_screenInfo_t *scr,int x0, int y0, int r)
{
  int x,y;
  int deltaE;
  int deltaSE;
  int d;

  x = 0;
  y = r;
  deltaE = 3;
  deltaSE = 5-r*2;
  d = 1-r;

  while( x < y ){
    if ( y0+y < scr->height ){
      gfx_hline(scr,x0-x,y0+y,x<<1); 
    }
    if ( y0-y >= 0 ){
      gfx_hline(scr,x0-x,y0-y,x<<1);
    }
    if ( y0+x < scr->height ){
      gfx_hline(scr,x0-y,y0+x,y<<1);
    }
    if ( y0-x >= 0 ){
      gfx_hline(scr,x0-y,y0-x,y<<1);
    }

    ++x;

    if ( d < 0 ){
      d += deltaE;
      deltaE += 2;
      deltaSE += 2;
    } else {
      d += deltaSE;
      deltaE += 2;
      deltaSE += 4;
      --y;
    }
  }
}

void gfx_ellipse (const gfx_screenInfo_t *scr,int x0, int y0, int rx, int ry)
{
/* intermediate terms to speed up loop */
  long t1 = rx*rx, t2 = t1<<1, t3 = t2<<1;
  long t4 = ry*ry, t5 = t4<<1, t6 = t5<<1;
  long t7 = rx*t5, t8 = t7<<1, t9 = 0L;
  long d1 = t2 - t7 + (t4>>1);/* error terms */
  long d2 = (t1>>1) - t8 + t5;

  int x = rx, y = 0;/* ellipse points */

  while (d2 < 0)/* til slope = -1 */
  {
    /* draw 4 points using symmetry */
    gfx_plot(scr,x0+x,y0+y);
    gfx_plot(scr,x0+x,y0-y);
    gfx_plot(scr,x0-x,y0+y);
    gfx_plot(scr,x0-x,y0-y);

    y++;/* always move up here */
    t9 += t3;
    if (d1 < 0)/* move straight up */
    {
      d1 += t9 + t2;
      d2 += t9;
    }
    else/* move up and left */
    {
      x--;
      t8 -= t6;
      d1 += t9 + t2 - t8;
      d2 += t9 + t5 - t8;
    }
  }
  
  do /* rest of top right quadrant */
  {
    /* draw 4 points using symmetry */
    gfx_plot(scr,x0+x,y0+y);
    gfx_plot(scr,x0+x,y0-y);
    gfx_plot(scr,x0-x,y0+y);
    gfx_plot(scr,x0-x,y0-y);

    x--;/* always move left here */
    t8 -= t6;
    if (d2 < 0)/* move up and left */
    {
      y++;
      t9 += t3;
      d2 += t9 + t5 - t8;
    }
    else/* move straight left */
      d2 += t5 - t8;
  } while (x>=0);
}

void gfx_fellipse (const gfx_screenInfo_t *scr,int x0, int y0, int rx, int ry)
{
  long t1 = rx*rx, t2 = t1<<1, t3 = t2<<1;
  long t4 = ry*ry, t5 = t4<<1, t6 = t5<<1;
  long t7 = rx*t5, t8 = t7<<1, t9 = 0;
  long d1 = t2 - t7 + (t4>>1);/* error terms */
  long d2 = (t1>>1) - t8 + t5;
  int x = rx, y = 0;/* ellipse points */

  while (d2 < 0)/* til slope = -1 */
  {
    /* fill in leftward to inner ellipse */
    gfx_hline(scr,x0-x,y0+y,2*x+1);
    gfx_hline(scr,x0-x,y0-y,2*x+1);

    y++;/* always move up here */
    t9 += t3;
    if (d1 < 0)/* move straight up */
    {
      d1 += t9 + t2;
      d2 += t9;
    }
    else/* move up and left */
    {
      x--;
      t8 -= t6;
      d1 += t9 + t2 - t8;
      d2 += t9 + t5 - t8;
    }
  }
  
  do /* rest of top right quadrant */
  {
    /* fill in downward to inner ellipse */
    gfx_hline(scr,x0-x,y0-y,2*x+1);
    gfx_hline(scr,x0-x,y0+y,2*x+1);
    
    x--;/* always move left here */
    t8 -= t6;
    if (d2 < 0)/* move up and left */
    {
      y++;
      t9 += t3;
      d2 += t9 + t5 - t8;
    }
    else/* move straight left */
    {
      d2 += t5 - t8;
    }
  } while (x>=0);
}
