/*
Copyright (C) 2011-2023, Comine.com ( profdevi@ymail.com )
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- The the names of the contributors of this project may not be used to 
  endorse or promote products derived from this software without specific 
  prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
`AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


//v1.20 copyright Comine.com 20190626W1723
#undef UNICODE
#include <windows.h>
#include <math.h>
#include "MStdLib.h"
#include "MWinGDI.h"


// Link in Libraries
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"user32.lib")


//*********************************************************
//*    MWinGDIFont
//*********************************************************
void MWinGDIFont::ClearObject(void)
	{
	mhFont=NULL;
	}


/////////////////////////////////////////////////////////////
MWinGDIFont::MWinGDIFont(void)
	{  ClearObject();  }


/////////////////////////////////////////////////////////////
MWinGDIFont::MWinGDIFont(int fontsize,const char *typefacename,int weight,bool italic,bool underline
		,bool strikethrough,int angle)
	{
	ClearObject();
	bool ret=Create(fontsize,typefacename,weight,italic,underline,strikethrough,angle);
	if(ret==false)
		{
		return;
		}
	}


/////////////////////////////////////////////////////////////
MWinGDIFont::~MWinGDIFont(void)
	{  Destroy();  }


/////////////////////////////////////////////////////////////
bool MWinGDIFont::Create(int fontsize,const char *typefacename,int weight
		,bool italic,bool underline,bool strikethrough,int angle)
	{
	Destroy();
	DWORD dwstrike;
	if(strikethrough==true) { dwstrike=TRUE; } else { dwstrike=FALSE; }
	
	DWORD dwitalics;
	if(italic==true) { dwitalics=TRUE; }  else { dwitalics=FALSE; }

	DWORD dwunderline;
	if(underline==true) { dwunderline=TRUE; } else { dwunderline=FALSE; }

	mhFont=CreateFontA(fontsize,0,angle,0,weight,dwitalics,dwunderline,dwstrike
			,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS
			,DEFAULT_QUALITY,DEFAULT_PITCH,typefacename);
	if(mhFont==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////
bool MWinGDIFont::Create(const LOGFONTA *loginfo)
	{
	Destroy();
	if(loginfo==NULL)
		{
		return false;
		}

	mhFont=CreateFontIndirectA(loginfo);
	if(mhFont==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIFont::Destroy(void)
	{
	if(mhFont!=NULL)
		{  DeleteObject(mhFont);  }

	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////////
HFONT MWinGDIFont::GetFont(void)
	{
	return mhFont;
	}


//*********************************************************
//*    MWinGDISolidBrush
//*********************************************************
void MWinGDISolidBrush::ClearObject(void)
	{
	mhBrush=NULL;
	}

////////////////////////////////////////////////////////////
MWinGDISolidBrush::MWinGDISolidBrush(void)
	{  ClearObject();  }


////////////////////////////////////////////////////////////
MWinGDISolidBrush::MWinGDISolidBrush(int red,int green,int blue)
	{
	ClearObject();
	if(Create(red,green,blue)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////////////////
MWinGDISolidBrush::MWinGDISolidBrush(COLORREF color)
	{
	ClearObject();
	
	if(Create(color)==false)
		{
		return; 
		}

	return;
	}


////////////////////////////////////////////////////////////
MWinGDISolidBrush::~MWinGDISolidBrush(void)
	{  Destroy(); }


////////////////////////////////////////////////////////////
bool MWinGDISolidBrush::Create(int red,int green,int blue)
	{
	Destroy();
	mhBrush=CreateSolidBrush(RGB(red,green,blue));
	if(mhBrush==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MWinGDISolidBrush::Create(COLORREF color)
	{
	Destroy();
	mhBrush=CreateSolidBrush(color);
	if(mhBrush==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MWinGDISolidBrush::Destroy(void)
	{
	if(mhBrush!=NULL)
		{  DeleteObject(mhBrush); }

	ClearObject();
	return true;
	}


/////////////////////////////////////////////////////////////
HBRUSH MWinGDISolidBrush::GetBrush(void)
	{
	return mhBrush;
	}


//*********************************************************
//*    MWinGDIPen
//*********************************************************
void MWinGDIPen::ClearObject(void)
	{
	mhPen=NULL;  
	}


/////////////////////////////////////////////////////////////
MWinGDIPen::MWinGDIPen(void)
	{	ClearObject();  }


/////////////////////////////////////////////////////////////
MWinGDIPen::MWinGDIPen(int width,COLORREF color,int penstyle)
	{
	ClearObject();
	if(Create(width,color,penstyle)==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////////////////
MWinGDIPen::~MWinGDIPen(void)
	{  Destroy();  }


/////////////////////////////////////////////////////////////
bool MWinGDIPen::Create(int width,COLORREF color,int penstyle)
	{
	Destroy();
	mhPen=CreatePen(penstyle,width,color);
	if(mhPen==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIPen::Destroy(void)
	{
	if(mhPen!=NULL)
		{ DeleteObject(mhPen); }

	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////////
HPEN MWinGDIPen::GetPen(void)
	{
	return mhPen;
	}


//*********************************************************
//*    MWinGDIBitmap
//*********************************************************
void MWinGDIBitmap::ClearObject(void)
	{
	mhBitmap=NULL;
	MStdMemSet(&mInfo,0,sizeof(mInfo));
	}


///////////////////////////////////////////////////////////////
MWinGDIBitmap::MWinGDIBitmap(void)
	{  ClearObject();  }


//////////////////////////////////////////////////////////
MWinGDIBitmap::~MWinGDIBitmap(void)
	{  Destroy();  }


//////////////////////////////////////////////////////////
bool MWinGDIBitmap::Create(const char *filename)
	{
	Destroy();
	mhBitmap=(HBITMAP)LoadImageA(NULL,filename,IMAGE_BITMAP
				,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if(mhBitmap==NULL)
		{
		Destroy();  return false;
		}

	
	if(GetObjectA(mhBitmap,sizeof(mInfo),&mInfo)!=sizeof(mInfo))
		{
		Destroy();  return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////
bool MWinGDIBitmap::Create(int ResID,HINSTANCE hInst)
	{
	Destroy();
	if(hInst==0) { hInst=GetModuleHandle(NULL); }

	mhBitmap=(HBITMAP)LoadImageA(hInst,MAKEINTRESOURCE(ResID),IMAGE_BITMAP,0,0
			,LR_CREATEDIBSECTION);
	if(mhBitmap==NULL)
		{
		Destroy();  return false;
		}

	if(GetObjectA(mhBitmap,sizeof(mInfo),&mInfo)!=sizeof(mInfo))
		{
		Destroy(); return false;
		}

	return true;	
	}


//////////////////////////////////////////////////////////
bool MWinGDIBitmap::Create(HDC hdc,int width,int height)
	{
	Destroy();
	if(hdc==NULL)
		{
		Destroy();  return false;
		}

	// Create 
	mhBitmap=CreateCompatibleBitmap(hdc,width,height);
	if(mhBitmap==NULL)
		{
		Destroy(); return false;
		}

	return true;
	}

//////////////////////////////////////////////////////////
bool MWinGDIBitmap::Destroy(void)
	{
	if(mhBitmap!=NULL) {  DeleteObject(mhBitmap);  }
	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////
int MWinGDIBitmap::GetWidth(void)
	{
	return mInfo.bmWidth;
	}


//////////////////////////////////////////////////////////
int MWinGDIBitmap::GetHeight(void)
	{
	return mInfo.bmHeight;
	}


//////////////////////////////////////////////////////////
int MWinGDIBitmap::GetWidthBytes(void)
	{
	return mInfo.bmWidthBytes;
	}


//////////////////////////////////////////////////////////
int MWinGDIBitmap::GetPlanes(void)
	{
	return mInfo.bmPlanes;
	}


//////////////////////////////////////////////////////////
int MWinGDIBitmap::GetBPP(void) // Bits per Pixel
	{
	return mInfo.bmBitsPixel;
	}


char *MWinGDIBitmap::GetBits(void)
	{
	return (char *)mInfo.bmBits;
	}


//////////////////////////////////////////////////////////
HBITMAP MWinGDIBitmap::GetHandle(void)
	{
	return mhBitmap;
	}


//////////////////////////////////////////////////////////
bool MWinGDIBitmap::GetDIBColorTable(HDC destdc,int startindex,int count,RGBQUAD *table)
	{
	HDC hdc=CreateCompatibleDC(destdc);
	if(hdc==NULL)
		{
		return false;
		}

	HGDIOBJ oldmap=SelectObject(hdc,mhBitmap);

	if(::GetDIBColorTable(hdc,startindex,count,table)==0)
		{
		}

	SelectObject(hdc,oldmap);  DeleteDC(hdc); return true;
	}


//*********************************************************
//*    MWinGDIBaseRefDC
//*********************************************************
void MWinGDIBaseRefDC::ClearObject(void)
	{
	mhDC=NULL;
	mhOldFont=NULL;
	mhOldBrush=NULL;
	mhOldBitMap=NULL;
	mhOldPen=NULL;
	}


/////////////////////////////////////////////////////////////
MWinGDIBaseRefDC::MWinGDIBaseRefDC(void)
	{  ClearObject();  }


/////////////////////////////////////////////////////////////
MWinGDIBaseRefDC::~MWinGDIBaseRefDC(void)
	{  Destroy();  }


////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::Create(HDC hdc)
	{
	Destroy();

	// Set the HDC
	mhDC=hdc;

	return true;
	}


////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::Destroy(void)
	{
	if(mhOldFont!=NULL)
		{
		::SelectObject(mhDC,mhOldFont);
		mhOldFont=NULL;
		}

	if(mhOldBrush!=NULL)
		{
		::SelectObject(mhDC,mhOldBrush);
		mhOldBrush=NULL;
		}

	if(mhOldBitMap!=NULL)
		{
		::SelectObject(mhDC,mhOldBitMap);
		mhOldBitMap=NULL;
		}

	if(mhOldPen!=NULL)
		{
		::SelectObject(mhDC,mhOldPen);
		mhOldPen=NULL;
		}

	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(MWinGDIFont &font)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldFont==NULL)
		{
		mhOldFont=(HFONT)::SelectObject(mhDC,font.GetFont() );
		return true;
		}

	::SelectObject(mhDC,font.GetFont() );
	return true;
	}


//////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(HFONT hfont)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldFont==NULL)
		{
		mhOldFont=(HFONT)::SelectObject(mhDC,hfont );
		return true;
		}

	::SelectObject(mhDC,hfont );  return true;	
	}


////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(MWinGDISolidBrush &brush)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldBrush==NULL)
		{
		mhOldBrush=(HBRUSH)::SelectObject(mhDC,brush.GetBrush() );
		return true;
		}

	::SelectObject(mhDC,brush.GetBrush() );  return true;
	}


////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(HBRUSH hbrush)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldBrush==NULL)
		{
		mhOldBrush=(HBRUSH)::SelectObject(mhDC,hbrush );
		return true;
		}

	::SelectObject(mhDC,hbrush);  return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(MWinGDIPen &pen)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldPen==NULL)
		{
		mhOldPen=(HPEN)::SelectObject(mhDC,pen.GetPen() );
		return true;
		}

	::SelectObject(mhDC,pen.GetPen() );  return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(HPEN hpen)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldPen==NULL)
		{
		mhOldPen=(HPEN)::SelectObject(mhDC,hpen);
		return true;
		}

	::SelectObject(mhDC,hpen);  return true;
	}



/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(MWinGDIBitmap &hbitmap)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldBitMap==NULL)
		{
		mhOldBitMap=(HBITMAP)::SelectObject(mhDC,hbitmap.GetHandle());
		return true;
		}

	::SelectObject(mhDC,hbitmap.GetHandle());  return true;	
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::SelectObject(HBITMAP hbmp)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(mhOldBitMap==NULL)
		{
		mhOldBitMap=(HBITMAP)::SelectObject(mhDC,hbmp);
		return true;
		}

	::SelectObject(mhDC,hbmp);
	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::DeselectFont(void)
	{
	if(mhOldFont==NULL)
		{ 
		return true;
		}

	::SelectObject(mhDC,mhOldFont);

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::DeselectPen(void)
	{
	if(mhOldPen==NULL)
		{  return true;  }

	::SelectObject(mhDC,mhOldPen);

	return true;	
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::DeselectBrush(void)
	{
	if(mhOldBrush==NULL)
		{  return true;  }

	::SelectObject(mhDC,mhOldBrush);

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::DeselectBitmap(void)
	{
	if(mhOldBitMap==NULL)
		{  return true;  }

	::SelectObject(mhDC,mhOldBitMap);

	return true;
	}


//////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::DeselectAll(void)
	{
	DeselectBitmap();
	DeselectBrush();
	DeselectFont();
	DeselectPen();
	return true;
	}



/////////////////////////////////////////////////////////////
HDC MWinGDIBaseRefDC::GetDC(void)
	{
	return mhDC;
	}


/////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::SetTextColor(COLORREF color)
	{
	if(mhDC==NULL)
		{
		return;
		}

	::SetTextColor(mhDC,color);
	}


/////////////////////////////////////////////////////////////
COLORREF MWinGDIBaseRefDC::GetTextColor(void)
	{
	if(mhDC==NULL)
		{
		return RGB(0,0,0);
		}

	return ::GetTextColor(mhDC);
	}


/////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::SetBkMode(int mode)
	{
	if(mhDC==NULL)
		{
		return;
		}

	::SetBkMode(mhDC,mode);
	}


/////////////////////////////////////////////////////////////
int MWinGDIBaseRefDC::GetBkMode(void)
	{
	if(mhDC==NULL)
		{
		return OPAQUE;
		}

	return ::GetBkMode(mhDC);
	}


/////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::SetBkColor(COLORREF color)
	{
	if(mhDC==NULL)
		{
		return;
		}
	
	::SetBkColor(mhDC,color);
	}


/////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::TextOut(int x,int y,const char *str)
	{
	if(mhDC==NULL)
		{
		return;
		}

	::TextOutA(mhDC,x,y,str,MStdStrLen(str));
	}


/////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::TextOut(int x,int y,const char *str,int length)
	{
	if(mhDC==NULL)
		{
		return;
		}

	::TextOutA(mhDC,x,y,str,length);
	}


////////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::DrawText(int x,int y,int width,int height
		,const char *str,int format)
	{
	if(mhDC==NULL)
		{
		return;
		}

	RECT rect={x,y,x+width,y+height};
	::DrawTextA(mhDC,str,-1,&rect,format);
	}


/////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::MoveTo(int x,int y)
	{
	if(mhDC==NULL)
		{
		return;
		}

	::MoveToEx(mhDC,x,y,NULL);
	}


/////////////////////////////////////////////////////////////
void MWinGDIBaseRefDC::LineTo(int x,int y)
	{
	if(mhDC==NULL)
		{
		return;
		}

	::LineTo(mhDC,x,y);
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::Rectangle(int x,int y,int width,int height)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	::Rectangle(mhDC,x,y,x+width,y+height);
	return true;	
	}


////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::FillRect(const RECT *rect,HBRUSH brush)
	{
	if(mhDC==NULL)
		{
		return false;
		}

	if(::FillRect(mhDC,rect,brush)==0) { return false; }

	return true;
	}


////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::FillRect(int x,int y,int width,int height,HBRUSH brush )
	{
	if(mhDC==NULL)
		{
		return false;
		}

	RECT rect={x,y,x+width,y+height};

	if(::FillRect(mhDC,&rect,brush)==0) { return false; }

	return true;
	}


////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::GetTextMetrics(LPTEXTMETRICA ptm)
	{
	if(mhDC==NULL)
		{
		return false;		
		}

	if(::GetTextMetricsA(mhDC,ptm)==FALSE)
		{ 
		return false;
		};

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::BitBlt(int x,int y,int width,int height
		,HDC hdc,int srcx,int srcy,DWORD dwrops)
	{
	if(mhDC==NULL)
		{
		return false;		
		}

	::BitBlt(mhDC,x,y,width,height,hdc,srcx,srcy,dwrops);

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::BitBlt(int x,int y,int width,int height
		,MWinGDIBaseRefDC &srcdc,int srcx,int srcy,DWORD dwrops)
	{
	if(mhDC==NULL)
		{
		return false;		
		}

	::BitBlt(mhDC,x,y,width,height,srcdc.GetDC(),srcx,srcy,dwrops);	

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::StretchBlt(int x,int y,int width,int height,HDC hdc,int srcx,int srcy
		,int srcwidth,int srcheight,DWORD dwrop)
	{
	if(mhDC==NULL)
		{
		return false;		
		}

	::StretchBlt(mhDC,x,y,width,height,hdc,srcx,srcy,srcwidth,srcheight,dwrop);

	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::StretchBlt(int x,int y,int width,int height
		,MWinGDIBaseRefDC &hdc,int srcx,int srcy
		,int srcwidth,int srcheight,DWORD dwrop)
	{
	if(mhDC==NULL)
		{
		return false;		
		}

	::StretchBlt(mhDC,x,y,width,height,hdc.GetDC(),srcx,srcy,srcwidth,srcheight,dwrop);

	return true;
	}


//////////////////////////////////////////////////////////////
bool MWinGDIBaseRefDC::DrawArrow(int xhead,int yhead,int xtail,int ytail,int arrowheadlength)
	{
	MStdAssert(mhDC!=NULL);

	// Draw a line
	MoveTo(xhead,yhead);
	LineTo(xtail,ytail);

	if(xtail==xhead && yhead==ytail)
		{  return true;  }

	int dx=xhead-xtail;
	int dy=yhead-ytail;

	if(dx*dx+dy*dy<arrowheadlength*arrowheadlength)
		{
		//=Arrow Head is too big
		if(dx>0)
			{  dx=+1; }
		else if(dx<0)
			{  dx=-1; }
		else	// wasted assignment of zero
			{  dx=0;  }
		
		if(dy>0)
			{  dy=+1; }
		else if(dy<0)
			{  dy=-1; }
		else	// wasted assignment of zero
			{  dy=0;  }

		MoveTo(xhead,yhead);
		LineTo(xhead+dx,yhead-dy);

		MoveTo(xhead,yhead);
		LineTo(xhead-dx,yhead+dy);

		return true;
		}
	
	//                    P1
	//                    |
	// Find Two Points    T---->H
	//                    |
	//                    P2
	int x1=xtail+dy;
	int y1=ytail-dx;
	int x2=xtail-dy;
	int y2=ytail+dx;

	//MoveTo(xhead,yhead);
	//LineTo(x1,y1);

	//MoveTo(xhead,yhead);
	//LineTo(x2,y2);

	float distance=(float)sqrt((float)(dy-dx)*(dy-dx)+(dy+dx)*(dy+dx) );
	if(distance<=1) { return true; }

	float n1dx=float(x1-xhead)*arrowheadlength/distance;
	float n1dy=float(y1-yhead)*arrowheadlength/distance;

	float n2dx=float(x2-xhead)*arrowheadlength/distance;
	float n2dy=float(y2-yhead)*arrowheadlength/distance;

	MoveTo(xhead,yhead);
	LineTo(int(xhead+n1dx),int(yhead+n1dy) );

	MoveTo(xhead,yhead);
	LineTo(int(xhead+n2dx),int(yhead+n2dy) );

	return true;
	}



//*********************************************************
//*    MWinGDIWrapDC
//*********************************************************
void MWinGDIWrapDC::ClearObject(void)
	{
	}


/////////////////////////////////////////////////////////////
MWinGDIWrapDC::MWinGDIWrapDC(void)
	{ ClearObject();  }


/////////////////////////////////////////////////////////////
MWinGDIWrapDC::MWinGDIWrapDC(HDC hdc)
	{
	ClearObject();
	if(Create(hdc)==false)
		{
		return;
		}
	}

////////////////////////////////////////////////////////////
MWinGDIWrapDC::~MWinGDIWrapDC(void)
	{  Destroy();  }


////////////////////////////////////////////////////////////
bool MWinGDIWrapDC::Create(HDC hdc)
	{
	Destroy();
	if(MWinGDIBaseRefDC::Create(hdc)==false)
		{
		return false;
		}
	return true;
	}


////////////////////////////////////////////////////////////
bool MWinGDIWrapDC::Destroy(void)
	{
	MWinGDIBaseRefDC::Destroy();
	ClearObject();
	return true;
	}


//*********************************************************
//*    MWinGDIPaintDC
//*********************************************************
void MWinGDIPaintDC::ClearObject(void)
	{
	mhWnd=NULL;
	MStdMemSet(&mPS,0,sizeof(mPS) );
	}


//////////////////////////////////////////////////////////////
MWinGDIPaintDC::MWinGDIPaintDC(void)
	{  ClearObject();  }


/////////////////////////////////////////////////////////////
MWinGDIPaintDC::MWinGDIPaintDC(HWND hWnd) // For Fast Construction
	{
	ClearObject();
	if(Create(hWnd)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////////////////
MWinGDIPaintDC::~MWinGDIPaintDC(void)
	{  Destroy();  }


///////////////////////////////////////////////////////////
bool MWinGDIPaintDC::Create(HWND hWnd) // Create Paint Context
	{
	if(hWnd==NULL)
		{
		return false;
		}

	Destroy();
	mhWnd=hWnd;
	HDC tempdc=BeginPaint(mhWnd,&mPS);

	if(tempdc==NULL)
		{
		mhWnd=NULL;
		Destroy();  return false;
		}
	
	if(MWinGDIBaseRefDC::Create(tempdc)==false)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////////////////
bool MWinGDIPaintDC::Destroy(void)
	{
	if(MWinGDIBaseRefDC::GetDC()==NULL) { return true; }
	MWinGDIBaseRefDC::Destroy();
	EndPaint(mhWnd,&mPS);
	ClearObject();
	return true;
	}


//***************************************************
//		MWinGDIClientDC
//***************************************************
void MWinGDIClientDC::ClearObject(void)
	{
	mhWnd=NULL;  	
	}


///////////////////////////////////////////////////////////////
MWinGDIClientDC::MWinGDIClientDC(void)
	{  ClearObject();  }


//////////////////////////////////////////////////////////////
MWinGDIClientDC::MWinGDIClientDC(HWND hWnd)
	{
	ClearObject();
	if(Create(hWnd)==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////////////////
MWinGDIClientDC::~MWinGDIClientDC(void)
	{  Destroy(); }


//////////////////////////////////////////////////////////////
bool MWinGDIClientDC::Create(HWND hWnd)
	{
//	Commented out to accept handle to desktopscreen	
//	if(hWnd==NULL)
//		{
//		MTrace("Unable to create DC for NULL Window");
//		return false;
//		}

	Destroy();

	HDC tempdc=::GetDC(hWnd);
	if(tempdc==NULL)
		{
		return false;
		}

	mhWnd=hWnd;
	
	if(MWinGDIBaseRefDC::Create(tempdc)==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////
bool MWinGDIClientDC::Destroy(void)
	{
	HDC tempdc=MWinGDIBaseRefDC::GetDC();
	MWinGDIBaseRefDC::Destroy();
	if(tempdc!=NULL){  ReleaseDC(mhWnd,tempdc);  }
	ClearObject();  return true;
	}


//***************************************************
//		MWinGDIWindowDC
//***************************************************
void MWinGDIWindowDC::ClearObject(void)
	{
	mhWnd=NULL;
	}

///////////////////////////////////////////////////////////////
MWinGDIWindowDC::MWinGDIWindowDC(void)
	{  ClearObject();  }


//////////////////////////////////////////////////////////////
MWinGDIWindowDC::MWinGDIWindowDC(HWND hWnd)
	{
	ClearObject();
	if(Create(hWnd)==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////////////////
MWinGDIWindowDC::~MWinGDIWindowDC(void)
	{  Destroy(); }


//////////////////////////////////////////////////////////////
bool MWinGDIWindowDC::Create(HWND hWnd)
	{
	if(hWnd==NULL)
		{
		return false;
		}

	Destroy();

	HDC tempdc=::GetWindowDC(hWnd);
	if(tempdc==NULL)
		{
		Destroy();  return false;
		}

	mhWnd=hWnd;
	
	if(MWinGDIBaseRefDC::Create(tempdc)==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////
bool MWinGDIWindowDC::Destroy(void)
	{
	if(mhWnd==NULL) { return true; }
	HDC tempdc=MWinGDIBaseRefDC::GetDC();
	MWinGDIBaseRefDC::Destroy();
	if(tempdc!=NULL) {  ReleaseDC(mhWnd,tempdc);  }
	ClearObject();  return true;
	}


//***************************************************
//		MWinGDICreateDC
//***************************************************
void MWinGDICreateDC::ClearObject(void)
	{
	}


//////////////////////////////////////////////////////////////
MWinGDICreateDC::MWinGDICreateDC(void)
	{  ClearObject();  }


//////////////////////////////////////////////////////////////
MWinGDICreateDC::MWinGDICreateDC(HDC hdc)  // A Compatible DC
	{
	ClearObject();
	if(Create(hdc)==false)
		{
		return; 
		}
	}


//////////////////////////////////////////////////////////////
MWinGDICreateDC::~MWinGDICreateDC(void)
	{  Destroy();  }


//////////////////////////////////////////////////////////////
bool MWinGDICreateDC::Create(HDC hdc)  // Create a compatible DC
	{
	Destroy();
	HDC compdc=CreateCompatibleDC(hdc);
	if(compdc==NULL)
		{
		return false;
		}

	if(MWinGDIBaseRefDC::Create(compdc)==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////
bool MWinGDICreateDC::Destroy(void)
	{
	HDC tempdc=MWinGDIBaseRefDC::GetDC();
	if(tempdc==NULL)
		{
		return false;
		}

	MWinGDIBaseRefDC::Destroy();
	DeleteDC(tempdc);
	return true;
	}


