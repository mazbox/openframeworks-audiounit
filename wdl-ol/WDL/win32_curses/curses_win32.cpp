#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include "../swell/swell.h"
#endif
#define CURSES_INSTANCE ___ERRROR_____

#include "curses.h"

#include <ctype.h>
#include <stdio.h>

#define CURSOR_BLINK_TIMER_MS 400
#define CURSOR_BLINK_TIMER 2
#define CURSOR_BLINK_TIMER_ZEROEVERY 3

#ifndef WIN32_CURSES_CURSORTYPE
#define WIN32_CURSES_CURSORTYPE 1 // 1 for vertical bar, 2 for horz bar, 0 for block
#endif
#define WIN32CURSES_CLASS_NAME "WDLCursesWindow"

#define WIN32_CONSOLE_KBQUEUE

static void doFontCalc(win32CursesCtx*, HDC);

static void m_InvalidateArea(win32CursesCtx *ctx, int sx, int sy, int ex, int ey)
{
  doFontCalc(ctx,NULL);

  RECT r;
  r.left=sx*ctx->m_font_w;
  r.top=sy*ctx->m_font_h;
  r.right=ex*ctx->m_font_w;
  r.bottom=ey*ctx->m_font_h;
  if (ctx->m_hwnd) InvalidateRect(ctx->m_hwnd,&r,FALSE);
}

void __addnstr(win32CursesCtx *ctx, const char *str,int n)
{
  if (ctx->m_cursor_x<0)
  {
    int skip = -ctx->m_cursor_x;
    ctx->m_cursor_x=0;
    if (n>=0) 
    {
      n -= skip;
      if (n<0)n=0;
    }
    int slen = strlen(str);
    str += min(slen,skip);    
  }

  int sx=ctx->m_cursor_x;
  int sy=ctx->m_cursor_y;
  if (n==0||!ctx->m_framebuffer || ctx->m_cursor_y < 0 || ctx->m_cursor_y >= ctx->lines) return;
  char *p=(char *)ctx->m_framebuffer + 2*(ctx->m_cursor_x + ctx->m_cursor_y*ctx->cols);
  while (n-- && *str)
  {
    p[0]=*str++;
    p[1]=ctx->m_cur_attr;
    p+=2;
	  if (++ctx->m_cursor_x >= ctx->cols) 
	  { 
		  ctx->m_cursor_y++; 
		  ctx->m_cursor_x=0; 
		  if (ctx->m_cursor_y >= ctx->lines) { ctx->m_cursor_y=ctx->lines-1; ctx->m_cursor_x=ctx->cols-1; break; }
	  }
  }
  m_InvalidateArea(ctx,sx,sy,sy < ctx->m_cursor_y ? ctx->cols : ctx->m_cursor_x+1,ctx->m_cursor_y+1);
}

void __clrtoeol(win32CursesCtx *ctx)
{
  if (ctx->m_cursor_x<0)ctx->m_cursor_x=0;
  int n = ctx->cols - ctx->m_cursor_x;
  if (!ctx->m_framebuffer || ctx->m_cursor_y < 0 || ctx->m_cursor_y >= ctx->lines || n < 1) return;
  char *p=(char *)ctx->m_framebuffer + 2*(ctx->m_cursor_x + ctx->m_cursor_y*ctx->cols);
  int sx=ctx->m_cursor_x;
  while (n--)
  {
    p[0]=0;
    p[1]=ctx->m_cur_erase_attr;
    p+=2;
  }
  m_InvalidateArea(ctx,sx,ctx->m_cursor_y,ctx->cols,ctx->m_cursor_y+1);
}

void __curses_erase(win32CursesCtx *ctx)
{
  ctx->m_cur_attr=0;
  ctx->m_cur_erase_attr=0;
  if (ctx->m_framebuffer) memset(ctx->m_framebuffer,0,ctx->cols*ctx->lines*2);
  ctx->m_cursor_x=0;
  ctx->m_cursor_y=0;
  m_InvalidateArea(ctx,0,0,ctx->cols,ctx->lines);
}

void __move(win32CursesCtx *ctx, int x, int y, int noupdest)
{
  m_InvalidateArea(ctx,ctx->m_cursor_x,ctx->m_cursor_y,ctx->m_cursor_x+1,ctx->m_cursor_y+1);
  ctx->m_cursor_x=y;
  ctx->m_cursor_y=x;
  if (!noupdest) m_InvalidateArea(ctx,ctx->m_cursor_x,ctx->m_cursor_y,ctx->m_cursor_x+1,ctx->m_cursor_y+1);
}


