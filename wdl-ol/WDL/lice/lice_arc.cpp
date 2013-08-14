#include "lice.h"
#include "lice_combine.h"
#include <math.h>

#define _PI 3.141592653589793238f

template <class T> inline void _SWAP(T& a, T& b) { T tmp = a; a = b; b = tmp; }

#define A(x) ((LICE_pixel_chan)((x)*255.0+0.5))

static bool CachedCircle(LICE_IBitmap* dest, float cx, float cy, float r, LICE_pixel color, float alpha, int mode, bool aa)
{
  // fast draw for some small circles 
  if (r == 1.5f)
  {
    if (aa) 
    {
      LICE_pixel_chan alphas[25] = 
      {
        A(0.31), A(1.00), A(1.00), A(0.31),
        A(1.00), A(0.06), A(0.06), A(1.00),
        A(1.00), A(0.06), A(0.06), A(1.00),
        A(0.31), A(1.00), A(1.00), A(0.31),
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 4, 4, alpha, mode);
    }
    else 
    {
      LICE_pixel_chan alphas[25] = 
      {
        A(0.00), A(1.00), A(1.00), A(0.00),
        A(1.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(1.00),
        A(0.00), A(1.00), A(1.00), A(0.00),
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 4, 4, alpha, mode);    
    }
    return true;
  }
  else if (r == 2.0f)
  {
    if (aa) 
    {
      LICE_pixel_chan alphas[25] = 
      {
        A(0.06), A(0.75), A(1.00), A(0.75), A(0.06),
        A(0.75), A(0.82), A(0.31), A(0.82), A(0.75),
        A(1.00), A(0.31), A(0.00), A(0.31), A(1.00),
        A(0.75), A(0.82), A(0.31), A(0.82), A(0.75),
        A(0.06), A(0.75), A(1.00), A(0.75), A(0.06)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 5, 5, alpha, mode);
    }
    else 
    {
      LICE_pixel_chan alphas[25] = 
      {
        A(0.00), A(0.00), A(1.00), A(0.00), A(0.00),
        A(0.00), A(1.00), A(0.00), A(1.00), A(0.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(0.00), A(1.00), A(0.00), A(1.00), A(0.00),
        A(0.00), A(0.00), A(1.00), A(0.00), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 5, 5, alpha, mode);    
    }
    return true;
  }
  else if (r == 2.5f) {
    if (aa) {
      LICE_pixel_chan alphas[36] = {
        A(0.06), A(0.75), A(1.00), A(1.00), A(0.75), A(0.06),
        A(0.75), A(0.82), A(0.31), A(0.31), A(0.82), A(0.75),
        A(1.00), A(0.31), A(0.00), A(0.00), A(0.31), A(1.00),
        A(1.00), A(0.31), A(0.00), A(0.00), A(0.31), A(1.00),
        A(0.75), A(0.82), A(0.31), A(0.31), A(0.82), A(0.75),
        A(0.06), A(0.75), A(1.00), A(1.00), A(0.75), A(0.06)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 6, 6, alpha, mode);
    }
    else {
      LICE_pixel_chan alphas[36] = {
        A(0.00), A(0.00), A(1.00), A(1.00), A(0.00), A(0.00),
        A(0.00), A(1.00), A(0.00), A(0.00), A(1.00), A(0.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(0.00), A(1.00), A(0.00), A(0.00), A(1.00), A(0.00),
        A(0.00), A(0.00), A(1.00), A(1.00), A(0.00), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 6, 6, alpha, mode);    
    }
    return true;
  }
  else if (r == 3.0f) {
    if (aa) {
      LICE_pixel_chan alphas[49] = {
        A(0.00), A(0.56), A(1.00), A(1.00), A(1.00), A(0.56), A(0.00),
        A(0.56), A(1.00), A(0.38), A(0.25), A(0.38), A(1.00), A(0.56),
        A(1.00), A(0.44), A(0.00), A(0.00), A(0.00), A(0.44), A(1.00),
        A(1.00), A(0.19), A(0.00), A(0.00), A(0.00), A(0.19), A(1.00),
        A(1.00), A(0.44), A(0.00), A(0.00), A(0.00), A(0.44), A(1.00),
        A(0.56), A(1.00), A(0.38), A(0.25), A(0.38), A(1.00), A(0.56),
        A(0.00), A(0.56), A(1.00), A(1.00), A(1.00), A(0.56), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 7, 7, alpha, mode);
    }
    else {
      LICE_pixel_chan alphas[49] = {
        A(0.00), A(0.00), A(1.00), A(1.00), A(1.00), A(0.00), A(0.00),
        A(0.00), A(1.00), A(0.00), A(0.00), A(0.00), A(1.00), A(0.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(0.00), A(1.00), A(0.00), A(0.00), A(0.00), A(1.00), A(0.00),
        A(0.00), A(0.00), A(1.00), A(1.00), A(1.00), A(0.00), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 7, 7, alpha, mode);    
    }
    return true;
  }
  else if (r == 3.5f) {
    if (aa) {
      LICE_pixel_chan alphas[64] = {
        A(0.00), A(0.31), A(0.87), A(1.00), A(1.00), A(0.87), A(0.31), A(0.00),
        A(0.31), A(1.00), A(0.69), A(0.25), A(0.25), A(0.69), A(1.00), A(0.31),
        A(0.87), A(0.69), A(0.00), A(0.00), A(0.00), A(0.00), A(0.69), A(0.87),
        A(1.00), A(0.25), A(0.00), A(0.00), A(0.00), A(0.00), A(0.25), A(1.00),
        A(1.00), A(0.25), A(0.00), A(0.00), A(0.00), A(0.00), A(0.25), A(1.00),
        A(0.87), A(0.69), A(0.00), A(0.00), A(0.00), A(0.00), A(0.69), A(0.87),
        A(0.31), A(1.00), A(0.69), A(0.25), A(0.25), A(0.69), A(1.00), A(0.31),
        A(0.00), A(0.31), A(0.87), A(1.00), A(1.00), A(0.87), A(0.31), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 8, 8, alpha, mode);
    }
    else {
      LICE_pixel_chan alphas[64] = {
        A(0.00), A(0.00), A(1.00), A(1.00), A(1.00), A(1.00), A(0.00), A(0.00),
        A(0.00), A(1.00), A(1.00), A(0.00), A(0.00), A(1.00), A(1.00), A(0.00),
        A(1.00), A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00), A(1.00),
        A(0.00), A(1.00), A(1.00), A(0.00), A(0.00), A(1.00), A(1.00), A(0.00),
        A(0.00), A(0.00), A(1.00), A(1.00), A(1.00), A(1.00), A(0.00), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 8, 8, alpha, mode);
    }
    return true;
  }
  else if (r == 4.0f) {
    if (aa) {
      LICE_pixel_chan alphas[81] = {
        A(0.00), A(0.12), A(0.69), A(1.00), A(1.00), A(1.00), A(0.69), A(0.12), A(0.00),
        A(0.12), A(0.94), A(0.82), A(0.31), A(0.25), A(0.31), A(0.82), A(0.94), A(0.12),
        A(0.69), A(0.82), A(0.06), A(0.00), A(0.00), A(0.00), A(0.06), A(0.82), A(0.69),
        A(1.00), A(0.31), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.31), A(1.00),
        A(1.00), A(0.19), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.19), A(1.00),
        A(1.00), A(0.31), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.31), A(1.00),
        A(0.69), A(0.82), A(0.06), A(0.00), A(0.00), A(0.00), A(0.06), A(0.82), A(0.69),
        A(0.12), A(0.94), A(0.82), A(0.31), A(0.25), A(0.31), A(0.82), A(0.94), A(0.12),
        A(0.00), A(0.12), A(0.69), A(1.00), A(1.00), A(1.00), A(0.69), A(0.12), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 9, 9, alpha, mode);
    }
    else {
      LICE_pixel_chan alphas[81] = {
        A(0.00), A(0.00), A(1.00), A(1.00), A(1.00), A(1.00), A(1.00), A(0.00), A(0.00),
        A(0.00), A(1.00), A(1.00), A(0.00), A(0.00), A(0.00), A(1.00), A(1.00), A(0.00),
        A(1.00), A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00),
        A(1.00), A(1.00), A(0.00), A(0.00), A(0.00), A(0.00), A(0.00), A(1.00), A(1.00),
        A(0.00), A(1.00), A(1.00), A(0.00), A(0.00), A(0.00), A(1.00), A(1.00), A(0.00),
        A(0.00), A(0.00), A(1.00), A(1.00), A(1.00), A(1.00), A(1.00), A(0.00), A(0.00)
      };
      LICE_DrawGlyph(dest, cx-r, cy-r, color, alphas, 9, 9, alpha, mode);
    }
    return true;
  }

  return false;
}


