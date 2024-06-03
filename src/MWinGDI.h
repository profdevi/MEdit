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
#ifndef MWinGDI_h
#define MWinGDI_h

/////////////////////////////////////////////////////
#undef UNICODE
#include <windows.h>

//////////////////////////////////////////////////////
// Solved Naming Problems 
#ifdef DrawText
#undef DrawText
#endif // DrawText

#ifdef TextOut
#undef TextOut
#endif // TextOut

#ifdef GetTextMetrics
#undef GetTextMetrics
#endif // GetTextMetrics


////////////////////////////////////////////////////
class MWinGDIFont
	{
	HFONT mhFont;

	/////////////////////////////////////////////////
	void ClearObject(void);

	/////////////////////////////////////////////////
	public:
	MWinGDIFont(void);
	MWinGDIFont(int fontsize,const char *typefacename="FixedSys"
			,int weight=400,bool italics=false
			,bool underline=false,bool strikethrough=false,int angle=0);
	
	~MWinGDIFont(void);
	bool Create(int fontsize=0,const char *typefacename="FixedSys"
			,int weight=400,bool italics=false
			,bool underline=false,bool strikethrough=false,int angle=0);
	bool Create(const LOGFONTA *loginfo);
	bool Destroy(void);
	HFONT GetFont(void);
	};


////////////////////////////////////////////////////
class MWinGDISolidBrush
	{
	HBRUSH mhBrush;

	////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////
	public:
	MWinGDISolidBrush(void);
	MWinGDISolidBrush(int red,int green,int blue);
	MWinGDISolidBrush(COLORREF color);
	~MWinGDISolidBrush(void);
	bool Create(int red,int green,int blue);
	bool Create(COLORREF color);
	bool Destroy(void);	
	HBRUSH GetBrush(void);
	};


////////////////////////////////////////////////////
class MWinGDIPen
	{
	HPEN mhPen;

	////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////
	public:
	MWinGDIPen(void);
	MWinGDIPen(int width,COLORREF color,int penstyle=PS_SOLID);
	~MWinGDIPen(void);
	bool Create(int width=1,COLORREF color=RGB(100,20,30),int penstyle=PS_SOLID);
	bool Destroy(void);
	HPEN GetPen(void);
	};


////////////////////////////////////////////////////
class MWinGDIBitmap
	{
	HBITMAP mhBitmap;  BITMAP mInfo;

	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWinGDIBitmap(void);
	~MWinGDIBitmap(void);
	bool Create(const char *filename);
	bool Create(int ResID,HINSTANCE hInst=0);
	bool Create(HDC hdc,int width,int height);  // Create compatible bitmap
	bool Destroy(void);
	int GetWidth(void);
	int GetHeight(void);
	int GetWidthBytes(void);
	int GetPlanes(void);
	int GetBPP(void); // Bits per Pixel
	char *GetBits(void);
	HBITMAP GetHandle(void);
	bool GetDIBColorTable(HDC hdc,int startindex,int count,RGBQUAD *table);
	};


