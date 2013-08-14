/*
    WDL - wdlstring.h
    Copyright (C) 2005 and later, Cockos Incorporated
  
    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
      
*/

/*

  This file provides a simple class for variable-length string manipulation.
  It provides only the simplest features, and does not do anything confusing like
  operator overloading. It uses a WDL_HeapBuf for internal storage.

  You can do Set, Get, Append, Insert, and SetLen.. that's about it
  
*/

#ifndef _WDL_STRING_H_
#define _WDL_STRING_H_

#include "heapbuf.h"
#include <stdio.h>
#include <stdarg.h>

#ifndef WDL_STRING_IMPL_ONLY
class WDL_String
{
public:
  explicit WDL_String(int hbgran) : m_hb(hbgran WDL_HEAPBUF_TRACEPARM("WDL_String(4)"))
  {
  }
  explicit WDL_String(const char *initial=NULL, int initial_len=0) : m_hb(128 WDL_HEAPBUF_TRACEPARM("WDL_String"))
  {
    if (initial) Set(initial,initial_len);
  }
  WDL_String(const WDL_String &s) : m_hb(128 WDL_HEAPBUF_TRACEPARM("WDL_String(2)"))
  {
    Set(&s);
  }
  WDL_String(const WDL_String *s) : m_hb(128 WDL_HEAPBUF_TRACEPARM("WDL_String(3)"))
  {
    if (s && s != this) Set(s);
  }
  ~WDL_String()
  {
  }
#define WDL_STRING_PREFIX 
#else
#define WDL_STRING_PREFIX WDL_String::
#endif

  void WDL_STRING_PREFIX Set(const char *str, int maxlen
#ifdef WDL_STRING_INTF_ONLY
      =0); 
#else
#ifdef WDL_STRING_IMPL_ONLY
    )
#else
    =0)
#endif
  {
    int s=0;
    if (maxlen>0) while (s < maxlen && str[s]) s++;
    else s=(int)strlen(str);   
    __doSet(0,str,s,0);
  }
#endif

  void WDL_STRING_PREFIX Set(const WDL_String *str, int maxlen
#ifdef WDL_STRING_INTF_ONLY
      =0); 
#else
#ifdef WDL_STRING_IMPL_ONLY
    )
#else
    =0)
#endif
  {
#ifdef WDL_STRING_FASTSUB_DEFINED
    int s = str->GetLength();
    if (maxlen>0 && maxlen<s) s=maxlen;

    __doSet(0,str->Get(),s,0);
#else
    Set(str->Get(), maxlen); // might be faster: "partial" strlen
#endif
  }
#endif

  void WDL_STRING_PREFIX Append(const char *str, int maxlen
#ifdef WDL_STRING_INTF_ONLY
      =0); 
#else
#ifdef WDL_STRING_IMPL_ONLY
    )
#else
    =0)
#endif
  {
    int s=0;
    if (maxlen>0) while (s < maxlen && str[s]) s++;
    else s=(int)strlen(str);

    __doSet(GetLength(),str,s,0);
  }
#endif

  void WDL_STRING_PREFIX Append(const WDL_String *str, int maxlen
#ifdef WDL_STRING_INTF_ONLY
      =0); 
#else
#ifdef WDL_STRING_IMPL_ONLY
    )
#else
    =0)
#endif
  {
#ifdef WDL_STRING_FASTSUB_DEFINED
    int s = str->GetLength();
    if (maxlen>0 && maxlen<s) s=maxlen;

    __doSet(GetLength(),str->Get(),s,0);
#else
    Append(str->Get(), maxlen); // might be faster: "partial" strlen
#endif
  }
#endif

  void WDL_STRING_PREFIX DeleteSub(int position, int len)
#ifdef WDL_STRING_INTF_ONLY
    ;
#else
    {
	  char *p=(char *)m_hb.Get();
	  if (!m_hb.GetSize() || !*p) return;
	  int l=m_hb.GetSize()-1;
	  if (position < 0 || position >= l) return;
	  if (position+len > l) len=l-position;
    if (len>0)
    {
  	  memmove(p+position,p+position+len,l-position-len+1);
      m_hb.Resize(l+1-len,false);
    }
  }
#endif

  void WDL_STRING_PREFIX Insert(const char *str, int position, int maxlen
#ifdef WDL_STRING_INTF_ONLY
      =0); 
#else
#ifdef WDL_STRING_IMPL_ONLY
    )
#else
    =0)
#endif
  {
    int ilen=0;
    if (maxlen>0) while (ilen < maxlen && str[ilen]) ilen++;
    else ilen=(int)strlen(str);

    int srclen = GetLength();
    if (position<0) position=0;
    else if (position>srclen) position=srclen;
    if (ilen>0) __doSet(position,str,ilen,srclen-position);
  }
#endif

  void WDL_STRING_PREFIX Insert(const WDL_String *str, int position, int maxlen
#ifdef WDL_STRING_INTF_ONLY
      =0); 
#else
#ifdef WDL_STRING_IMPL_ONLY
    )
#else
    =0)
#endif
  {
#ifdef WDL_STRING_FASTSUB_DEFINED
    int ilen = str->GetLength();
    if (maxlen>0 && maxlen<ilen) ilen=maxlen;

    int srclen = m_hb.GetSize()>0 ? m_hb.GetSize()-1 : 0;
    if (position<0) position=0;
    else if (position>srclen) position=srclen;
    if (ilen>0) __doSet(position,str->Get(),ilen,srclen-position);
#else
    Insert(str->Get(), position, maxlen); // might be faster: "partial" strlen
#endif
  }