void __init_pair(win32CursesCtx *ctx, int pair, int fcolor, int bcolor)
{
  if (pair < 0 || pair >= COLOR_PAIRS) return;

  pair=COLOR_PAIR(pair);

  ctx->colortab[pair][0]=fcolor;
  ctx->colortab[pair][1]=bcolor;

  if (fcolor & 0xff) fcolor|=0xff;
  if (fcolor & 0xff00) fcolor|=0xff00;
  if (fcolor & 0xff0000) fcolor|=0xff0000;
  ctx->colortab[pair|A_BOLD][0]=fcolor;
  ctx->colortab[pair|A_BOLD][1]=bcolor;

}

static int xlateKey(int msg, int wParam, int lParam)
{
  if (msg == WM_KEYDOWN)
  {
#ifndef _WIN32
    if (lParam & FVIRTKEY)
#endif
    switch (wParam)
	  {
	    case VK_HOME: return KEY_HOME;
	    case VK_UP: return KEY_UP;
	    case VK_PRIOR: return KEY_PPAGE;
	    case VK_LEFT: return KEY_LEFT;
	    case VK_RIGHT: return KEY_RIGHT;
	    case VK_END: return KEY_END;
	    case VK_DOWN: return KEY_DOWN;
	    case VK_NEXT: return KEY_NPAGE;
	    case VK_INSERT: return KEY_IC;
	    case VK_DELETE: return KEY_DC;
	    case VK_F1: return KEY_F1;
	    case VK_F2: return KEY_F2;
	    case VK_F3: return KEY_F3;
	    case VK_F4: return KEY_F4;
	    case VK_F5: return KEY_F5;
	    case VK_F6: return KEY_F6;
	    case VK_F7: return KEY_F7;
	    case VK_F8: return KEY_F8;
	    case VK_F9: return KEY_F9;
	    case VK_F10: return KEY_F10;
	    case VK_F11: return KEY_F11;
	    case VK_F12: return KEY_F12;
#ifndef _WIN32
      case VK_SUBTRACT: return (GetAsyncKeyState(VK_SHIFT)&0x8000)?'_':'-'; // numpad -
#endif
    }
    
    switch (wParam)
    {
      case VK_RETURN: case VK_BACK: case VK_TAB: case VK_ESCAPE: return wParam;
      case VK_CONTROL: break;
    
      default:
        if(GetAsyncKeyState(VK_CONTROL)&0x8000)
        {
          if (wParam>='a' && wParam<='z') 
          {
            wParam += 1-'a';
            return wParam;
          }
          if (wParam>='A' && wParam<='Z') 
          {
            wParam += 1-'A';
            return wParam;
          }
        }
    }
	}
    
#ifdef _WIN32 // todo : fix for nonwin32
  if (msg == WM_CHAR)
  {
    if(wParam>=32) return wParam;
  }  
#else
  
  //osx/linux
  if (wParam >= 32)
  {
    if (!(GetAsyncKeyState(VK_SHIFT)&0x8000))
    {
      if (wParam>='A' && wParam<='Z') 
      {
        if ((GetAsyncKeyState(VK_MENU)&0x8000)) wParam -= 'A'-1;
        else
          wParam += 'a'-'A';
      }
    }
    else
    {
      if (wParam=='-') wParam='_';
      else if (wParam>='0' && wParam<='9')
      {
        if (wParam=='0') wParam = ')';
        else if (wParam=='1') wParam = '!';
        else if (wParam=='2') wParam = '@';
        else if (wParam=='3') wParam = '#';
        else if (wParam=='4') wParam = '$';
        else if (wParam=='5') wParam = '%';
        else if (wParam=='6') wParam = '^';
        else if (wParam=='7') wParam = '&';
        else if (wParam=='8') wParam = '*';
        else if (wParam=='9') wParam = '(';
      }
    }
    return wParam;
  }
      
#endif
  return ERR;
}