////////////////////////////////////////////////////
class MWinGDIBaseRefDC
	{
	HDC mhDC;
	HFONT mhOldFont;
	HBRUSH mhOldBrush;
	HBITMAP mhOldBitMap;
	HPEN mhOldPen;

	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	protected:
	MWinGDIBaseRefDC(void);
	~MWinGDIBaseRefDC(void);	
	bool Create(HDC hdc);
	bool Destroy(void);
	
	////////////////////////////////////////////////
	public:
	bool SelectObject(MWinGDIFont &font);
	bool SelectObject(HFONT hfont);
	bool SelectObject(MWinGDISolidBrush &brush);
	bool SelectObject(HBRUSH hbrush);
	bool SelectObject(MWinGDIPen &pen);
	bool SelectObject(HPEN hpen);
	bool SelectObject(MWinGDIBitmap &hbitmap);
	bool SelectObject(HBITMAP hbitmap);
	bool DeselectFont(void);
	bool DeselectPen(void);
	bool DeselectBrush(void);
	bool DeselectBitmap(void);
	bool DeselectAll(void);
	HDC GetDC(void);
	void SetTextColor(COLORREF color);
	COLORREF GetTextColor(void);
	void SetBkMode(int mode=TRANSPARENT);
	int GetBkMode(void);
	void SetBkColor(COLORREF color);
	void TextOut(int x,int y,const char *str);
	void TextOut(int x,int y,const char *str,int length);
	void DrawText(int x,int y,int width,int height,const char *str
			,int format=DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	void MoveTo(int x,int y);
	void LineTo(int x,int y);
	bool Rectangle(int x,int y,int width,int height);
	bool FillRect(const RECT *rect,HBRUSH brush=(HBRUSH)(COLOR_WINDOW+1));
	bool FillRect(int x,int y,int width,int height,HBRUSH brush=(HBRUSH)(COLOR_WINDOW+1));
	bool GetTextMetrics(LPTEXTMETRICA ptm);
	bool BitBlt(int x,int y,int width,int height,HDC hdc,int srcx=0,int srcy=0
			,DWORD dwrops=SRCCOPY);
	bool BitBlt(int x,int y,int width,int height,MWinGDIBaseRefDC &dc
			,int srcx=0,int srcy=0,DWORD dwrops=SRCCOPY);
	bool StretchBlt(int x,int y,int width,int height,HDC hdc,int srcx,int srcy
			,int srcwidth,int srcheight,DWORD dwrop=SRCCOPY);
	bool StretchBlt(int x,int y,int width,int height,MWinGDIBaseRefDC &dc
			,int srcx,int srcy,int srcwidth,int srcheight,DWORD dwrop=SRCCOPY);
	bool DrawArrow(int xhead,int yhead,int xtail,int ytail,int arrowheadlen=5);
	};


////////////////////////////////////////////////////
class MWinGDIWrapDC:public MWinGDIBaseRefDC
	{

	///////////////////////////////////////////////
	void ClearObject(void);

	///////////////////////////////////////////////
	public:
	MWinGDIWrapDC(void);
	MWinGDIWrapDC(HDC hdc);	 // For Quick Construction
	~MWinGDIWrapDC(void);
	bool Create(HDC hdc);
	bool Destroy(void);
	};


////////////////////////////////////////////////////
class MWinGDIPaintDC:public MWinGDIBaseRefDC
	{
	HWND mhWnd;  PAINTSTRUCT mPS;

	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWinGDIPaintDC(void);
	MWinGDIPaintDC(HWND hWnd); // For Fast Construction
	~MWinGDIPaintDC(void);
	bool Create(HWND hWnd); // Create Paint Context
	bool Destroy(void);
	};


///////////////////////////////////////////////////
class MWinGDIClientDC:public MWinGDIBaseRefDC
	{
	HWND mhWnd;

	///////////////////////////////////////////////
	void ClearObject(void);

	///////////////////////////////////////////////
	public:
	MWinGDIClientDC(void);
	MWinGDIClientDC(HWND hWnd);
	~MWinGDIClientDC(void);
	bool Create(HWND hWnd);
	bool Destroy(void);
	};


///////////////////////////////////////////////////
class MWinGDIWindowDC:public MWinGDIBaseRefDC
	{
	HWND mhWnd;

	///////////////////////////////////////////////
	void ClearObject(void);

	///////////////////////////////////////////////
	public:
	MWinGDIWindowDC(void);
	MWinGDIWindowDC(HWND hWnd);
	~MWinGDIWindowDC(void);
	bool Create(HWND hWnd);
	bool Destroy(void);
	};


///////////////////////////////////////////////////
class MWinGDICreateDC:public MWinGDIBaseRefDC
	{
	///////////////////////////////////////////////
	void ClearObject(void);

	///////////////////////////////////////////////
	public:
	MWinGDICreateDC(void);
	MWinGDICreateDC(HDC hdc);  // A Compatible DC
	~MWinGDICreateDC(void);
	bool Create(HDC hdc);  // Create a compatible DC
	bool Destroy(void);
	};


#endif // MWinGDI_h