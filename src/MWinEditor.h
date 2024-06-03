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
#ifndef MWinEditor_h
#define MWinEditor_h

////////////////////////////////////////////////
#include <windows.h>
#include "MWinGDI.h"
#include "MWindowTracker.h"
#include "MEditBuffer.h"
#include "MEditBufferParser.h"
#include "MColorPalette.h"
#include "MEditUndoRedo.h"
#include "MClipBoard.h"
#include "MBlockOps.h"
#include "MDynamicString.h"
#include "MStringList.h"
#include "MEmptyLexer.h"

//******************************************************
//**  MWinEditor class
//******************************************************
class MWinEditor
	{
	HWND mhMainWindow;					// Handle to main window
	MEditBuffer mBuffer;				// Holds the character data
	MWinGDIFont mFont;					// Main Font to be used for displaying text
	MWinGDISolidBrush mBrush;			// Used to erase background
	MWindowTracker mWindowTracker;		// Controls the window dimensions/caret
	MEmptyLexer mEmptyLexer;			// Empty Lexer
	MEditBufferParser mBaseLexer;		// Default Lexer for C++ et al
	MILexer *mRefLexer;					// Parses the edit buffer text
	MColorPalette mTextPalette;			// Text Colors for rendering Text
	MColorPalette mBackPalette;			// Text Colors for rendering Cells
	MEditUndoRedo mUndoRedo;			// Holds the Undo/Redo Data
	MClipBoard mClipBoard;				// Clipbord object
	MBlockOps mBlockOps;				// Tracks the Blocked Region
	bool mReadOnly;						// Flag to check for readonly editor
	bool mModified;						// Flag to check if file modified

	////////////////////////////////////////////////
	void ClearObject(void);
	static LRESULT CALLBACK WindowProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);

	////////// WindowEvent Handlers ///////////////////
	bool EventChar(WPARAM keycode);		// Handle WM_CHAR
	bool EventClose(void);				// Handle WM_CLOSE
	bool EventPaint(void);				// Handle WM_PAINT
	bool EventSize(void);				// Handle WM_SIZE
	bool EventSetFocus(void);			// Handle WM_SETFOCUS
	bool EventKillFocus(void);			// Handle WM_KILLFOCUS
	bool EventKeyDown(WPARAM keycode);	// Handle WM_KEYDOWN
	bool EventLButtonDown(void);		// Handle WM_LBUTTONDOWN
	bool EventMouseMove(void);			// Handle WM_MOUSEMOVE
	bool EventLButtonUp(void);			// Handle WM_LBUTTONUP
	
	////////////////////////////////////////////////
	bool DrawChar(HDC hdc,int row,int col,MEditBufferElement *element
				,bool blocked);			// Draw a single element
	bool RenderLine(HDC hdc,int linerow,int widowrow);	// render linerow to window row
	bool RenderBlankLine(HDC hdc,int windowrow);	// render a blank line
	bool RedrawScreen(HDC hdc);			// Render whole screen
	bool UpdateScreenSilently();		// Update screen directly (no WM_PAINT)
	bool UpdateLineSilently(void);		// Updates current line
	bool GetBlockedText(int blockstart,int blockstartoffset
			,int blockend,int blockendoffset
			,MDynamicString &strbuffer);	// Remove and return blocked region

	//////////////////////////////////////////////////
	bool MoveCaretUp(void);				// Move the Caret up by 1 line
	bool MoveUpScreenLine(void);		// Move the screen up by 1 line
	bool MoveCaretDown(void);			// Move the Caret down by 1 line
	bool MoveDownScreenLine(void);		// Move the screen down by 1 line
	bool MoveCaretLeft(void);			// Move Caret left by 1 character
	bool MoveCaretPrevWord(void);		// Move Caret to Prev Word
	bool MoveCaretRight(void);			// Move Caret right by 1 character
	bool MoveCaretNextWord(void);		// Move Caret to next word
	bool MoveCaretLineStart(void);		// Move Caret to line start
	bool MoveCaretDocStart(void);		// Move Caret to top of document
	bool MoveCaretLineEnd(void);		// Move Caret to Line end
	bool MoveCaretDocEnd(void);			// Move Caret to end of document
	bool MoveCaretLastPage(void);		// Move Caret prior page
	bool MoveCaretNextPage(void);		// Move Caret next page
	bool DeleteChar(void);				// Delete the current character
	bool PressEnter(void);				// Press the return key
	bool DeleteBlock(void);				// Delete the current block
	bool InsertChar(char ch);			// Insert a single character
	bool BackSpace(void);				// Press the backspace key

	////////////////////////////////////////////////
	public:
	MWinEditor(void);
	~MWinEditor(void);
	bool Create(int x=100,int y=100,int width=400,int height=500,HWND hparent=NULL
			,int style=WS_OVERLAPPEDWINDOW);
	bool Destroy(void);
	HWND GetHWND(void);					// Get Handle to window
	bool MessageLoop(void);				// Perform a message loop
	bool LoadTextFile(const char *filename);	// Load Text File into buffer
	bool SaveTextFile(const char *filename);	// Save Text File into buffer
	bool IsUndoPossible(void);			// Is Undo Possible
	bool IsRedoPossible(void);			// IsRedo possible
	bool UndoLastOperation(void);		// Undo the last operation
	bool RedoLastOperation(void);		// Redo the last operation
	bool PasteClipBoardText(void);		// Paste Text from clipboard
	bool CopyTextToClipBoard(void);		// Copy Text to clipboard
	bool CanPasteTextFromClipBoard(void);	// Checks if text is avail from clipboard
	bool HasBlockedText(void);			// Checks if text is blocked
	bool SetReadOnly(bool flag=true);	// Set/Reset Readonly flag
	bool IsReadOnly(void);				// Set ReadOnly
	MWindowTracker *GetWindowTracker(void);		// Get Reference to window tracker
	MEditBuffer *GetEditBuffer(void);	// Get Reference to Edit Buffer
	MColorPalette *GetTextPalette(void);// Get Reference to Text Color Palete
	bool Update(void);					// calls update screen silently
	bool Show(bool flag=true);			// Show Window
	bool ColorBlock(int startrow,int startoffset,int endrow,int endcol,int color=2);
	bool ColorBlock(int color=2);		// Color a blocked region
	bool SetLanguage(const char *language);	// Set the language
	bool GetLexers(MStringList &langlist);	//  Get Languages which can be processed
	bool IsModified(void);				// Check if buffer modified
	};

#endif // MWinEditor_h