static void m_reinit_framebuffer(win32CursesCtx *ctx)
{
  doFontCalc(ctx,NULL);
    RECT r;

    GetClientRect(ctx->m_hwnd,&r);
    
    ctx->lines=r.bottom / ctx->m_font_h;
    ctx->cols=r.right / ctx->m_font_w;
    if (ctx->lines<1) ctx->lines=1;
    if (ctx->cols<1) ctx->cols=1;
    ctx->m_cursor_x=0;
    ctx->m_cursor_y=0;
    ctx->m_framebuffer=(unsigned char *)realloc(ctx->m_framebuffer,2*ctx->lines*ctx->cols);
    if (ctx->m_framebuffer) memset(ctx->m_framebuffer,0,2*ctx->lines*ctx->cols);
}
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20A
#endif

LRESULT CALLBACK cursesWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
  win32CursesCtx *ctx = (win32CursesCtx*)GetWindowLongPtr(hwnd,GWLP_USERDATA);

#ifdef _WIN32

  static int Scroll_Message;
  if (!Scroll_Message)
  {
    Scroll_Message = (int)RegisterWindowMessage("MSWHEEL_ROLLMSG");
    if (!Scroll_Message) Scroll_Message=-1;
  }
  if (Scroll_Message > 0 && uMsg == (UINT)Scroll_Message)
  {
    uMsg=WM_MOUSEWHEEL;
    wParam<<=16; 
  }
#endif

  if (ctx)switch (uMsg)
  {
	case WM_DESTROY:
		ctx->m_hwnd=0;
	return 0;
#ifdef WIN32_CONSOLE_KBQUEUE
  case WM_CHAR: case WM_KEYDOWN: 

    {
      int a=xlateKey(uMsg,wParam,lParam);
      if (a != ERR && ctx->m_kbq)
      {
        ctx->m_kbq->Add(&a,sizeof(int));
      }
    }
  case WM_KEYUP:
  return 0;
#endif
	case WM_GETMINMAXINFO:
	      {
	        LPMINMAXINFO p=(LPMINMAXINFO)lParam;
	        p->ptMinTrackSize.x = 160;
	        p->ptMinTrackSize.y = 120;
	      }
	return 0;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			m_reinit_framebuffer(ctx);
			ctx->m_need_redraw=1;
		}
	return 0;
  case WM_RBUTTONDOWN:
  case WM_LBUTTONDOWN:
    SetFocus(hwnd);
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_CAPTURECHANGED:
  case WM_MOUSEMOVE:
  case WM_MOUSEWHEEL:
    if (ctx && ctx->onMouseMessage) return ctx->onMouseMessage(ctx->user_data,hwnd,uMsg,wParam,lParam);
  return 0;
#ifdef _WIN32
  case WM_GETDLGCODE:
    if (GetParent(hwnd))
    {
      return DLGC_WANTALLKEYS;
    }
  return 0;
#endif
  case WM_TIMER:
    if (wParam==CURSOR_BLINK_TIMER && ctx)
    {
      int la = ctx->m_cursor_state;
      ctx->m_cursor_state= (ctx->m_cursor_state+1)%CURSOR_BLINK_TIMER_ZEROEVERY;
      
      if (!!ctx->m_cursor_state != !!la)
        __move(ctx,ctx->m_cursor_y,ctx->m_cursor_x,1);// refresh cursor
    }
#ifdef WIN32_CONSOLE_KBQUEUE
    if (wParam == 1)
    {
      if (!ctx->m_intimer)
      {
        ctx->m_intimer=1;
        if (ctx->ui_run) ctx->ui_run(ctx);
        ctx->m_intimer=0;
      }
    }