template <class COMBFUNC> class _LICE_CircleDrawer
{
public:

  static void DrawClippedPt(LICE_IBitmap* dest, int x, int y, int clip[], 
    int r, int g, int b, int a, int alpha, bool doclip)
  {
    if (doclip && (x < clip[0] || x >= clip[2] || y < clip[1] || y >= clip[3])) return;
    LICE_pixel* px = dest->getBits()+y*dest->getRowSpan()+x;
    COMBFUNC::doPix((LICE_pixel_chan*)px, r, g, b, a, alpha);
  }

  static void DrawClippedHorzLine(LICE_IBitmap* dest, int y, int xlo, int xhi, int clip[],
    int r, int g, int b, int a, int alpha, bool doclip)
  {
    if (doclip) {
      if (y < clip[1] || y >= clip[3]) return;
      xlo = max(xlo, clip[0]);
      xhi = min(xhi, clip[2]-1);
    }
    LICE_pixel* px = dest->getBits()+y*dest->getRowSpan()+xlo;
    while (xlo <= xhi) {
      COMBFUNC::doPix((LICE_pixel_chan*)px, r, g, b, a, alpha);
      ++px;
      ++xlo;
    }    
  }

  static void DrawClippedCircleAA(LICE_IBitmap* dest, float cx, float cy, float rad,
    int clip[], LICE_pixel color, int ai, bool filled, bool doclip)
  {
    int r = LICE_GETR(color), g = LICE_GETG(color), b = LICE_GETB(color), a = LICE_GETA(color);

    float r2 = rad*rad;
    float x = 0.0f;
    float y = rad;
    while (x < y+1.0f) 
    {
      float w = y-floor(y);

      if (w == 0.0f) 
      { 
        DrawClippedPt(dest, cx+x, cy+y, clip, r, g, b, a, ai, doclip);
        DrawClippedPt(dest, cx+x, cy-y, clip, r, g, b, a, ai, doclip);
        DrawClippedPt(dest, cx-x, cy+y, clip, r, g, b, a, ai, doclip);
        DrawClippedPt(dest, cx-x, cy-y, clip, r, g, b, a, ai, doclip);
        DrawClippedPt(dest, cx+y, cy+x, clip, r, g, b, a, ai, doclip);
        DrawClippedPt(dest, cx+y, cy-x, clip, r, g, b, a, ai, doclip);
        DrawClippedPt(dest, cx-y, cy+x, clip, r, g, b, a, ai, doclip);
        DrawClippedPt(dest, cx-y, cy-x, clip, r, g, b, a, ai, doclip);
      }
      else 
      { 
        int wa = (int)(ai*w);
        int iwa = ai-wa;

        DrawClippedPt(dest, cx+x, cy+y+1, clip, r, g, b, a, wa, doclip);
        DrawClippedPt(dest, cx+x, cy+y, clip, r, g, b, a, iwa, doclip);

        DrawClippedPt(dest, cx+x, cy-y+1, clip, r, g, b, a, iwa, doclip);
        DrawClippedPt(dest, cx+x, cy-y, clip, r, g, b, a, wa, doclip);

        DrawClippedPt(dest, cx-x, cy+y+1, clip, r, g, b, a, wa, doclip);
        DrawClippedPt(dest, cx-x, cy+y, clip, r, g, b, a, iwa, doclip);

        DrawClippedPt(dest, cx-x, cy-y+1, clip, r, g, b, a, iwa, doclip);
        DrawClippedPt(dest, cx-x, cy-y, clip, r, g, b, a, wa, doclip);

        DrawClippedPt(dest, cx+y+1, cy+x, clip, r, g, b, a, wa, doclip);
        DrawClippedPt(dest, cx+y, cy+x, clip, r, g, b, a, iwa, doclip);

        DrawClippedPt(dest, cx+y+1, cy-x, clip, r, g, b, a, wa, doclip);
        DrawClippedPt(dest, cx+y, cy-x, clip, r, g, b, a, iwa, doclip);

        DrawClippedPt(dest, cx-y+1, cy+x, clip, r, g, b, a, iwa, doclip);
        DrawClippedPt(dest, cx-y, cy+x, clip, r, g, b, a, wa, doclip);

        DrawClippedPt(dest, cx-y+1, cy-x, clip, r, g, b, a, iwa, doclip);
        DrawClippedPt(dest, cx-y, cy-x, clip, r, g, b, a, wa, doclip);
      }

      if (filled) 
      {
        DrawClippedHorzLine(dest, cy-y+1, cx-x, cx+x, clip, r, g, b, a, ai, doclip);
        DrawClippedHorzLine(dest, cy+y, cx-x, cx+x, clip, r, g, b, a, ai, doclip);
        DrawClippedHorzLine(dest, cy-x, cx-y+1, cx+y, clip, r, g, b, a, ai, doclip);
        DrawClippedHorzLine(dest, cy+x, cx-y+1, cx+y, clip, r, g, b, a, ai, doclip);
      }

      x += 1;
      y = sqrt(r2-x*x);
    }
  }

  static void DrawClippedCircle(LICE_IBitmap* dest, float cx, float cy, float rad,
    int clip[], LICE_pixel color, int ai, bool filled, bool doclip)
  {
    int r = LICE_GETR(color), g = LICE_GETG(color), b = LICE_GETB(color), a = LICE_GETA(color);

    float x = 0.0;
    float y = rad;
    float p = (5.0-rad*4.0)/4.0;

    DrawClippedPt(dest, cx, cy+rad, clip, r, g, b, a, ai, doclip);
    DrawClippedPt(dest, cx, cy-rad, clip, r, g, b, a, ai, doclip);
    DrawClippedPt(dest, cx+rad, cy, clip, r, g, b, a, ai, doclip);
    DrawClippedPt(dest, cx-rad, cy, clip, r, g, b, a, ai, doclip);

    if (filled) {
      DrawClippedHorzLine(dest, cy, cx-rad+1, cx+rad-1, clip, r, g, b, a, ai, doclip);
    }
    
    while (x < y) {
      x += 1.0f;

      if (p < 0) {
        p += 2.0f*x+1.0f;
      }
      else {
        y -= 1.0f;
        p += 2.0*(x-y)+1.0f;
      }

      DrawClippedPt(dest, cx+x, cy+y, clip, r, g, b, a, ai, doclip);
      DrawClippedPt(dest, cx-x, cy+y, clip, r, g, b, a, ai, doclip);
      DrawClippedPt(dest, cx+x, cy-y, clip, r, g, b, a, ai, doclip);
      DrawClippedPt(dest, cx-x, cy-y, clip, r, g, b, a, ai, doclip);
      DrawClippedPt(dest, cx+y, cy+x, clip, r, g, b, a, ai, doclip);
      DrawClippedPt(dest, cx-y, cy+x, clip, r, g, b, a, ai, doclip);
      DrawClippedPt(dest, cx+y, cy-x, clip, r, g, b, a, ai, doclip);
      DrawClippedPt(dest, cx-y, cy-x, clip, r, g, b, a, ai, doclip);

      if (filled) {
        DrawClippedHorzLine(dest, cy-y, cx-x+1, cx+x-1, clip, r, g, b, a, ai, doclip);
        DrawClippedHorzLine(dest, cy+y, cx-x+1, cx+x-1, clip, r, g, b, a, ai, doclip);
        DrawClippedHorzLine(dest, cy-x, cx-y+1, cx+y-1, clip, r, g, b, a, ai, doclip);
        DrawClippedHorzLine(dest, cy+x, cx-y+1, cx+y-1, clip, r, g, b, a, ai, doclip);
      }
    }
  }


};