#endif

  void WDL_STRING_PREFIX SetLen(int length, bool resizeDown
#ifdef WDL_STRING_INTF_ONLY
      =false); 
#else
#ifdef WDL_STRING_IMPL_ONLY
    )
#else
    =false)
#endif
  {                       
#ifdef WDL_STRING_FASTSUB_DEFINED
    int osz = m_hb.GetSize()?m_hb.GetSize()-1:0;
#endif
    char *b=(char*)m_hb.Resize(length+1,resizeDown);
    if (m_hb.GetSize()==length+1) 
    {
#ifdef WDL_STRING_FASTSUB_DEFINED
      if (length > osz) memset(b+osz,' ',length-osz);
#endif
      b[length]=0;
    }
  }
#endif
  void WDL_STRING_PREFIX SetAppendFormattedArgs(bool append, int maxlen, const char* fmt, va_list arglist) 
#ifdef WDL_STRING_INTF_ONLY
    ; 
#else
  {
    int offs = append ? GetLength() : 0;
    char* b= (char*) m_hb.Resize(offs+maxlen+1,false)+offs;
    if (m_hb.GetSize() != offs+maxlen+1) return;

    #ifdef _WIN32
		  int written = _vsnprintf(b, maxlen+1, fmt, arglist);
      if (written < 0 || written>=maxlen) b[written=b[0]?maxlen:0]=0;
    #else
		  int written = vsnprintf(b, maxlen+1, fmt, arglist);
      if (written > maxlen) written=maxlen;
    #endif

    m_hb.Resize(offs + written + 1,false);
	}
#endif


  void WDL_VARARG_WARN(printf,3,4) WDL_STRING_PREFIX SetFormatted(int maxlen, const char *fmt, ...) 
#ifdef WDL_STRING_INTF_ONLY
    ; 
#else
  {
    va_list arglist;
    va_start(arglist, fmt);
    SetAppendFormattedArgs(false,maxlen,fmt,arglist);
    va_end(arglist);
  }
#endif

  void WDL_VARARG_WARN(printf,3,4) WDL_STRING_PREFIX AppendFormatted(int maxlen, const char* fmt, ...) 
#ifdef WDL_STRING_INTF_ONLY
    ; 
#else
  {
    va_list arglist;
    va_start(arglist, fmt);
    SetAppendFormattedArgs(true,maxlen,fmt,arglist);
    va_end(arglist);
  }
#endif


  void WDL_STRING_PREFIX Ellipsize(int minlen, int maxlen)
#ifdef WDL_STRING_INTF_ONLY
    ;
#else
  {
    if (maxlen >= 4 && m_hb.GetSize() && GetLength() > maxlen) 
    {
      if (minlen<0) minlen=0;
      char *b = (char *)m_hb.Get();
      int i;
      for (i = maxlen-4; i >= minlen; --i) 
      {
        if (b[i] == ' ') 
        {
          memcpy(b+i, "...",4);
          m_hb.Resize(i+4,false);
          break;
        }
      }
      if (i < minlen && maxlen >= 4) 
      {
        memcpy(b+maxlen-4, "...",4);    
        m_hb.Resize(maxlen,false);
      }
    }
  }
#endif

#ifndef WDL_STRING_IMPL_ONLY
    #ifdef WDL_STRING_FASTSUB_DEFINED
      const char *Get() const { return m_hb.GetSize()?(char*)m_hb.Get():""; }
      int GetLength() const { int a = m_hb.GetSize(); return a>0?a-1:0; }
    #else
      char *Get() const
      {
        if (m_hb.GetSize()) return (char *)m_hb.Get();
        static char c; c=0; return &c; // don't return "", in case it gets written to.
      }
      int GetLength() const { return m_hb.GetSize()?(int)strlen((const char*)m_hb.Get()):0; }
    #endif

    private:

#endif

    void WDL_STRING_PREFIX __doSet(int offs, const char *str, int len, int trailkeep)
    #ifdef WDL_STRING_INTF_ONLY
        ; 
    #else
    {   
      if (len>0 || (!trailkeep && !offs && m_hb.GetSize()>1)) // if non-empty, or (empty and allocated and Set() rather than append/insert), then allow update, otherwise do nothing
      {
        char *newbuf=(char*)m_hb.Resize(offs+len+trailkeep+1,false);
        if (m_hb.GetSize()==offs+len+trailkeep+1) 
        {
          if (trailkeep>0) memmove(newbuf+offs+len,newbuf+offs,trailkeep);
          memcpy(newbuf+offs,str,len);
          newbuf[offs+len+trailkeep]=0;
        }
      }
    }
    #endif

#ifndef WDL_STRING_IMPL_ONLY

    WDL_HeapBuf m_hb;
};
#endif

#ifndef WDL_STRING_FASTSUB_DEFINED
#undef _WDL_STRING_H_
#define WDL_STRING_FASTSUB_DEFINED
#define WDL_String WDL_FastString
#include "wdlstring.h"
#undef WDL_STRING_FASTSUB_DEFINED
#undef WDL_String
#endif

#endif