#endif
  return 0;
    case WM_CREATE:

      // this only is called on osx or from standalone, it seems, since on win32 ctx isnt set up yet
      ctx->m_hwnd=hwnd;
      #ifdef WIN32_CONSOLE_KBQUEUE
         SetTimer(hwnd,1,33,NULL);
      #endif
      #ifndef _WIN32
	      m_reinit_framebuffer(ctx);
	      ctx->m_need_redraw=1;
      #endif
      SetTimer(hwnd,CURSOR_BLINK_TIMER,CURSOR_BLINK_TIMER_MS,NULL);
    return 0;
    case WM_ERASEBKGND:
    return 0;
    case WM_PAINT:
      {
        RECT r;
#ifdef _WIN32
        if (GetUpdateRect(hwnd,&r,FALSE))
#else
          GetClientRect(hwnd,&r);
#endif
        {
          PAINTSTRUCT ps;
          HDC hdc=BeginPaint(hwnd,&ps);
          if (hdc)
          {
            doFontCalc(ctx,ps.hdc);
            
            HGDIOBJ oldf=SelectObject(hdc,ctx->mOurFont);
            int y,ypos;
			      int lattr=-1;
#ifdef _WIN32
            SetTextAlign(hdc,TA_TOP|TA_LEFT);
#endif
            const char *ptr=(const char*)ctx->m_framebuffer;
            RECT updr=r;

			      r.left /= ctx->m_font_w;
			      r.top /= ctx->m_font_h;
			      r.bottom += ctx->m_font_h-1;
			      r.bottom /= ctx->m_font_h;
			      r.right += ctx->m_font_w-1;
			      r.right /= ctx->m_font_w;
    
			      ypos = r.top * ctx->m_font_h;
			      ptr += 2*(r.top * ctx->cols);

			      if (r.top < 0) r.top=0;
			      if (r.bottom > ctx->lines) r.bottom=ctx->lines;
			      if (r.left < 0) r.left=0;
			      if (r.right > ctx->cols) r.right=ctx->cols;

            HBRUSH bgbrushes[COLOR_PAIRS << NUM_ATTRBITS];
            for(y=0;y<sizeof(bgbrushes)/sizeof(bgbrushes[0]);y++) bgbrushes[y] = CreateSolidBrush(ctx->colortab[y][1]);

            int cstate=ctx->m_cursor_state;
            if (ctx->m_cursor_y != ctx->m_cursor_state_ly ||
                ctx->m_cursor_x != ctx->m_cursor_state_lx)
            {
              ctx->m_cursor_state_lx=ctx->m_cursor_x;
              ctx->m_cursor_state_ly=ctx->m_cursor_y;
              ctx->m_cursor_state=0;
              cstate=1;
            }
                    
            if (ctx->m_framebuffer) for (y = r.top; y < r.bottom; y ++, ypos+=ctx->m_font_h, ptr += ctx->cols*2)
            {
              int x = r.left,xpos = r.left * ctx->m_font_w;

				      const char *p = ptr + r.left*2;

              int defer_blanks=0;

              for (;; x ++, xpos+=ctx->m_font_w, p += 2)
              {
                char c=' ',attr=0; 
                
                if (x < r.right)
                {
                  c=p[0];
                  attr=p[1];
                }

                bool isCursor = cstate && y == ctx->m_cursor_y && x == ctx->m_cursor_x;
                bool isNotBlank = (isprint(c) && !isspace(c));

                if (defer_blanks > 0 && (isNotBlank || isCursor || attr != lattr || x>=r.right))
                {
                  RECT tr={xpos - defer_blanks*ctx->m_font_w,ypos,xpos,ypos+ctx->m_font_h};
                  FillRect(hdc,&tr,bgbrushes[lattr&((COLOR_PAIRS << NUM_ATTRBITS)-1)]);
                  defer_blanks=0;
                }

                if (x>=r.right) break;

#if WIN32_CURSES_CURSORTYPE == 0
						    if (isCursor)
						    {
						      SetTextColor(hdc,ctx->colortab[attr&((COLOR_PAIRS << NUM_ATTRBITS)-1)][1]);
						      SetBkColor(hdc,ctx->colortab[attr&((COLOR_PAIRS << NUM_ATTRBITS)-1)][0]);
                  lattr = -1;
						    }
				        else 
#endif // WIN32_CURSES_CURSORTYPE == 0
                if (attr != lattr)
				        {
						      SetTextColor(hdc,ctx->colortab[attr&((COLOR_PAIRS << NUM_ATTRBITS)-1)][0]);
						      SetBkColor(hdc,ctx->colortab[attr&((COLOR_PAIRS << NUM_ATTRBITS)-1)][1]);
					        lattr=attr;
				        }

                if (isNotBlank||isCursor)
                {
                  #ifdef _WIN32
                    int txpos = xpos;
                    TextOut(hdc,txpos,ypos,isNotBlank ? p : " ",1);
                  #else
                    RECT tr={xpos,ypos,xpos+32,ypos+32};
                    HBRUSH br=bgbrushes[attr&((COLOR_PAIRS << NUM_ATTRBITS)-1)];
#if WIN32_CURSES_CURSORTYPE == 0
                    if (isCursor)
                    {
                      br = CreateSolidBrush(ctx->colortab[attr&((COLOR_PAIRS << NUM_ATTRBITS)-1)][0]);
                      FillRect(hdc,&tr,br);
                      DeleteObject(br);
                    }
                    else
#endif
                    {
                      FillRect(hdc,&tr,br);
                    }
                    char tmp[2]={c,0};
                    DrawText(hdc,isprint(c) && !isspace(c) ?tmp : " ",-1,&tr,DT_LEFT|DT_TOP|DT_NOPREFIX|DT_NOCLIP);
                  #endif
#if WIN32_CURSES_CURSORTYPE > 0
                  if (isCursor)
                  {
                    #if WIN32_CURSES_CURSORTYPE == 1
                      RECT r={xpos,ypos,xpos+2,ypos+ctx->m_font_h};
                    #elif WIN32_CURSES_CURSORTYPE == 2
                      RECT r={xpos,ypos+ctx->m_font_h-2,xpos+ctx->m_font_w,ypos+ctx->m_font_h};
                    #endif
                    HBRUSH br=CreateSolidBrush(ctx->colortab[attr&((COLOR_PAIRS << NUM_ATTRBITS)-1)][0]);
                    FillRect(hdc,&r,br);
                    DeleteObject(br);
                  }
#endif
                }
                else 
                {
                  defer_blanks++;
                }
              }
            }
            int rm=ctx->cols * ctx->m_font_w;
            int bm=ctx->lines * ctx->m_font_h;
            if (updr.right >= rm)
            {
              RECT tr={max(rm,updr.left),max(updr.top,0),updr.right,updr.bottom};
              FillRect(hdc,&tr,bgbrushes[0]);
            }
            if (updr.bottom >= bm)
            {
              RECT tr={max(0,updr.left),max(updr.top,bm),updr.right,updr.bottom};
              FillRect(hdc,&tr,bgbrushes[0]);
            }

            for(y=0;y<sizeof(bgbrushes)/sizeof(bgbrushes[0]);y++) DeleteObject(bgbrushes[y]);
            SelectObject(hdc,oldf);

            EndPaint(hwnd,&ps);
          }
        }
      }
    return 0;
  }
  return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