static void __DrawCircleClipped(LICE_IBitmap* dest, float cx, float cy, float rad,
  LICE_pixel color, int ia, bool aa, bool filled, int mode, int *clip, bool doclip)
{
  // todo: more clipped/filled versions (to optimize constants out?)
  if (aa) 
  {
    #define __LICE__ACTION(COMBFUNC) _LICE_CircleDrawer<COMBFUNC>::DrawClippedCircleAA(dest, cx, cy, rad, clip, color, ia, filled, doclip)
      __LICE_ACTION_NOSRCALPHA(mode, ia,false)
    #undef __LICE__ACTION
  }
  else 
  {
    #define __LICE__ACTION(COMBFUNC) _LICE_CircleDrawer<COMBFUNC>::DrawClippedCircle(dest, cx, cy, rad, clip, color, ia, filled, doclip)
      __LICE_ACTION_CONSTANTALPHA(mode,ia,false)
    #undef __LICE__ACTION
  }
}


static void __DrawArc(int w, int h, LICE_IBitmap* dest, float cx, float cy, float rad, float anglo, float anghi,
  LICE_pixel color, int ialpha, bool aa, int mode)
{
  // -2PI <= anglo <= anghi <= 2PI

  if (anglo < -_PI && anghi > -_PI)
  {   
    __DrawArc(w,h,dest, cx, cy, rad, anglo, -_PI, color, ialpha, aa,mode);
    anglo=-_PI;
  }
     
  if (anglo < 0.0f && anghi > 0.0f)
  {   
    __DrawArc(w,h,dest, cx, cy, rad, anglo+2.0f*_PI, 2.0f*_PI, color, ialpha, aa,mode);
    anglo=0.0f;
  }
  
  if (anglo < _PI && anghi > _PI)
  {
    __DrawArc(w,h,dest, cx, cy, rad, anglo, _PI, color, ialpha, aa,mode);
    anglo=_PI;
  }

  int ylo = (int) (cy-rad*cos(anglo)+0.5);
  int yhi = (int) (cy-rad*cos(anghi)+0.5);
    
  if (yhi < ylo) { int tmp = ylo; ylo = yhi; yhi=tmp; }
  
  int clip[4]={0,max(0, ylo),w,min(h, yhi+1)};

  if (anglo < -_PI || (anglo >= 0.0f && anglo < _PI))
  {
    if (cx>0) clip[0]=cx;
  }
  else
  {
    if (cx<w) clip[2]=cx;
  }

  __DrawCircleClipped(dest,cx,cy,rad,color,ialpha,aa,false,mode,clip,true);
}

