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


//v0.15 copyright Comine.com 20240603M1017
#include <windows.h>
#include "MStdLib.h"
#include "MWinGDI.h"
#include "MEditBuffer.h"
#include "MWindowTracker.h"
#include "MEditBufferParser.h"
#include "MColorPalette.h"
#include "MEditUndoRedo.h"
#include "MClipBoard.h"
#include "MStringBuffer.h"
#include "MBlockOps.h"
#include "MDynamicString.h"
#include "MStringList.h"
#include "MEmptyLexer.h"
#include "MWinEditor.h"


//******************************************************
//**  Module Elements
//******************************************************
static const char *GWindowClassName="MWinEditor";			// Class Name of window
static const char *GObjectRefProp="ObjectPointer";			// Name of property for window object
static const char *GTabFillChar=" ";						// Default Space character
static MEditBufferElement GFillElement={' ',0x00 };			// Default Element for fill space
static const int GDefaultCursorWidth=2;						// Default Cursor Width

/////////////////////////////////////////////////////////
// Default colors for text order is as in MEditBufferParserType enumeration
static COLORREF GTextColorSet[]=
	{
	RGB(0,0,0),						// Unset Text Color
	RGB(0,0,0),						// Regular text
	RGB(100,100,250),				// Operators
	RGB(100,0,100),					// Double Quoted strings
	RGB(100,0,0),					// Single Quoted Strings
	RGB(0,170,0),					// C like Comment /* */
	RGB(0,0,250),					// KeyWords
	RGB(255,255,255),				// Blank Spaces
	RGB(255,0,0),					// Error
	RGB(0,170,100),					// C++ Like Comment //
	};


/////////////////////////////////////////////////////////
// Default colors for text order is as in MEditBufferParserType enumeration
static COLORREF GBackColorSet[]=
	{
	RGB(255,255,255),				// Normal Back Color
	RGB(180,180,180),				// Blocking Color
	RGB(240,170,170)				// Marking Color
	};


///////////////////////////////////////////////////////////////
static bool GIsDisplayable(char ch)  // Checks if ch is displayable
	{
	if(ch>=32 && ch<128) { return true; }
	if(ch=='\t' || ch=='\n') { return true; }
	return false;
	}


///////////////////////////////////////////////////////////////
int GGetLineCount(const char *line)
	{
	int count;
	count=1;

	int i;
	for(i=0;line[i];++i)
		{
		if(line[i]=='\n')
			{
			// Increment count
			count=count+1;
			}
		}

	return count;
	}


//******************************************************
//**  ClearObject
//******************************************************
void MWinEditor::ClearObject(void)
	{
	mhMainWindow=NULL;	
	mRefLexer=NULL;
	mReadOnly=false;
	mModified=false;
	}


//******************************************************
//**  Window Call Back Functions
//******************************************************
LRESULT CALLBACK MWinEditor::WindowProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
	{
	MWinEditor *objref=(MWinEditor*)GetProp(hwnd,GObjectRefProp);
	if(objref==NULL)
		{ return DefWindowProc(hwnd,msg,wparam,lparam); }

	// Object Event Handler
	if(msg==WM_CHAR) {  objref->EventChar(wparam); return 0; }
	else if(msg==WM_PAINT) { objref->EventPaint();  return 0;  }
	else if(msg==WM_SIZE) { objref->EventSize();  return 0; }
	else if(msg==WM_CLOSE) {  objref->EventClose();  return 0; }
	else if(msg==WM_SETFOCUS) {  objref->EventSetFocus();  return 0;  }
	else if(msg==WM_KILLFOCUS) {  objref->EventKillFocus();  return 0;  }
	else if(msg==WM_KEYDOWN) {  objref->EventKeyDown(wparam);  return 0;  }
	else if(msg==WM_LBUTTONDOWN) { objref->EventLButtonDown();  return 0; }
	else if(msg==WM_MOUSEMOVE) { objref->EventMouseMove();  return 0;  }
	else if(msg==WM_LBUTTONUP) { objref->EventLButtonUp();  return 0; }

	return DefWindowProc(hwnd,msg,wparam,lparam);
	}


//*******************************************************************
//*** Object Window Event Handlers
//*******************************************************************
bool MWinEditor::EventChar(WPARAM keycode)				// Handle WM_CHAR
	{
	// if escape stop blocking
	if(keycode==27)
		{
		mBlockOps.BlockStart(0,0);
		mBlockOps.BlockStop(0,0);
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		return true;
		}

	// Non Printable Character
	if(keycode<32 && keycode!='\t' && keycode!='\b') { return true; }

	if(mReadOnly==true)
		{
		// Read Only means dont update
		return false;
		}

	if(mBlockOps.IsBlocked()==true)
		{
		DeleteBlock();
		if(keycode=='\b') { return true; }
		}

	if(keycode=='\b') { return BackSpace();  }
	return InsertChar((char)keycode);
	}


////////////////////////////////////////////////
bool MWinEditor::EventClose(void)
	{  PostQuitMessage(0);  return true;  }


