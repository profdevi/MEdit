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


//v2.0 copyright Comine.com  20170328T1345
#include <windows.h>
#include "MStdLib.h"
#include "MBuffer.h"
#include "MClipBoard.h"


//**********************************************************
//** MClipBoard Code
//**********************************************************
void MClipBoard::ClearObject(void)
	{
	mhWndOwner=NULL;
	}

	
/////////////////////////////////////////////////////////////
MClipBoard::MClipBoard(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MClipBoard::MClipBoard(HWND howner)
	{
	mhWndOwner=NULL;
	if(Create(howner)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MClipBoard::~MClipBoard(void) {  Destroy();  }


////////////////////////////////////////////////
bool MClipBoard::Create(HWND howner)
	{
	Destroy();
	if(howner==NULL)
		{
		Destroy();  return false;
		}

	mhWndOwner=howner;  

	return true;
	}


////////////////////////////////////////////////
bool MClipBoard::Destroy(void)
	{
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MClipBoard::CopyText(const char *text)
	{
	if(OpenClipboard(mhWndOwner)==FALSE)
		{
		return false;
		}

	// Empty the clipboard before saving
	EmptyClipboard();

	HGLOBAL hmemory=GlobalAlloc(GMEM_MOVEABLE,MStdStrLen(text)+1);
	if(hmemory==NULL)
		{
		CloseClipboard();  return false;
		}

	char *fixedmemory=(char *)GlobalLock(hmemory);
	if(fixedmemory==NULL)
		{
		GlobalFree(hmemory);  CloseClipboard();  return false;
		}

	MStdStrCpy(fixedmemory,text);

	GlobalUnlock(hmemory); 

	SetClipboardData(CF_TEXT,hmemory);

	CloseClipboard();  return true;
	}


////////////////////////////////////////////////
bool MClipBoard::GetText(MBuffer &text)
	{
	if(OpenClipboard(mhWndOwner)==FALSE)
		{
		return false;
		}

	if(IsClipboardFormatAvailable(CF_TEXT)==FALSE)
		{  CloseClipboard();  return false;  }

	// CopyClipboard Data
	HGLOBAL hglobal=GetClipboardData(CF_TEXT);
	if(hglobal==NULL)
		{
		CloseClipboard();  return false;
		}


	const char *clipdata=(const char *)GlobalLock(hglobal);
	if(clipdata==NULL)
		{
		CloseClipboard();  return false;
		}

	if(text.Create(MStdStrLen(clipdata)+1)==false)
		{
		GlobalUnlock(hglobal); CloseClipboard();  return false;
		}
	
	if(text.SetString(clipdata)==false)
		{
		GlobalUnlock(hglobal); CloseClipboard();  return false;
		}

	GlobalUnlock(hglobal); CloseClipboard();
	return true;
	}


//////////////////////////////////////////////////////////
bool MClipBoard::HasText(void)						// Check if clipboard has text
	{
	if(OpenClipboard(mhWndOwner)==FALSE)
		{
		return false;
		}

	if(IsClipboardFormatAvailable(CF_TEXT)==FALSE)
		{  CloseClipboard();  return false;  }
	
	CloseClipboard();  return true;
	}


//////////////////////////////////////////////////////
bool MClipBoard::Clear(void)
	{
	if(OpenClipboard(mhWndOwner)==FALSE)
		{
		return false;
		}

	if(EmptyClipboard()==FALSE)
		{  CloseClipboard();  return false;  }
	
	CloseClipboard();  return true;
	}



