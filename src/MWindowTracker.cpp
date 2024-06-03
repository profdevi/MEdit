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
#include <windows.h>
#include "MStdLib.h"
#include "MEditBuffer.h"
#include "MWindowTracker.h"


//******************************************************
//**  Module Elements
//******************************************************
static bool GIsSymbol(unsigned short ch)
	{
	if(isalnum(ch)!=0 || ch=='_') { return true; }
	return false;
	}


///////////////////////////////////////////////////////////////
static bool GIsDisplayable(char ch)  // Checks if ch is displayable
	{
	if(ch>=32 && ch<128) { return true; }
	if(ch=='\t' || ch=='\n') { return true; }
	return false;
	}

//******************************************************
//**  MWindowTracker class
//******************************************************
void MWindowTracker::ClearObject(void)
	{
	mBuffer=NULL;			// Holds a reference to buffer
	mScreenWidth=0;			// Holds the Screen Width
	mScreenHeight=0;		// Holds the Screen Height
	mCarotRow=0;			// Location of the Carot Row
	mCarotCol=0;			// Location of the Carot Col
	mWindowRows=0;			// Calculated # of rows in window
	mWindowCols=0;			// Calculated # of cols in window
	mTopRow=0;				// Window Origin Row
	mTopCol=0;				// Window Origin Col
	mFontCharWidth=1;		// Font Cell width
	mFontCharHeight=1;		// Font Cell height
	mTabSize=4;				// reference to tab size
	mEditLine=0;			// Holds position of where the edit point is
	mEditDistance=0;		// Holds the real left distance of edit point
	mScreenUpdate=false;	// =true if whole window should be redrawn
	mLineUpdate=false;		// =true if line should be updated	
	}


//////////////////////////////////////////////////////////
int MWindowTracker::GetLeftDistance(int lineno)
	{
	int linecount=mBuffer->GetRows();
	if(lineno==linecount) { return 0; }

	int linelength=mBuffer->GetCols(lineno);
	if(linelength==0) { return 0; }
	
	MEditBufferElement *elements=mBuffer->Get(lineno,0);

	int distance=0;
	for(int i=0;i<linelength;++i)
		{
		if(elements[i].Value=='\t')
			{  distance += mTabSize;  }
		else
			{  distance += 1; }
		}

	return distance;	
	}


///////////////////////////////////////////////////////////
int MWindowTracker::GetLeftDistance(int lineno,int elementpos)
	{
	int linecount=mBuffer->GetRows();
	if(lineno==linecount) { return 0; }

	int linelength=mBuffer->GetCols(lineno);
	if(linelength==0) { return 0; }

	MEditBufferElement *elements=mBuffer->Get(lineno,0);

	int distance=0;
	for(int i=0;i<linelength && i<elementpos;++i)
		{
		if(elements[i].Value=='\t')
			{  distance += mTabSize;  }
		else
			{  distance += 1; }
		}

	return distance;
	}


////////////////////////////////////////////////////////
int MWindowTracker::GetElmentLeftDistance(int lineno,int elementdistance)
	{
	int linecount=mBuffer->GetRows();
	if(lineno==linecount) { return 0; }
	
	int linelength=mBuffer->GetCols(lineno);
	if(linelength==0) { return 0; }

	MEditBufferElement *elements=mBuffer->Get(lineno,0);

	int distance=0;  int tmpdist;
	for(int i=0;i<linelength;++i)
		{
		if(elements[i].Value=='\t')
			{ tmpdist=distance+mTabSize; }
		else
			{ tmpdist=distance+1; }

		if(tmpdist>elementdistance) { return distance; }

		distance=tmpdist;
		}

	return distance;
	}


///////////////////////////////////////////////////////
int MWindowTracker::GetNextElementLeftDistance(int lineno,int elementdistance)
	{
	int linecount=mBuffer->GetRows();
	if(lineno==linecount) { return 0; }
	
	int linelength=mBuffer->GetCols(lineno);
	if(linelength==0) { return 0; }

	MEditBufferElement *elements=mBuffer->Get(lineno,0);

	int distance=0;
	for(int i=0;i<linelength;++i)
		{
		if(elements[i].Value=='\t')
			{ distance += mTabSize; }
		else
			{ distance += 1; }

		if(distance>elementdistance) { return distance; }
		}

	return distance;
	}