static void doFontCalc(win32CursesCtx *ctx, HDC hdcIn)
{
  if (!ctx || !ctx->m_hwnd || !ctx->m_need_fontcalc) return;

  HDC hdc = hdcIn;
  if (!hdc) hdc = GetDC(ctx->m_hwnd);

  if (!hdc) return;
   
  ctx->m_need_fontcalc=false;
  HGDIOBJ oldf=SelectObject(hdc,ctx->mOurFont);
  TEXTMETRIC tm;
  GetTextMetrics(hdc,&tm);
  ctx->m_font_h=tm.tmHeight;
  ctx->m_font_w=tm.tmAveCharWidth;
  SelectObject(hdc,oldf);
  
  if (hdc != hdcIn) ReleaseDC(ctx->m_hwnd,hdc);

}

static void reInitializeContext(win32CursesCtx *ctx)
{
  if (!ctx->mOurFont) ctx->mOurFont = CreateFont(
#ifdef _WIN32
                                                 16,
#else
#ifdef __LP64__
                                                14,
#else
                                                 13,
#endif
#endif
                        0, // width
                        0, // escapement
                        0, // orientation
#ifdef _WIN32
                        FW_NORMAL, // normal
#else
                        FW_BOLD,
#endif
                        FALSE, //italic
                        FALSE, //undelrine
                        FALSE, //strikeout
                        ANSI_CHARSET,
                        OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS,
                        NONANTIALIASED_QUALITY,//DEFAULT_QUALITY,
#ifdef _WIN32
                        FF_MODERN,
#else
                                                 0,
#endif
                        "Courier New");

  ctx->m_need_fontcalc=true;
  ctx->m_font_w=8;
  ctx->m_font_h=8;
  doFontCalc(ctx,NULL);
}




void __initscr(win32CursesCtx *ctx)
{
  __init_pair(ctx,0,RGB(192,192,192),RGB(0,0,0));
}

void __endwin(win32CursesCtx *ctx)
{
  if (ctx->m_hwnd)
    curses_setWindowContext(ctx->m_hwnd,0);
  ctx->m_hwnd=0;
  free(ctx->m_framebuffer);
  ctx->m_framebuffer=0;
  delete ctx->m_kbq;
  ctx->m_kbq=0;
  if (ctx->mOurFont) DeleteObject(ctx->mOurFont);
  ctx->mOurFont=0;

}