void LICE_Arc(LICE_IBitmap* dest, float cx, float cy, float r, float minAngle, float maxAngle, 
	LICE_pixel color, float alpha, int mode, bool aa)
{
  if (!dest) return;

  if (dest->isFlipped()) { cy=dest->getHeight()-1-cy; minAngle=_PI-minAngle; maxAngle=_PI-maxAngle; }

  if (maxAngle < minAngle)
  {
    float tmp=maxAngle; 
    maxAngle=minAngle;
    minAngle=tmp;
  }

  if (maxAngle - minAngle >= 2.0f*_PI) 
  {
    LICE_Circle(dest,cx,cy,r,color,alpha,mode,aa);
    return;
  }

  if (maxAngle >= 2.0f*_PI)
  {
    float tmp = fmod(maxAngle,2.0f*_PI);
    minAngle -= maxAngle - tmp; // reduce by factors of 2PI 
    maxAngle = tmp;
  }
  else if (minAngle <= -2.0f*_PI)
  {
    float tmp = fmod(minAngle,2.0f*_PI);
    maxAngle -= minAngle - tmp; // toward zero by factors of 2pi
    minAngle = tmp;
  }

  // -2PI <= minAngle <= maxAngle <= 2PI

  int ia = (int) (alpha*256.0f);
  if (!ia) return;

  __DrawArc(dest->getWidth(),dest->getHeight(),dest,cx,cy,r,minAngle,maxAngle,color,ia,aa,mode);
}