////////////////////////////////////////////////
bool MWinEditor::EventPaint(void)
	{
	MWinGDIPaintDC paintdc(mhMainWindow);
	paintdc.SelectObject(mFont);
	paintdc.SelectObject(mBrush);
	paintdc.SetTextColor(RGB(0,0,200) );
	RedrawScreen(paintdc.GetDC());
	paintdc.Destroy();
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::EventSize(void)
	{
	RECT clientrect;
	if(GetClientRect(mhMainWindow,&clientrect)==FALSE) { return false; }

	// Update Screen Width/Height
	mWindowTracker.SetScreenSize(clientrect.right-clientrect.left
			,clientrect.bottom-clientrect.top);

	InvalidateRect(mhMainWindow,NULL,TRUE);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::EventSetFocus(void)				// Handle WM_SETFOCUS
	{
	CreateCaret(mhMainWindow,NULL,GDefaultCursorWidth,mWindowTracker.GetFontCharHeight() );
	ShowCaret(mhMainWindow);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::EventKillFocus(void)				// Handle WM_KILLFOCUS
	{
	HideCaret(mhMainWindow);
	return true;
	}


///////////////////////////////////////////////
bool MWinEditor::EventKeyDown(WPARAM keycode)					// Handle WM_KEYDOWN
	{
	// Check if Control Key is being held down
	if(GetKeyState(VK_CONTROL)<0)
		{
		//=Control Key has been pressed
		if(keycode==VK_UP)
			{
			//** Move Cursor to Up by one line
			return MoveUpScreenLine();
			}
		else if(keycode==VK_DOWN)
			{
			//** Move Cursor to Down by one line
			return MoveDownScreenLine();
			}
		else if(keycode==VK_LEFT)
			{
			//** Move Cursor to Left by one
			return MoveCaretPrevWord();
			}	
		else if(keycode==VK_RIGHT)
			{
			//** Move Cursor to Right by one
			return MoveCaretNextWord();
			}
		else if(keycode==VK_HOME)
			{
			//** Move Cursor to doc Start
			return MoveCaretDocStart();
			}
		else if(keycode==VK_END)
			{
			//** Move Cursor to doc End
			return MoveCaretDocEnd();
			}
		else if(keycode=='Z')
			{
			//** Undo Operation
			// If ReadOnly, do not paste
			if(mReadOnly==true)
				{
				// Read Only means dont update
				return false;
				}

			return UndoLastOperation();
			}
		else if(keycode=='Y')
			{
			//** Redo Last Undo
			// If ReadOnly, do not paste
			if(mReadOnly==true)
				{
				// Read Only means dont update
				return false;
				}

			return RedoLastOperation();
			}
		else if(keycode=='M')
			{
			//**Color Selected Text
			if(mBlockOps.IsBlocked()==true)
				{
				int startline,startoffset;
				int endline,endoffset;
				if(mBlockOps.GetBlockedRegion(startline,startoffset,endline,endoffset)==false)
					{
					return false;
					}

				ColorBlock(startline,startoffset,endline,endoffset-1);

				// Turn Off the blocking
				mBlockOps.BlockStart(0,0);
				mBlockOps.BlockStop(0,0);

				// Update Screen
				UpdateScreenSilently();
				return true;
				}

			return true;
			}
		else if(keycode=='K')
			{
			//** Clear all marked regions
			mBlockOps.BlockStart(0,0);
			mBlockOps.BlockStop(0,0);

			// Set to normal text color
			ColorBlock(0);

			// UpdateScreen
			UpdateScreenSilently();

			return true;
			}
		else if(keycode=='C')
			{
			// Copy selected text to clipboard
			Beep(2000,50);
			Beep(1000,50);
			return CopyTextToClipBoard();
			}
		else if(keycode=='V')
			{
			// If ReadOnly, do not paste
			if(mReadOnly==true)
				{
				// Read Only means dont update
				return false;
				}

			if(mBlockOps.IsBlocked()==true)
				{
				// Delete block first before paste
				DeleteBlock();
				}

			return PasteClipBoardText();
			}
		else if(keycode=='A')
			{
			//* Block all the text
			int linecount=mBuffer.GetRows();

			// If No Lines exit
			if(linecount==0)
				{ return false;  }

			int linecols=mBuffer.GetCols(linecount-1);

			mBlockOps.BlockStart(0,0);
			mBlockOps.BlockStop(linecount-1,linecols);

			UpdateScreenSilently();
			return true;
			}

		else if(keycode=='P')
			{
			// Parse Whole File Again
			mRefLexer->ParseAll();
			UpdateScreenSilently();
			return true;
			}
		
		return true;
		}

	//= Control Key is not held down
	if(keycode==VK_UP)
		{ return MoveCaretUp();  }
	else if(keycode==VK_DOWN)
		{ return MoveCaretDown();  }
	else if(keycode==VK_LEFT)
		{  return MoveCaretLeft();  }
	else if(keycode==VK_RIGHT)
		{  return MoveCaretRight();  }
	else if(keycode==VK_HOME)
		{  return MoveCaretLineStart();  }
	else if(keycode==VK_END)
		{  return MoveCaretLineEnd();  }
	else if(keycode==VK_PRIOR)
		{  return MoveCaretLastPage();  }
	else if(keycode==VK_NEXT)
		{  return MoveCaretNextPage();  } 
	else if(keycode==VK_DELETE)
		{
		// If ReadOnly, do not paste
		if(mReadOnly==true)
			{
			// Read Only means dont update
			return false;
			}

		if(mBlockOps.IsBlocked()==true)
			{ return DeleteBlock();  }

		return DeleteChar();
		}
	else if(keycode==VK_RETURN)
		{
		// If ReadOnly, do not paste
		if(mReadOnly==true)
			{
			// Read Only means dont update
			return false;
			}

		return PressEnter();
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::EventLButtonDown(void)				// Handle WM_LBUTTONDOWN
	{
	POINT pt;
	if(GetCursorPos(&pt)==TRUE && ScreenToClient(mhMainWindow,&pt)==TRUE)
		{
		mWindowTracker.SetCursor(pt.y,pt.x);
		mBlockOps.BlockStart(mWindowTracker.GetEditLine(),mWindowTracker.GetEditOffset());
		SetCapture(mhMainWindow);  // Start Mouse Capture
		if(mWindowTracker.IsScreenUpdated()==true)
			{ 
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}
		else if(mBlockOps.IsBlocked()==false && mBlockOps.IsBlockStopped()==true)
			{
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}

		int col=mWindowTracker.GetCaretCol();
		int row=mWindowTracker.GetCaretRow();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::EventMouseMove(void)				// Handle WM_MOUSEMOVE
	{
	if(GetCapture()!=mhMainWindow) { return true; }

	//=We have the mouse capture

	// Get Mouse Position
	POINT pt;
	GetCursorPos(&pt);  ScreenToClient(mhMainWindow,&pt);
	mWindowTracker.SetCursor(pt.y,pt.x);
	mBlockOps.BlockStop(mWindowTracker.GetEditLine(),mWindowTracker.GetEditOffset() );

	HideCaret(mhMainWindow);
	UpdateScreenSilently();
	ShowCaret(mhMainWindow);
	mWindowTracker.ResetScreenUpdated();		

	int col=mWindowTracker.GetCaretCol();
	int row=mWindowTracker.GetCaretRow();
	SetCaretPos(mWindowTracker.GetFontCharWidth()*col
				,mWindowTracker.GetFontCharHeight()*row);
	
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::EventLButtonUp(void)				// Handle WM_LBUTTONDOWN
	{
	ReleaseCapture();  // Release the Capture
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::DrawChar(HDC hdc,int row,int col,MEditBufferElement *element,bool blocked)
	{
	if(blocked==true)
		{
		//Render a blocked cell
		if(element==NULL)
			{
			COLORREF color;
			
			// Set Back Color
			color=mBackPalette.Get(1);
			SetBkColor(hdc,color);

			::TextOutA(hdc,col*mWindowTracker.GetFontCharWidth()
				,row*mWindowTracker.GetFontCharHeight(),GTabFillChar,1);
			}
		else
			{
			COLORREF color;
			
			// Set Back Color
			color=mBackPalette.Get(1);
			SetBkColor(hdc,color);

			// Set Text Color
			color=mTextPalette.Get(element->GetTextType() );
			SetTextColor(hdc,color );

			// If Tab write a space
			if(element->Value=='\t')
				{
				TextOutA(hdc,col*mWindowTracker.GetFontCharWidth()
					,row*mWindowTracker.GetFontCharHeight(),GTabFillChar,1);
				}
			else
				{
				TextOutA(hdc,col*mWindowTracker.GetFontCharWidth()
					,row*mWindowTracker.GetFontCharHeight(),(char *)&(element->Value),1);
				}
			}
		}
	else // !(blocked==true)
		{
		//Render a Normal Cell
		COLORREF color;
			
		// Set Back Color
		if(element==NULL)
			{
			color=mBackPalette.Get(GFillElement.GetBackType());
			}
		else
			{
			color=mBackPalette.Get(element->GetBackType() );
			}

		SetBkColor(hdc,color);

		if(element==NULL)
			{
			TextOutA(hdc,col*mWindowTracker.GetFontCharWidth()
					,row*mWindowTracker.GetFontCharHeight(),GTabFillChar,1);
			}
		else
			{
			// If Tab write a space
			if(element->Value=='\t')
				{
				TextOutA(hdc,col*mWindowTracker.GetFontCharWidth()
						,row*mWindowTracker.GetFontCharHeight(),GTabFillChar,1);
				}
			else
				{
				COLORREF color=mTextPalette.Get(element->GetTextType() );
				SetTextColor(hdc,color );
				TextOutA(hdc,col*mWindowTracker.GetFontCharWidth()
					,row*mWindowTracker.GetFontCharHeight(),(char *)&(element->Value),1);
				}
			}
		}
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::RenderLine(HDC hdc,int linerow,int windowrow)		// render linerow to window row
	{
	// Get a pointer to all line elements
	// Get the maximum length
	MEditBufferElement *elements;
	int linelength=mBuffer.GetCols(linerow);
	if(linelength>0)
		{
		elements=mBuffer.Get(linerow,0);
		MStdAssert(elements!=NULL);
		}
	else
		{
		// Set Elements to NULL
		elements=NULL;
		}

	// Track the maximum write position of window
	const int topcol=mWindowTracker.GetTopCol();
	int maxwritepos=topcol+mWindowTracker.GetWindowCols();
	
	int writepos=0;
	MEditBufferElement *writech;
	int k,i;
	int tabsize=mWindowTracker.GetTabSize();
	for(i=0;i<maxwritepos;++i)
		{
		// if writeposition
		if(writepos>=maxwritepos) { return true; }
		// Check Find Next Character
		if( i>=linelength)
			{ writech=&GFillElement;  }
		else
			{ writech=elements+i; }

		// checkif writech is tab
		if(writech!=NULL && writech->Value=='\t')
			{
			for(k=0;k<tabsize;++k)
				{
				if(writepos>=topcol)
					{
					if(mBlockOps.IsBlocked()==true
							&& mBlockOps.IsBlocked(linerow,i)==true)
						{
						// Draw a blocked character
						DrawChar(hdc,windowrow,writepos-topcol,writech,true);
						}
					else
						{
						// Draw a normal character
						DrawChar(hdc,windowrow,writepos-topcol,writech,false);
						}
					}

				writepos=writepos+1;
				}

			continue;
			}

		// Just a normal character
		if(writepos>=topcol)
			{
			if(mBlockOps.IsBlocked()==true
							&& mBlockOps.IsBlocked(linerow,i)==true)
				{  DrawChar(hdc,windowrow,writepos-topcol,writech,true);  }
			else
				{  DrawChar(hdc,windowrow,writepos-topcol,writech,false);  }
			}

		writepos=writepos+1;
		}

	return true;
	}

      
////////////////////////////////////////////////
bool MWinEditor::RenderBlankLine(HDC hdc,int windowrow)
	{
	int windowcols=mWindowTracker.GetWindowCols();
	for(int i=0;i<windowcols;++i)
		{
		// Draw NULL chars
		DrawChar(hdc,windowrow,i,&GFillElement,false);
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::RedrawScreen(HDC hdc)  // Render whole screen
	{
	int r;
	const int bufferlength=mBuffer.GetRows();
	const int windowrows=mWindowTracker.GetWindowRows();
	const int toprow=mWindowTracker.GetTopRow();
	for(r=0;r<windowrows;++r)
		{
		// Check if you have reached the end of buffer
		if(r+toprow>=bufferlength)
			{
			RenderBlankLine(hdc,r);
			continue;
			}
		RenderLine(hdc,r+toprow,r);
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::UpdateScreenSilently(void)  // 
	{
	MWinGDIClientDC clientdc(mhMainWindow);
	clientdc.SelectObject(mFont);
	clientdc.SelectObject(mBrush);
	clientdc.SetTextColor(RGB(0,0,200) );
	RedrawScreen(clientdc.GetDC());
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::UpdateLineSilently(void)
	{
	MWinGDIClientDC clientdc(mhMainWindow);
	clientdc.SelectObject(mFont);
	clientdc.SelectObject(mBrush);
	clientdc.SetTextColor(RGB(0,0,200) );
	
	int windowrow=mWindowTracker.GetCaretRow();
	int toprow=mWindowTracker.GetTopRow();
	RenderLine(clientdc.GetDC(),toprow+windowrow,windowrow);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::GetBlockedText(int blockstart,int blockstartoffset
			,int blockend,int blockendoffset,MDynamicString &strbuffer)
	{
	// Run Through The Editor Group and set all the elements erase
	int row=blockstart; int offset=blockstartoffset;
	int rowlength=mBuffer.GetCols(row);
	

	if(blockstart==blockend && blockstartoffset==blockendoffset)
		{
		//=We have nothing really blocked
		return false;
		}

	MEditBufferElement *element; 

	// Init a dynamic string
	if(strbuffer.Create()==false)
		{
		return false;
		}

	int count=0;  // Hold the count of chars to delete
	for(;;)
		{
		if(offset>=rowlength)
			{
			//= Reached the end of the line
			//Update row information
			row=row+1;  offset=0;
			rowlength=mBuffer.GetCols(row);
			strbuffer.Insert('\n'); // New Line Insert
			}
		else
			{
			element=mBuffer.Get(row,offset);  // Get the element
			strbuffer.Insert(element->Value); // Append the character
			offset=offset+1;
			}

		if(row==blockend && offset==blockendoffset) { break; }
		}

 	return true;
	}


/////////////////////////////////////////////////
bool MWinEditor::MoveCaretUp(void)
	{
	if(mWindowTracker.MoveUp()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
				|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;	
	}


////////////////////////////////////////////////
bool MWinEditor::MoveUpScreenLine(void)				// Move the screen up by 1 line
	{
	// Reposition Screen
	if(mWindowTracker.MoveUpScreenLine()==false){ return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
				|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::MoveCaretDown(void)
	{
	if(mWindowTracker.MoveDown()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;	
	}


//////////////////////////////////////////////////
bool MWinEditor::MoveDownScreenLine(void)
	{
	if(mWindowTracker.MoveDownScreenLine()==false){ return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;	
	}


//////////////////////////////////////////////////
bool MWinEditor::MoveCaretLeft(void)
	{
	if(mWindowTracker.MoveLeft()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}

	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;		
	}


////////////////////////////////////////////////
bool MWinEditor::MoveCaretPrevWord(void)
	{
	if(mWindowTracker.MoveLastword()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}

	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;	
	}


////////////////////////////////////////////////
bool MWinEditor::MoveCaretRight(void)
	{
	if(mWindowTracker.MoveRight()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();


	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }
		

	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}

	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::MoveCaretNextWord(void)
	{
	if(mWindowTracker.MoveNextWord()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();


	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }
		

	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}

	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::MoveCaretLineStart(void)
	{
	if(mWindowTracker.MoveLineBegin()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();


	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();


	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
				|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;			
	}


////////////////////////////////////////////////
bool MWinEditor::MoveCaretDocStart(void)
	{
	// Move to beginning of document
	if(mWindowTracker.MoveDocStart()==false) { return true; }
	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();


	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();


	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
				|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;	
	}


////////////////////////////////////////////////
bool MWinEditor::MoveCaretLineEnd(void)
	{
	if(mWindowTracker.MoveLineEnd()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();


	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
				|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;
	}


///////////////////////////////////////////////////
bool MWinEditor::MoveCaretDocEnd(void)
	{
	if(mWindowTracker.MoveDocEnd()==false) { return true; }

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();


	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();

	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
				|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;
	}


///////////////////////////////////////////////////
bool MWinEditor::MoveCaretLastPage(void)				// Move Caret prior
	{
	mWindowTracker.MovePageUp();

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();

	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();


	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;		
	}


//////////////////////////////////////////////////
bool MWinEditor::MoveCaretNextPage(void)		// Move Caret next page
	{
	mWindowTracker.MovePageDown();

	// Find Caret Position after change
	int caretrow,caretcol;
	caretrow=mWindowTracker.GetCaretRow();
	caretcol=mWindowTracker.GetCaretCol();


	// Get Edit Position 
	int editrow=mWindowTracker.GetEditLine();
	int editoffset=mWindowTracker.GetEditOffset();


	// Check if Shift key is being held for block operation
	if(GetKeyState(VK_SHIFT)<0)
		{  mBlockOps.BlockStop(editrow,editoffset);  }
	else
		{  mBlockOps.BlockStart(editrow,editoffset);  }


	// Update Screen if necessary
	if(mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mBlockOps.IsBlocked()==true
			|| mBlockOps.IsBlockStopped()==true) // Update Current line due to block operation
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}


	SetCaretPos(mWindowTracker.GetFontCharWidth()*caretcol
			,mWindowTracker.GetFontCharHeight()*caretrow);
	return true;
	}


/////////////////////////////////////////////////
bool MWinEditor::DeleteChar(void)
	{
	char deletech;
	int deleterow,deleteoffset;
	if(mWindowTracker.Delete(deletech,deleterow,deleteoffset)==false)
		{
		mBlockOps.BlockStart(deleterow,deleteoffset);
		mBlockOps.BlockStop(deleterow,deleteoffset);
		return true;
		}

	// Set Modified Flag
	mModified=true;

	// Perform UNDO OP
	mUndoRedo.DeleteChar(deletech,deleterow,deleteoffset);

	mRefLexer->Parse(mWindowTracker.GetEditLine());

	if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mWindowTracker.IsLineUpdated()==true)
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		}

	int row,col;
	row=mWindowTracker.GetCaretRow();
	col=mWindowTracker.GetCaretCol();

	SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::PressEnter(void)
	{
	int retrow,retoffset;
	if(mWindowTracker.NewLine(retrow,retoffset)==false) { return true; }

	// Perform Undo/Redo
	mUndoRedo.InsertChar('\n',retrow,retoffset);

	bool updatescreen;
	mRefLexer->Parse(mWindowTracker.GetEditLine());
	updatescreen=mRefLexer->IsScreenUpdated();

	// Parse the prior line
	mRefLexer->Parse(mWindowTracker.GetEditLine()-1);

	if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}

	int row,col;
	row=mWindowTracker.GetCaretRow();
	col=mWindowTracker.GetCaretCol();

	SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
	return true;		
	}


/////////////////////////////////////////////////
bool MWinEditor::DeleteBlock(void)
	{
	if(mBlockOps.IsBlocked()==false) { return false; }
	int blockstart,blockstartoffset,blockend,blockendoffset;
	if(mBlockOps.GetBlockedRegion(blockstart,blockstartoffset
				,blockend,blockendoffset)==false)
		{ return false;  }

	// if beyond last line, set to end of last line
	int linecount=mBuffer.GetRows();
	if(linecount==blockend)
		{	// Set to last character of buffer
		blockend=linecount-1;
		blockendoffset=mBuffer.GetCols(blockend);
		}

	// remove the blocked text region
	MDynamicString blocktext;
	if(GetBlockedText(blockstart,blockstartoffset,blockend
				,blockendoffset,blocktext)==false)
		{
		mBlockOps.BlockStart(0,0);
		mBlockOps.BlockStop(0,0);
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		return false;
		}

	// Set Modified Flag
	mModified=true;

	mWindowTracker.SetEditPosition(blockstart,blockstartoffset);
	mWindowTracker.CutText(blocktext.Get());
	mBlockOps.BlockStart(blockstart,blockstartoffset);
	mUndoRedo.CutText(blocktext.Get(),blockstart,blockstartoffset);  // Track undo operation

	mRefLexer->Parse(mWindowTracker.GetEditLine());

	if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true
			|| mBlockOps.IsBlockStopped()==true)
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mWindowTracker.IsLineUpdated()==true)
		{
		HideCaret(mhMainWindow);
		UpdateLineSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetLineUpdated();
		}

	int row,col;
	row=mWindowTracker.GetCaretRow();
	col=mWindowTracker.GetCaretCol();
	SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::InsertChar(char ch)
	{
	int insertrow,insertoffset;
	if(mWindowTracker.Insert(ch,insertrow,insertoffset)==false)
		{  return true; }

	// Set Modified Flag
	mModified=true;

	// Update Position of Block
	mBlockOps.BlockStart(insertrow,insertoffset+1);
	mBlockOps.BlockStart(insertrow,insertoffset+1);

	mUndoRedo.InsertChar(ch,insertrow,insertoffset);

	mRefLexer->Parse(mWindowTracker.GetEditLine());

	if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true
			|| mBlockOps.IsBlockStopped()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mWindowTracker.IsLineUpdated()==true)
		{
		HideCaret(mhMainWindow);
		UpdateLineSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetLineUpdated();
		}

	int row,col;
	row=mWindowTracker.GetCaretRow();
	col=mWindowTracker.GetCaretCol();
	SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
	return true;		
	}


////////////////////////////////////////////////
bool MWinEditor::BackSpace(void)
	{
	char deletech;
	int deleterow,deletecol;
	if(mWindowTracker.BackSpace(deletech,deleterow,deletecol)==false)
		{ return true; }

	mUndoRedo.DeleteChar(deletech,deleterow,deletecol);
	
	mRefLexer->Parse(mWindowTracker.GetEditLine());

	if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true
			|| mBlockOps.IsBlockStopped()==true)
		{ 
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else if(mWindowTracker.IsLineUpdated()==true)
		{
		HideCaret(mhMainWindow);
		UpdateLineSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetLineUpdated();
		}

	int row,col;
	row=mWindowTracker.GetCaretRow();
	col=mWindowTracker.GetCaretCol();
	SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);

	return true;
	}


////////////////////////////////////////////////
MWinEditor::MWinEditor(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinEditor::~MWinEditor(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinEditor::Create(int x,int y,int width,int height,HWND hparent
			,int style)
	{
	Destroy();
	
	// Create Buffer
	if(mBuffer.Create()==false)
		{
		Destroy();
		return false;
		}

	//Create font
	if(mFont.Create()==false)
		{
		Destroy();
		return false;
		}

	//Create Brush
	if(mBrush.Create(RGB(250,250,250))==false)
		{
		Destroy();
		return false;
		}

	// Create the NULL Parser
	if(mEmptyLexer.Create()==false)
		{
		Destroy();
		return false;
		}

	// Create the parser instance
	if(mBaseLexer.Create(mBuffer)==false)
		{
		Destroy();
		return false;
		}

	// Create the palette
	if(mTextPalette.Create(GTextColorSet,sizeof(GTextColorSet)/sizeof(COLORREF))==false)
		{
		Destroy();
		return false;
		}

	// Create the palette
	if(mBackPalette.Create(GBackColorSet
				,sizeof(GBackColorSet)/sizeof(COLORREF))==false)
		{
		Destroy();
		return false;
		}

	// Create the undoredo object
	if(mUndoRedo.Create()==false)
		{
		Destroy();
		return false;
		}

	// Set Default Lexer
	mRefLexer=&mBaseLexer;

	//Register Class
	WNDCLASS wndclass; memset(&wndclass,0,sizeof(wndclass) );
	//wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.lpfnWndProc=WindowProc;
	wndclass.lpszClassName=GWindowClassName;
	wndclass.hInstance=GetModuleHandle(NULL);
	RegisterClass(&wndclass); // Can fail if called many times

	//Create main window
	mhMainWindow=CreateWindow(GWindowClassName,GWindowClassName
			,style,x,y,width,height,hparent,NULL,GetModuleHandle(NULL),NULL);
	if(mhMainWindow==NULL)
		{
		return false;
		}

	// Set Object Property
	if(SetProp(mhMainWindow,GObjectRefProp,this)==FALSE)
		{
		Destroy();
		return false;
		}

	// Set up CaretTracker
	if(mWindowTracker.Create(mBuffer)==false)
		{
		Destroy();
		return false;
		}

	// Init MClipBoard object
	if(mClipBoard.Create(mhMainWindow)==false)
		{
		Destroy();
		return false;
		}

	// Init mBlockOps
	if(mBlockOps.Create()==false)
		{
		Destroy();
		return false;
		}

	// Update FontCharacter dimensions
	MWinGDIClientDC clientdc(mhMainWindow);
	clientdc.SelectObject(mFont);

	TEXTMETRIC tm;
	if(clientdc.GetTextMetrics(&tm)==false)
		{
		// Unable to create TEXTMETRIC Info
		Destroy();
		return false;
		}

	mWindowTracker.SetFontSize(tm.tmMaxCharWidth,tm.tmHeight+tm.tmExternalLeading);
	clientdc.Destroy();

	// Call Resize Again
	EventSize();
	EventSetFocus();

	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::Destroy(void)
	{
	mBrush.Destroy();
	mBuffer.Destroy();
	mFont.Destroy();
	mWindowTracker.Destroy();
	mBaseLexer.Destroy();
	mTextPalette.Destroy();
	mBackPalette.Destroy();
	mUndoRedo.Destroy();
	mClipBoard.Destroy();
	mBlockOps.Destroy();

	if(mhMainWindow!=NULL)
		{
		// Destroy Window
		DestroyWindow(mhMainWindow);
		mhMainWindow=NULL;
		}

	ClearObject();
	return true;
	}


///////////////////////////////////////////////
HWND MWinEditor::GetHWND(void)
	{
	return mhMainWindow;
	}


////////////////////////////////////////////////
bool MWinEditor::MessageLoop(void)			// Perform a message loop
	{
	// Windows Message loop
	MSG msg;
	while(GetMessage(&msg,NULL,0,0)==TRUE)
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinEditor::LoadTextFile(const char *filename)		// Load Text File into buffer
	{
	// Update Modified if buffer not empty
	if(mBuffer.GetRows()!=0)
		{
		// Set Modifed Flag
		mModified=true;
		}

	// Load the file
	if(mWindowTracker.LoadTextFile(filename)==false)
		{
		return false;
		}
	
	// Parse Whole File
	mRefLexer->ParseAll();

	if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else
		{
		HideCaret(mhMainWindow);
		UpdateLineSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetLineUpdated();
		}

	int row,col;
	row=mWindowTracker.GetCaretRow();
	col=mWindowTracker.GetCaretCol();
	SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
	return true;	
	}


////////////////////////////////////////////////
bool MWinEditor::SaveTextFile(const char *filename)
	{

	if(mWindowTracker.SaveTextFile(filename)==false)
		{
		return false;
		}

	// Update mModified to false
	mModified=false;

	return true;
	}


///////////////////////////////////////////////////////
bool MWinEditor::IsUndoPossible(void)			// Is Ubdo Possible
	{
	return mUndoRedo.IsUndoPossible();
	}


///////////////////////////////////////////////////////
bool MWinEditor::IsRedoPossible(void)			// IsRedo possible
	{
	return mUndoRedo.IsRedoPossible();
	}


///////////////////////////////////////////////////////
bool MWinEditor::UndoLastOperation(void)		// Undo the last operation
	{
	if(mUndoRedo.IsUndoPossible()==false) { return false; }

	const char *undodata=mUndoRedo.GetUndoString();
	int undolength=MStdStrLen(undodata);
	int undorow=mUndoRedo.GetUndoRow();
	int undooffset=mUndoRedo.GetUndoColumn();
	MUndoStackType undotype=mUndoRedo.GetUndoType();

	// Undo an insert text
	if(undotype==MUST_INSERT)
		{
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.CutText(undodata);	// Cut Text
		mUndoRedo.Undo(); // perform an undo operation
		mRefLexer->Parse(undorow);  // Parse over line

		if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
			{
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}
		else
			{
			HideCaret(mhMainWindow);
			UpdateLineSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetLineUpdated();
			}

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}

	// Check for Undo of of a Delete
	if(undotype==MUST_DELETE)
		{	// Undo a Delete
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.PasteText(undodata);	//Paste Text at position
		mUndoRedo.Undo();			// perform an undo operation
		mRefLexer->Parse(undorow);		// Parse over line

		if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
			{
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}
		else
			{
			HideCaret(mhMainWindow);
			UpdateLineSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetLineUpdated();
			}

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;	
		}

	// Undo an overwrite text
	if(undotype==MUST_OVERWRITE)
		{
		//= Undo a normal line fragment 
		for(int i=0;undodata[i]!=0;i+=2) // Double Character Sequence
			{
			MEditBufferElement *element=mBuffer.Get(undorow,undooffset);
			element->Value = *(undodata+i+1);
			}

		mUndoRedo.Undo(); // perform an undo operation

		mRefLexer->Parse(undorow);  // Parse over line

		mWindowTracker.SetEditPosition(undorow,undooffset);

		if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
			{
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}
		else
			{
			HideCaret(mhMainWindow);
			UpdateLineSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetLineUpdated();
			}

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}


	// Undo the Paste Paste Text
	if(undotype==MUST_PASTE)
		{
		// Paste Text which may contain \n
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.CutText(undodata);
		mUndoRedo.Undo(); // perform an undo operation

		mRefLexer->Parse(undorow);  // Parse over line

		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}


	// Undo Cut Text
	if(undotype==MUST_CUT)
		{
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.PasteText(undodata); // Paste text back at position

		// Get Line Count of paste
		int linecount=GGetLineCount(undodata);

		mUndoRedo.Undo(); // perform an undo operation

		// Parse All the text again
		int i;
		for(i=undorow;i<undorow+linecount;++i)
			{  mRefLexer->Parse(i);  }

		mWindowTracker.SetEditPosition(undorow,undooffset);

		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}
	
	return true;
	}


///////////////////////////////////////////////////////
bool MWinEditor::RedoLastOperation(void)		// Redo the last operation
	{
	if(mUndoRedo.IsRedoPossible()==false) { return false; }

	if(mUndoRedo.Redo()==false)
		{
		return false;
		}

	const char *undodata=mUndoRedo.GetUndoString();
	int undolength=MStdStrLen(undodata);
	int undorow=mUndoRedo.GetUndoRow();
	int undooffset=mUndoRedo.GetUndoColumn();
	MUndoStackType undotype=mUndoRedo.GetUndoType();

	// Check for Insert Redo	
	if(undotype==MUST_INSERT)
		{	//=Perform an insert of text
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.PasteText(undodata);
		mRefLexer->Parse(undorow);

		if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
			{ 
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}
		else
			{
			HideCaret(mhMainWindow);
			UpdateLineSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetLineUpdated();
			}

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;		
		}

	// Check for Delete Redo	
	if(undotype==MUST_DELETE)
		{//=Perform an undo of the text
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.CutText(undodata);
		mRefLexer->Parse(undorow);

		if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
			{ 
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}
		else
			{
			HideCaret(mhMainWindow);
			UpdateLineSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetLineUpdated();
			}

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}

	// Redo an overwrite text
	if(undotype==MUST_OVERWRITE)
		{
		//= Undo a normal line fragment 
		for(int i=0;undodata[i]!=0;i+=2) // Double Character Sequence
			{
			MEditBufferElement *element=mBuffer.Get(undorow,undooffset);
			element->Value = *(undodata+i+0);
			}

		mUndoRedo.Undo(); // perform an undo operation

		mRefLexer->Parse(undorow);  // Parse over line

		mWindowTracker.SetEditPosition(undorow,undooffset);

		if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
			{
			HideCaret(mhMainWindow);
			UpdateScreenSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetScreenUpdated();
			}
		else
			{
			HideCaret(mhMainWindow);
			UpdateLineSilently();
			ShowCaret(mhMainWindow);
			mWindowTracker.ResetLineUpdated();
			}

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}

	// Redo Paste Paste Text
	if(undotype==MUST_PASTE)
		{
		// Paste Text which may contain \n
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.PasteText(undodata); // Paste text back at position
		mRefLexer->Parse(undorow);  // Parse over line
		mWindowTracker.SetEditPosition(undorow,undooffset);	// Move to Fist position
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}

	// Redo Cut
	if(undotype==MUST_CUT)
		{
		// Paste Text which may contain \n
		mWindowTracker.SetEditPosition(undorow,undooffset);
		mWindowTracker.CutText(undodata);
		mRefLexer->Parse(undorow);  // Parse over line

		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();

		int row,col;
		row=mWindowTracker.GetCaretRow();
		col=mWindowTracker.GetCaretCol();
		SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
		return true;
		}


	return true;
	}


//////////////////////////////////////////////////////////
bool MWinEditor::PasteClipBoardText(void)			// Paste Text from clipboard
	{
	// Check if clipboard has text
	if(mClipBoard.HasText()==false) {  return false;  }

	// If ReadOnly, do not paste
	if(mReadOnly==true) { return false; }

	// Get Clipboard Text
	MBuffer text;
	if(mClipBoard.GetText(text)==false)
		{
		return false;
		}

	// Clean any Wierd Characters from  paste text
	MStringBuffer cleanedtext;
	if(cleanedtext.Create(text.GetSize()+1)==false)
		{
		return false;
		}

	int textlength=MStdStrLen(text.GetBuffer() ); int i;
	for(i=0;i<textlength;++i)
		{
		char ch=text[i];
		if(GIsDisplayable(ch)==false) { continue; }
		//=char is displayable
		cleanedtext.Add(ch);
		}

	// Insert into buffer
	mWindowTracker.PasteText(cleanedtext.Get());

	// Set Modified Flag
	mModified=true;

	// Update undo information
	mUndoRedo.PasteText(cleanedtext.Get(),mWindowTracker.GetEditLine()
			,mWindowTracker.GetEditOffset() );

	//Find the first line which is not empty to parse
	int lineno=mWindowTracker.GetEditLine();
	while(lineno<mBuffer.GetRows())
		{
		if(mBuffer.GetCols(lineno)>0) { break; }
		lineno=lineno+1;
		}

	// If Reached End then set to edit line
	if(lineno==mBuffer.GetRows())
		{  lineno=mWindowTracker.GetEditLine();  }
	
	mRefLexer->Parse(lineno);

	if(mRefLexer->IsScreenUpdated()==true || mWindowTracker.IsScreenUpdated()==true)
		{
		HideCaret(mhMainWindow);
		UpdateScreenSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetScreenUpdated();
		}
	else
		{
		HideCaret(mhMainWindow);
		UpdateLineSilently();
		ShowCaret(mhMainWindow);
		mWindowTracker.ResetLineUpdated();
		}

	int row,col;
	row=mWindowTracker.GetCaretRow();
	col=mWindowTracker.GetCaretCol();
	SetCaretPos(mWindowTracker.GetFontCharWidth()*col,mWindowTracker.GetFontCharHeight()*row);
	return true;	
	}


///////////////////////////////////////////////////////
bool MWinEditor::CopyTextToClipBoard(void)				// Copy Text to clipboard
	{
	if(mBlockOps.IsBlocked()==false) { return false; }

	int blockstart,blockstartoffset,blockend,blockendoffset;
	if(mBlockOps.GetBlockedRegion(blockstart,blockstartoffset
				,blockend,blockendoffset)==false)
		{ return false;  }

	// if beyond last line, set to end of last line
	int linecount=mBuffer.GetRows();
	if(linecount==blockend)
		{	// Set to last character of buffer
		blockend=linecount-1;
		blockendoffset=mBuffer.GetCols(blockend);
		}

	// Get Blocked text
	MDynamicString blocktext;
	if(GetBlockedText(blockstart,blockstartoffset,blockend,blockendoffset,blocktext)==false)
		{
		return false;
		}

	// Copy to clipboard
	if(mClipBoard.CopyText(blocktext.Get())==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////
bool MWinEditor::CanPasteTextFromClipBoard(void)
	{
	// If ReadOnly, do not paste
	if(mReadOnly==true) { return false; }

	return mClipBoard.HasText();
	}


////////////////////////////////////////////////////
bool MWinEditor::HasBlockedText(void)
	{
	return mBlockOps.IsBlocked();
	}


///////////////////////////////////////////////////
bool MWinEditor::SetReadOnly(bool flag)
	{
	mReadOnly=flag;
	return true;
	}


////////////////////////////////////////////////////
bool MWinEditor::IsReadOnly(void)
	{
	return mReadOnly;
	}


///////////////////////////////////////////////////
MWindowTracker *MWinEditor::GetWindowTracker(void)
	{
	return &mWindowTracker;
	}


///////////////////////////////////////////////////
MEditBuffer *MWinEditor::GetEditBuffer(void)
	{
	return &mBuffer;
	}


//////////////////////////////////////////////////
MColorPalette *MWinEditor::GetTextPalette(void)
	{
	return &mTextPalette;
	}


//////////////////////////////////////////////////
bool MWinEditor::Update(void)
	{
	return UpdateScreenSilently();	
	}


///////////////////////////////////////////////////
bool MWinEditor::Show(bool flag)
	{
	if(flag==true)
		{  ShowWindow(mhMainWindow,SW_SHOW);  }
	else
		{  ShowWindow(mhMainWindow,SW_HIDE);  }

	return true;
	}


//////////////////////////////////////////////////
bool MWinEditor::ColorBlock(int startrow,int startoffset,int endrow,int endcol,int color)
	{
	// Wrap Color into BackPalette
	color = color % mBackPalette.GetLength();

	if(mBuffer.SetBackType(color,startrow,startoffset,endrow,endcol)==false)
		{
		//=Unable to set back colortype
		return false;
		}

	HideCaret(mhMainWindow);
	UpdateScreenSilently();
	ShowCaret(mhMainWindow);
	mWindowTracker.ResetScreenUpdated();

	return true;
	}


//////////////////////////////////////////////////////////
bool MWinEditor::ColorBlock(int color)
	{
	int lastline=mBuffer.GetRows();
	int lastcol=mBuffer.GetCols(lastline-1)-1;

	return ColorBlock(0,0,lastline,lastcol,color);
	}


//////////////////////////////////////////////////////////
bool MWinEditor::SetLanguage(const char *language)
	{
	if(language==NULL)
		{  language="None";  }

	if(MStdStrCmp("C++",language)==0
			||  MStdStrCmp("C#",language)==0
			||  MStdStrCmp("C",language)==0 
			||  MStdStrCmp("MSQL",language)==0 )
		{
		if(mBaseLexer.Create(mBuffer,language)==false)
			{
			return false;
			}

		// Point to new lexer
		mRefLexer=&mBaseLexer;

		// Reparse the whole editor
		mRefLexer->ParseAll();
		UpdateScreenSilently();
		return true;
		}

	// Set the default Language here
	mRefLexer=&mEmptyLexer;
	mBuffer.SetBackType(0);
	mBuffer.SetTextType(0);
	UpdateScreenSilently();

	return false;	
	}


//////////////////////////////////////////////////////////
bool MWinEditor::GetLexers(MStringList &langlist)
	{
	if(langlist.Create()==false)
		{
		return false;
		}

	// Add the language Lists 
	langlist.AddString("C++");
	langlist.AddString("C#");
	langlist.AddString("C");
	langlist.AddString("MSQL");

	return true;
	}


////////////////////////////////////////////////////////
bool MWinEditor::IsModified(void)
	{
	return mModified;
	}