int curses_getch(win32CursesCtx *ctx)
{
  if (!ctx->m_hwnd) return ERR;

 
#ifndef WIN32_CONSOLE_KBQUEUE
  // if we're suppose to run the message pump ourselves (optional!)
  MSG msg;
  while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
  {
    TranslateMessage(&msg);
    int a=xlateKey(msg.message,msg.wParam,msg.lParam);
    if (a != ERR) return a;

    DispatchMessage(&msg);

  }
#else

#ifdef _WIN32
  if (ctx->want_getch_runmsgpump>0)
  {
    MSG msg;
    if (ctx->want_getch_runmsgpump>1)
    {
      while(!(ctx->m_kbq && ctx->m_kbq->Available()>=(int)sizeof(int)) && GetMessage(&msg,NULL,0,0))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    else while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
#endif

  if (ctx->m_kbq && ctx->m_kbq->Available()>=(int)sizeof(int))
  {
    int a=*(int *) ctx->m_kbq->Get();
    ctx->m_kbq->Advance(sizeof(int));
    ctx->m_kbq->Compact();
    return a;
  }
#endif
  
  if (ctx->m_need_redraw)
  {
    ctx->m_need_redraw=0;
    InvalidateRect(ctx->m_hwnd,NULL,FALSE);
    return 'L'-'A'+1;
  }

  return ERR;
}

void curses_setWindowContext(HWND hwnd, win32CursesCtx *ctx)
{
  SetWindowLongPtr(hwnd,GWLP_USERDATA,(INT_PTR)ctx);
  if (ctx)
  {
    ctx->m_hwnd=hwnd;
    delete ctx->m_kbq;
    ctx->m_kbq=new WDL_Queue;

    free(ctx->m_framebuffer);
    ctx->m_framebuffer=0;

    SetTimer(hwnd,CURSOR_BLINK_TIMER,CURSOR_BLINK_TIMER_MS,NULL);
    reInitializeContext(ctx);
    m_reinit_framebuffer(ctx);
    InvalidateRect(hwnd,NULL,FALSE);
  }
}

static int m_regcnt;
void curses_unregisterChildClass(HINSTANCE hInstance)
{
#ifdef _WIN32
  if (!--m_regcnt)
    UnregisterClass(WIN32CURSES_CLASS_NAME,hInstance);
#endif
}

void curses_registerChildClass(HINSTANCE hInstance)
{
#ifdef _WIN32
  if (!m_regcnt++)
  {
	  WNDCLASS wc={0,};	
	  wc.lpfnWndProc = cursesWindowProc;
    wc.hInstance = hInstance;	
	  wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	  wc.lpszClassName = WIN32CURSES_CLASS_NAME;

    RegisterClass(&wc);
  }
#endif
}

#ifndef _WIN32
HWND curses_ControlCreator(HWND parent, const char *cname, int idx, const char *classname, int style, int x, int y, int w, int h)
{
  HWND hw=0;
  if (!strcmp(classname,WIN32CURSES_CLASS_NAME))
  {
    hw=CreateDialog(NULL,0,parent,(DLGPROC)cursesWindowProc);
  }
  
  if (hw)
  {
    SetWindowLong(hw,GWL_ID,idx);
    SetWindowPos(hw,HWND_TOP,x,y,w,h,SWP_NOZORDER|SWP_NOACTIVATE);
    ShowWindow(hw,SW_SHOWNA);
    return hw;
  }
  
  return 0;
}

#endif

HWND curses_CreateWindow(HINSTANCE hInstance, win32CursesCtx *ctx, const char *title)
{
#ifdef _WIN32
 ctx->m_hwnd = CreateWindowEx(0,WIN32CURSES_CLASS_NAME, title,WS_CAPTION|WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_SIZEBOX|WS_SYSMENU,
					CW_USEDEFAULT,CW_USEDEFAULT,640,480,
					NULL, NULL,hInstance,NULL);
#else
  ctx->m_hwnd = CreateDialog(NULL,0,NULL,(DLGPROC)cursesWindowProc);
  
#endif
 if (ctx->m_hwnd) 
 {
   curses_setWindowContext(ctx->m_hwnd,ctx);
   ShowWindow(ctx->m_hwnd,SW_SHOW);
 }
 return ctx->m_hwnd;
}