void LICE_Circle(LICE_IBitmap* dest, float cx, float cy, float r, LICE_pixel color, float alpha, int mode, bool aa)
{
  if (!dest) return;
  if (CachedCircle(dest, cx, cy, r, color, alpha, mode, aa)) return;  

  if (dest->isFlipped()) cy=dest->getHeight()-1-cy;

  int ia = (int) (alpha*256.0f);
  if (!ia) return;

  int w = dest->getWidth(), h = dest->getHeight();
  int clip[4] = { 0, 0, w, h };

  bool doclip = (cx-r-1 < 0 || cy-r-1 < 0 || cx+r+1 >= w || cy+r+1 >= h);

  __DrawCircleClipped(dest,cx,cy,r,color,ia,aa,false,mode,clip,doclip);
}

void LICE_FillCircle(LICE_IBitmap* dest, float cx, float cy, float r, LICE_pixel color, float alpha, int mode, bool aa)
{
  if (!dest) return;
  if (dest->isFlipped()) cy=dest->getHeight()-1-cy;

  int ia = (int) (alpha*256.0f);
  if (!ia) return;
  int w = dest->getWidth(), h = dest->getHeight();
  int clip[4] = { 0, 0, w, h };

  bool doclip = (cx-r-1 < 0 || cy-r-1 < 0 || cx+r+1 >= w || cy+r+1 >= h);
  __DrawCircleClipped(dest,cx,cy,r,color,ia,aa,true,mode,clip,doclip);
}


