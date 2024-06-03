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


//v1.6 copyright Comine.com 202440603M1105
#ifndef MWindowTracker_h
#define MWindowTracker_h

//******************************************************
//**  MWindowTracker class
//******************************************************
class MEditBuffer;
class MWindowTracker
	{

	MEditBuffer *mBuffer;		// Holds a reference to buffer
	int mScreenWidth;			// Holds the Screen Width
	int mScreenHeight;			// Holds the Screen Height
	int mCarotRow;				// Location of the Carot Row
	int mCarotCol;				// Location of the Carot Col
	int mWindowRows;			// Calculated # of rows in window
	int mWindowCols;			// Calculated # of cols in window
	int mTopRow;				// Window Origin Row
	int mTopCol;				// Window Origin Col
	int mFontCharWidth;			// Font Cell width
	int mFontCharHeight;		// Font Cell height
	int mTabSize;				// reference to tab size
	int mEditLine;				// Holds position of where the edit point is
	int mEditDistance;			// Holds the real left distance of edit point
	bool mScreenUpdate;			// =true if whole window should be redrawn
	bool mLineUpdate;			// =true if line should be updated

	//////////////////////////////////////////////
	void ClearObject(void);
	int GetLeftDistance(int lineno);					//=left distance of last position
	int GetLeftDistance(int lineno,int elementpos);		//=left distance of element pos in lineno
	int GetElmentLeftDistance(int lineno,int elementdistance);	//= left distance given an element
	int GetNextElementLeftDistance(int lineno,int elementdistance);	// left distance of next element
	int GetPrevElementLeftDistance(int lineno,int elementdistance);	// left distance of previous element
	bool UpdateWindowPosition(void);					// updates the window position
	bool UpdateCaretPosition(void);						// updated the caret position
	int GetOffset(int lineno,int leftdistance);		// = returns the left offset given left distance

	////////////////////////////////////////////////
	public:
	MWindowTracker(void);
	~MWindowTracker(void);
	bool Create(MEditBuffer &buffer);
	bool Destroy(void);

	// If any of the following functions return false, Screen has
	// to be redrawn
	bool MoveUp(void);					// Move up cursor by one line
	bool MoveDown(void);				// Move down cursor by one line
	bool MoveLeft(void);				// Move cursor left by 1 character
	bool MoveRight(void);				// Move cursor right by 1 character
	bool MoveLineBegin(void);			// Move cursor to beginning of line
	bool MoveLineEnd(void);				// Move cursor to end of line
	bool MovePageUp(void);				// Move cursor up by windowheight
	bool MovePageDown(void);			// Move cursor down by windowheight
	bool MoveNextWord(void);			// Move to last word for Ctrl-Left
	bool MoveLastword(void);			// Move to last word for Ctrl-Right
	bool MoveDocStart(void);			// Move to the beginning of the document
	bool MoveDocEnd(void);				// Move to the end of the document
	bool MoveUpScreenLine(void);		// Move the screen up 1 line
	bool MoveDownScreenLine(void);		// Move the screen down by 1 line


	bool SetFontSize(int width,int height);		// Update the font dimensions
	bool SetScreenSize(int width,int height);	// Update Screen Dimensions
	bool SetTabSize(int size);					// Update the tabsize

	// Accessor Functions
	int GetFontCharHeight(void);	// Get Font Char Height in pixels
	int GetFontCharWidth(void);		// Get Font Char Width in pixels
	int GetScreenWidth(void);	// Get Screen Width
	int GetScreenHeight(void);	// Get Screen Height
	int GetWindowRows(void);	// Get Window Row Count
	int GetWindowCols(void);	// Get Window Column count
	int GetCaretRow(void);		// Get Caret Row Position in window
	int GetCaretCol(void);		// Get Caret Column position in window
	int GetTabSize(void);		// Get Current Tab Size
	int GetTopRow(void);		// Get Top Row of Window
	int GetTopCol(void);		// Get Top Left Column of window
	int GetEditLine(void);		// return the currently line where editing is being done
	int GetEditOffset(void);	// Get element offset of Caret
	bool IsScreenUpdated(void);	// Check is screen requires an whole redraw
	bool ResetScreenUpdated(void);	// Call after screen has been redrawn
	bool IsLineUpdated(void);		// Check if line should be redrawn
	bool ResetLineUpdated(void);  // Call after line has been redrawn

	// Text Operations
	bool Insert(char ch,int &retrow,int &retoffset); // Insert ch at current position
	bool OverWrite(char ch);	// Overwrite current position with char
	bool Delete(char &delchar,int &delrow,int &deloffset);	// Delete Current Character
	bool NewLine(int &row,int &offset);	// Pressed Enter at current point
	bool BackSpace(char &delch,int &row,int &offset);	// Pressed back space at current point
	bool SetCursor(int pixelrow,int pixelcolumn);  // Set pixelrow/pixelcolumn
	bool SetEditPosition(int lineno,int charoffset); // Set New Edit Position
	bool PasteText(const char *text);  // Paste text at current position
	bool CutText(const char *text);  // Uses text as guide to cut text from current position
	bool LoadTextFile(const char *filename);	// Read into buffer the text file
	bool SaveTextFile(const char *filename);	// Save buffer to text file
	};

#endif // MWindowTracker_h