//////////////////////////////////////////////////////
int MWindowTracker::GetPrevElementLeftDistance(int lineno,int elementdistance)
	{
	int linecount=mBuffer->GetRows();
	if(lineno==linecount) { return 0; }
	
	int linelength=mBuffer->GetCols(lineno);
	if(linelength==0) { return 0; }

	MEditBufferElement *elements=mBuffer->Get(lineno,0);

	int distance=0;  int tmpdist;
	for(int i=0;i<linelength;++i)
		{
		if(elements[i].Value=='\t')
			{ tmpdist=distance+mTabSize; }
		else
			{ tmpdist=distance+1; }

		if(tmpdist>=elementdistance) { return distance; }

		distance=tmpdist;
		}

	return distance;
	}


/////////////////////////////////////////////////
bool MWindowTracker::UpdateWindowPosition(void)
	{
	// Find the row position
	if(mEditLine<mTopRow)
		{
		mTopRow=mEditLine;
		mScreenUpdate=true;
		}
	else if(mEditLine>mTopRow+mWindowRows-2)
		{
		mTopRow=mEditLine-mWindowRows+2;
		mScreenUpdate=true;
		}

	// Update the column position
	int elementleftdistance=GetElmentLeftDistance(mEditLine,mEditDistance);
	if(elementleftdistance<mTopCol)
		{
		mScreenUpdate=true;
		mTopCol=elementleftdistance;
		}
	else if(elementleftdistance>mTopCol+mWindowCols-mTabSize)
		{
		mScreenUpdate=true;
		mTopCol=(elementleftdistance/mWindowCols)*mWindowCols;
		if(elementleftdistance>mTopCol+mWindowCols-mTabSize)
			{  mTopCol+=mTabSize; }
		}

	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::UpdateCaretPosition(void)
	{
	mCarotRow=mEditLine-mTopRow;
	
	int maxleftdistance=GetLeftDistance(mEditLine);

	if(maxleftdistance<=mEditDistance)
		{  mCarotCol=maxleftdistance-mTopCol; return true;  }

	int distance=GetElmentLeftDistance(mEditLine,mEditDistance);
	mCarotCol=distance-mTopCol;
	return true;
	}


////////////////////////////////////////////////
int MWindowTracker::GetOffset(int lineno,int leftdistance)
	{
	int linecount=mBuffer->GetRows();
	if(lineno<0 || lineno>=linecount) { return 0; }
	int linelength=mBuffer->GetCols(lineno);
	if(linelength==0) { return 0; }	

	MEditBufferElement *line=mBuffer->Get(lineno,0);
	int position=0;

	for(int i=0;i<linelength;++i)
		{
		if(line[i].Value!='\t')
			{  position+=1; }
		else
			{  position+=mTabSize; }

		if(position>leftdistance) { return i; }
		}

	return linelength;
	}


////////////////////////////////////////////////
MWindowTracker::MWindowTracker(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWindowTracker::~MWindowTracker(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWindowTracker::Create(MEditBuffer &buffer)
	{
	Destroy();
	mBuffer=&buffer;

	// Set Tab Size to default
	mTabSize=4;

	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::Destroy(void)
	{
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveUp(void)					// Move up cursor by one line
	{
	if(mEditLine==0) { return false; }

	mEditLine=mEditLine-1;
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveDown(void)				// Move down cursor by one line
	{
	int linecount=mBuffer->GetRows();
	if(mEditLine==linecount) { return false; }
	mEditLine=mEditLine+1;
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveLeft(void)				// Move cursor left by 1 position
	{
	if(mEditDistance==0) { return false; }
	mEditDistance=GetPrevElementLeftDistance(mEditLine,mEditDistance);
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveRight(void)				// Move cursor lright by 1 character
	{
	mEditDistance=GetNextElementLeftDistance(mEditLine,mEditDistance);
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveLineBegin(void)			// Move cursor to beginning of line
	{
	mEditDistance=0;
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveLineEnd(void)				// Move cursor to end of line
	{
	mEditDistance=GetLeftDistance(mEditLine);
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;	
	}


////////////////////////////////////////////////
bool MWindowTracker::MovePageUp(void)				// Move cursor up by windowheight
	{
	if(mTopRow>=mWindowRows)
		{
		mTopRow -= mWindowRows;
		mEditLine -=mWindowRows;
		mScreenUpdate=true;
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}

	//=top is withing 1 window hieght from top
	if(mTopRow>0)
		{
		mEditLine -=mTopRow;
		mTopRow=0;
		mScreenUpdate=true;
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}

	//= mTopRow=0

	//Do nothing if already at first line
	if(mEditLine==0) { return false; }

	mEditLine=0;
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MovePageDown(void)			// Move cursor down by windowheight
	{
	int linecount=mBuffer->GetRows();
	if(linecount==0) { return false; }

	// Still More Space availible
	if(mEditLine+mWindowRows<linecount)
		{
		mEditLine += mWindowRows;
		mTopRow += mWindowRows;
		mScreenUpdate=true;
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}

	//=mEditLine is within a window height of last line

	if(linecount<mTopRow+mWindowRows-1)
		{
		//=Last line is in current window view
		mEditLine=linecount;
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}

	mEditLine=linecount;
	if(mTopRow+mWindowRows<linecount)
		{ mTopRow+=mWindowRows; mScreenUpdate=true; }
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveNextWord(void)			// Move to last word for Ctrl-Left
	{
	int offset=GetEditOffset();
	int linecount=mBuffer->GetRows();

	if(mEditLine==linecount) { return false; }

	int linelength=mBuffer->GetCols(mEditLine);

	// if already at end of line, stop
	if(offset==linelength || offset+1==linelength) { return false; }

	MEditBufferElement *elements=mBuffer->Get(mEditLine,0);
	for(int i=1;offset+i<linelength;++i)
		{
		if(GIsSymbol(elements[offset].Value)==GIsSymbol(elements[offset+i].Value) )
			{ continue; }
		//=found a different element
		mEditDistance=GetLeftDistance(mEditLine,offset+i);
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}
	
	
	//=No Next Word

	//=Move caret to end of line
	mEditDistance=GetLeftDistance(mEditLine);
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////
bool MWindowTracker::MoveLastword(void)			// Move to last word for Ctrl-Right
	{
	int offset=GetEditOffset();
	int linecount=mBuffer->GetRows();

	if(mEditLine==linecount) { return false; }

	int linelength=mBuffer->GetCols(mEditLine);

	// if already at end of line, stop
	if(offset==0) { return false; }

	// Second character
	if(offset==1)
		{
		mEditDistance=0;
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;		
		}

	MEditBufferElement *elements=mBuffer->Get(mEditLine,0);
	for(int i=1;offset-i>=0;++i)
		{
		if(GIsSymbol(elements[offset-1].Value)==GIsSymbol(elements[offset-i].Value) )
			{ continue; }
		//=found a different element
		mEditDistance=GetLeftDistance(mEditLine,offset-i+1);
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}


	//=No Last Word

	//=Move caret to end of line
	mEditDistance=0;
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


//////////////////////////////////////////////////
bool MWindowTracker::MoveDocStart(void)		// Move to the beginning of the document
	{
	if(mEditLine==0 && mEditDistance==0) { return false; }
	mEditLine=0;  mEditDistance=0;
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


//////////////////////////////////////////////////
bool MWindowTracker::MoveDocEnd(void)		// Move to the end of the document
	{
	int linecount=mBuffer->GetRows();
	if(linecount==mEditLine && mEditDistance==0) { return false; }
	mEditLine=linecount;  mEditDistance=0;
	UpdateWindowPosition();
	UpdateCaretPosition();	
	return true;
	}


/////////////////////////////////////////////////
bool MWindowTracker::MoveUpScreenLine(void)		// Shift the screen up 1 line
	{
	int linecount=mBuffer->GetRows();
	if(mTopRow==0) { return false; }
	mTopRow -= 1; mScreenUpdate=true;
	if(mEditLine>=mTopRow+mWindowRows-2) { mEditLine-=1; }
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


/////////////////////////////////////////////////
bool MWindowTracker::MoveDownScreenLine(void)		// Shift the screen down by 1 line
	{
	int linecount=mBuffer->GetRows();
	if(mTopRow+mWindowRows>=linecount) { return false; }
	mTopRow=mTopRow+1;  mScreenUpdate=true;
	if(mEditLine<mTopRow) { mEditLine+=1; }
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}

////////////////////////////////////////////////
bool MWindowTracker::SetFontSize(int width,int height)	// Update the font dimensions
	{
	// Update Font Data
	mFontCharWidth=width;  mFontCharHeight=height;

	//update Window Dimensions
	mWindowRows=mScreenHeight/mFontCharHeight+1;
	mWindowCols=mScreenWidth/mFontCharWidth+1;
	
	return true;
	}


//////////////////////////////////////////////////
bool MWindowTracker::SetScreenSize(int width,int height)
	{
	// Update screen dimensions
 	mScreenWidth=width;  mScreenHeight=height;

	// Update Window Dimensions
	mWindowRows=mScreenHeight/mFontCharHeight+1;
	mWindowCols=mScreenWidth/mFontCharWidth+1;

	// Screen rows is never smaller then mWindowRows
	if(mWindowRows<2*mTabSize) { mWindowRows=2*mTabSize; }

	return true;
	}


///////////////////////////////////////////////////////////////
bool MWindowTracker::SetTabSize(int size)			// Update the tabsize
	{
	if(size<1)
		{
		return false;
		}

	// Stay on the current line and update distance
	mTabSize=size; mEditDistance=0;
	if(mWindowRows<2*mTabSize) { mWindowRows=2*mTabSize; }

	return true;
	}


/////////////////////////////////////////////////////////
int MWindowTracker::GetFontCharHeight(void)
	{  return mFontCharHeight;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetFontCharWidth(void)
	{  return mFontCharWidth;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetScreenWidth(void)
	{  return mScreenWidth;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetScreenHeight(void)
	{  return mScreenHeight;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetWindowRows(void)
	{  return mWindowRows;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetWindowCols(void)
	{  return mWindowCols;  }


////////////////////////////////////////////////////////
int MWindowTracker::GetCaretRow(void)
	{ return mCarotRow; }


///////////////////////////////////////////////////////
int MWindowTracker::GetCaretCol(void)
	{ return mCarotCol; }


/////////////////////////////////////////////////////////
int MWindowTracker::GetTabSize(void)
	{  return mTabSize;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetTopRow(void)
	{  return mTopRow;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetTopCol(void)
	{  return mTopCol;  }


/////////////////////////////////////////////////////////
int MWindowTracker::GetEditLine(void)
	{  return mEditLine;  }


int MWindowTracker::GetEditOffset(void)
	{
	int linecount=mBuffer->GetRows();
	if(mEditLine==linecount) { return 0; }
	int linelength=mBuffer->GetCols(mEditLine);
	if(linelength==0) { return 0; }

	MEditBufferElement *elements=mBuffer->Get(mEditLine,0);

	int distance=0;  int leftdistance=mCarotCol+mTopCol;
	for(int i=0;i<linelength;++i)
		{
		if(distance==leftdistance) { return i; }
		else if(distance>leftdistance) { break; }

		if((elements+i)->Value=='\t')
			{  distance += mTabSize; }
		else
			{ distance += 1; }
		}

	return linelength;
	}
	
/////////////////////////////////////////////////////////
bool MWindowTracker::IsScreenUpdated(void)			// =true if screen requires an update
	{  return mScreenUpdate; }


/////////////////////////////////////////////////////////
bool MWindowTracker::ResetScreenUpdated(void)		// Called if the whole screen has been updated
	{
	mScreenUpdate=false;
	mLineUpdate=false;
	return true;
	}


////////////////////////////////////////////////////////
bool MWindowTracker::IsLineUpdated(void)				// Check if line should be redrawn
	{  return mLineUpdate;  }


////////////////////////////////////////////////////////
bool MWindowTracker::ResetLineUpdated(void)				// Call after line has been redrawn
	{
	mLineUpdate=false;
	return true;
	}


/////////////////////////////////////////////////////////
bool MWindowTracker::Insert(char ch,int &retrow,int &retoffset)
	{
	retrow=0; retoffset=NULL;
	if(ch<32 && ch!='\t' && ch!=' ') { return false; }

	mLineUpdate=true;
	int linecount=mBuffer->GetRows();

	// If Last Line append a new row
	if(mEditLine==linecount) {  mBuffer->AppendRow();  }
	int linelength=mBuffer->GetCols(mEditLine);

	int editoffset=GetEditOffset();
	if(editoffset==linelength)
		{
		if(mBuffer->AppendCell(mEditLine)==false)
			{
			return false;
			}
		}
	else
		{
		if(mBuffer->InsertCell(mEditLine,editoffset)==false)
			{
			return false;
			}
		}

	MEditBufferElement *element=mBuffer->Get(mEditLine,editoffset);
	element->Value=ch;

	//Set return elements
	retrow=mEditLine;  retoffset=editoffset;

	return MoveRight();
	}


//////////////////////////////////////////////////////
bool MWindowTracker::OverWrite(char ch)
	{
	if(ch<32 && ch!='\t' && ch!=' ') { return false; }
	mLineUpdate=true;
	int linecount=mBuffer->GetRows();

	// If Last Line append a new row
	if(mEditLine==linecount) {  mBuffer->AppendRow();  }
	int linelength=mBuffer->GetCols(mEditLine);

	int editoffset=GetEditOffset();
	if(editoffset==linelength)
		{
		if(mBuffer->AppendCell(mEditLine)==false)
			{
			return false;
			}
		}

	MEditBufferElement *element=mBuffer->Get(mEditLine,editoffset);
	element->Value=ch;
	return MoveRight();	
	}


///////////////////////////////////////////////////////
bool MWindowTracker::Delete(char &delchar,int &delrow,int &deloffset)  // Delete Current Character
	{
	mLineUpdate=true;
	int linecount=mBuffer->GetRows();

	// If Last Line do nothing
	if(mEditLine==linecount) {  return false; }

	int linelength=mBuffer->GetCols(mEditLine);

	int editoffset=GetEditOffset();
	if(editoffset==linelength) // Merge two lines
		{
		// update return info
		delchar='\n';  delrow=mEditLine;  deloffset=linelength;

		// Delete cell
		mBuffer->RemoveCell(mEditLine,editoffset);
		mScreenUpdate=true;  return true;
		}

	MEditBufferElement *element=mBuffer->Get(mEditLine,editoffset);

	// Get Return information	
	delchar=(char)element->Value;
	delrow=mEditLine;  deloffset=editoffset;

	// Delete cell
	mBuffer->RemoveCell(mEditLine,editoffset);
	return true;
	}


////////////////////////////////////////////////////
bool MWindowTracker::NewLine(int &retrow,int &retoffset)	// Pressed Enter at current point
	{
	mScreenUpdate=true;

	int editoffset=GetEditOffset();
	int ret=mBuffer->SplitLine(mEditLine,editoffset);
	if(ret==false) { return false; }

	// Update return position
	retrow=mEditLine;  retoffset=editoffset;

	mEditLine=mEditLine+1;  mEditDistance=0;
	UpdateWindowPosition();
	UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////////
bool MWindowTracker::BackSpace(char &delch,int &row,int &offset)  // Pressed back space at current point
	{
	int editoffset=GetEditOffset();

	delch=0; row=0; offset=0;

	// If Not at beginning of line
	if(editoffset>0)
		{
		// Copy deleted char and position
		MEditBufferElement *element=mBuffer->Get(mEditLine,editoffset-1);

		// Update line
		mLineUpdate=true;

		delch=(char)element->Value;  row=mEditLine; offset=editoffset-1;
		mBuffer->RemoveCell(mEditLine,editoffset-1);


		mEditDistance=GetLeftDistance(mEditLine,editoffset-1);
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}

	//=at the begining of a line

	// if at first line, do nothing
	if(mEditLine==0) { return false; }

	int linecount=mBuffer->GetRows();

	//if at last line, do nothing
	if(linecount==mEditLine) { return false; }
	
	// Move Up to prior line
	mEditLine -= 1;

	
	int linelength=mBuffer->GetCols(mEditLine);

	// return position
	delch='\n';  row=mEditLine;  offset=linelength;

	// Move to end of current line
	mEditDistance=GetLeftDistance(mEditLine,linelength);
	
	// Append Contents of prior line to current line
	mBuffer->AppendRow(mEditLine,mEditLine+1);
	
	// Delete the last line
	mBuffer->RemoveRow(mEditLine+1);

	// Update Window/Caret positions
	UpdateWindowPosition();
	UpdateCaretPosition();
	
	mScreenUpdate=true;  return true;
	}


//////////////////////////////////////////////////////////
bool MWindowTracker::SetCursor(int pixelrow,int pixelcolumn)	// Set pixelrow/pixelcolumn
	{
	int suggestrow=mTopRow+pixelrow/mFontCharHeight;
	int suggestcol=mTopCol+pixelcolumn/mFontCharWidth;

	int linecount=mBuffer->GetRows();
	if(suggestrow<0) { suggestrow=0;  suggestcol=0; }
	else if(suggestrow>=linecount) { suggestrow=linecount;  suggestcol=0; }

	// If Last Line
	if(suggestrow==linecount)
		{
		mEditLine=suggestrow;
		mEditDistance=0;
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}

	mEditDistance=GetLeftDistance(suggestrow,GetOffset(suggestrow,suggestcol));
	mEditLine=suggestrow;
	
	UpdateWindowPosition();
	UpdateCaretPosition();

	return false;
	}


////////////////////////////////////////////////////////////
bool MWindowTracker::SetEditPosition(int lineno,int charoffset) // Set New Edit Position
	{
	int linecount=mBuffer->GetRows();
	if(lineno<0)
		{ lineno=0; }
	else if(lineno>linecount)
		{ lineno=linecount;  charoffset=0; }

	if(lineno==linecount)
		{
		mEditLine=linecount;  mEditDistance=0;
		UpdateWindowPosition();
		UpdateCaretPosition();
		return true;
		}

	mEditLine=lineno;  mEditDistance=GetLeftDistance(mEditLine,charoffset);
	UpdateWindowPosition();  UpdateCaretPosition();
	return true;
	}


////////////////////////////////////////////////////////////
bool MWindowTracker::PasteText(const char *text)
	{
	int editline=mEditLine;
	int editoffset=GetEditOffset();

	MEditBufferElement *element;
	for(int i=0;text[i]!=0;++i)
		{
		char ch=text[i];
		if(ch!='\n')
			{
			mBuffer->InsertCell(editline,editoffset);
			element=mBuffer->Get(editline,editoffset);
			element->Value=ch; editoffset += 1;
			mLineUpdate=true;
			}
		else
			{	//=Got a new line character
			mBuffer->SplitLine(editline,editoffset);
			editline+=1;  editoffset=0; mScreenUpdate=true;
			}
		}

	UpdateWindowPosition();  UpdateCaretPosition();
	return true;
	}


//////////////////////////////////////////////////////////////
bool MWindowTracker::CutText(const char *text)
	{
	int editline=mEditLine;
	int editoffset=GetEditOffset();

	for(int i=0;text[i]!=0;++i)
		{
		char ch=text[i];
		if(ch!='\n')
			{ //=Regular character
			// Check if text data matches position data
			mBuffer->RemoveCell(editline,editoffset);
			}
		else
			{ //=New Line char '\n'
			mBuffer->AppendRow(editline,editline+1);
			mBuffer->RemoveRow(editline+1);
			mScreenUpdate=true; // We Need to update the screen
			}
		}

	UpdateWindowPosition();  UpdateCaretPosition();
	return true;
	}


//////////////////////////////////////////////////////////////
bool MWindowTracker::LoadTextFile(const char *filename)	// Read into buffer the text file
	{
	MStdFileHandle file=MStdFileOpen(filename,"r");
	if(file==NULL)
		{
		return false;
		}

	// Load Text File Data from current position
	int editline=mEditLine;
	int editoffset=GetEditOffset();	
	
	char ch;
	MEditBufferElement *element;
	while(MStdFileRead(file,&ch,sizeof(ch),1)>0)
		{
		if(GIsDisplayable(ch)==false) { continue; }

		if(ch!='\n')
			{
			mBuffer->InsertCell(editline,editoffset);
			element=mBuffer->Get(editline,editoffset);
			element->Value=ch; editoffset += 1;
			mLineUpdate=true;
			}
		else
			{	//=Got a new line character
			mBuffer->SplitLine(editline,editoffset);
			editline+=1;  editoffset=0; mScreenUpdate=true;
			}
		}

	MStdFileClose(file);  //close file

	//Update screen info
	UpdateWindowPosition();  UpdateCaretPosition();
	return true;
	}


//////////////////////////////////////////////////////////////
bool MWindowTracker::SaveTextFile(const char *filename)	// Save buffer to text file
	{
	//Open file for read
	MStdFileHandle file=MStdFileOpen(filename,"wb");
	if(file==NULL)
		{
		return false;
		}
	
	int linecount=mBuffer->GetRows();
	// Write each row to file
	for(int r=0;r<linecount;++r)
		{
		// Write each line character to file
		int colcount=mBuffer->GetCols(r);
		for(int c=0;c<colcount;++c)
			{
			MEditBufferElement *element=mBuffer->Get(r,c);
			if(element==NULL)
				{
				continue;
				}

			MStdFileWrite(file,&(element->Value),1,1);
			}

		// Add a new line only if there is a next line
		if(r+1<linecount)
			{
			MStdFileWrite(file,"\r\n",1,2);
			}
		}

	MStdFileClose(file);
	return true;
	}