void LICE_RoundRect(LICE_IBitmap *drawbm, float xpos, float ypos, float w, float h, int cornerradius,
                    LICE_pixel col, float alpha, int mode, bool aa)
{
  if (cornerradius>0)
  {
    float cr=cornerradius;
    if (cr > w*0.5) cr=w*0.5;
    if (cr > h*0.5) cr=h*0.5;
    cr=floor(cr);
    if (cr>=2)
    {
      LICE_Line(drawbm,xpos+cr,ypos,xpos+w-cr,ypos,col,alpha,mode,aa);
      LICE_Line(drawbm,xpos+cr-1,ypos+h,xpos+w-cr,ypos+h,col,alpha,mode,aa);
      LICE_Line(drawbm,xpos+w,ypos+cr,xpos+w,ypos+h-cr,col,alpha,mode,aa);
      LICE_Line(drawbm,xpos,ypos+cr-1,xpos,ypos+h-cr,col,alpha,mode,aa);


      LICE_Arc(drawbm,xpos+cr,ypos+cr,cr,-_PI*0.5f,0,col,alpha,mode,aa);
      LICE_Arc(drawbm,xpos+w-cr,ypos+cr,cr,0,_PI*0.5f,col,alpha,mode,aa);
      LICE_Arc(drawbm,xpos+w-cr,ypos+h-cr,cr,_PI*0.5f,_PI,col,alpha,mode,aa);
      LICE_Arc(drawbm,xpos+cr,ypos+h-cr,cr,_PI,_PI*1.5f,col,alpha,mode,aa);

      return;
    }
  }

  LICE_DrawRect(drawbm, xpos, ypos, w, h, col, alpha, mode);
}

