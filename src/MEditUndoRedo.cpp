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


//v2.0 copyright Comine.com 20230602F2200
#include "MStdLib.h"
#include "MUndoStack.h"
#include "MEditUndoRedo.h"



//************************************************************
//** MEditUndoRedo Implementation
//************************************************************
void MEditUndoRedo::ClearObject(void)
	{
	}


////////////////////////////////////////////////////////////
MEditUndoRedo::MEditUndoRedo(void)
	{  ClearObject();  }


////////////////////////////////////////////////////////////
MEditUndoRedo::~MEditUndoRedo(void)
	{ Destroy(); }


////////////////////////////////////////////////////////////
bool MEditUndoRedo::Create(void)
	{
	Destroy();

	if(mUndoStack.Create()==false)
		{
		return false;
		}

	if(mRedoStack.Create()==false)
		{
		Destroy();  return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::Destroy(void)
	{
	mUndoStack.Destroy();
	mRedoStack.Destroy();

	ClearObject();
	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::InsertChar(char oldch,int row,int col)
	{
	// Empty out the redo data
	if(mRedoStack.Create()==false)
		{
		return false;
		}

	if(mUndoStack.InsertChar(oldch,row,col)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::OverWriteChar(char oldch,char newch,int row,int col)
	{
	// Empty out the redo data
	if(mRedoStack.Create()==false)
		{
		return false;
		}

	if(mUndoStack.OverWriteChar(oldch,newch,row,col)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::DeleteChar(char ch,int row,int col)
	{
	// Empty out the redo data
	if(mRedoStack.Create()==false)
		{
		return false;
		}

	if(mUndoStack.DelChar(ch,row,col)==false)
		{
		return false;
		}
	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::CutText(const char *text,int row,int col)
	{
	// Empty out the redo data
	if(mRedoStack.Create()==false)
		{
		return false;
		}

	if(mUndoStack.CutText(text,row,col)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::PasteText(const char *text,int row,int col)
	{
	// Empty out the redo data
	if(mRedoStack.Create()==false)
		{
		return false;
		}

	if(mUndoStack.PasteText(text,row,col)==false)
		{
		return false;
		}
	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::IsUndoPossible(void)
	{
	if(mUndoStack.IsEmpty()==true) { return false; }
	return true;
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::IsRedoPossible(void)
	{
	if(mRedoStack.IsEmpty()==true) { return false; }
	return true;
	}


/////////////////////////////////////////////////////////////
const char *MEditUndoRedo::GetUndoString(void)			// Get Undo String
	{
	if(mUndoStack.IsEmpty()==true)
		{
		return "";
		}

	return mUndoStack.GetText();
	}

/////////////////////////////////////////////////////////////
int MEditUndoRedo::GetUndoRow(void)						// Get Undo Row
	{
	if(mUndoStack.IsEmpty()==true)
		{
		return MUST_UNSET;
		}

	return mUndoStack.GetRow();
	}

/////////////////////////////////////////////////////////////
int MEditUndoRedo::GetUndoColumn(void)
	{
	if(mUndoStack.IsEmpty()==true)
		{
		return 0;
		}

	return mUndoStack.GetColumn();
	}

////////////////////////////////////////////////////////////
MUndoStackType MEditUndoRedo::GetUndoType(void)	// Get Undo String
	{
	if(mUndoStack.IsEmpty()==true)
		{
		return MUST_UNSET;
		}

	return mUndoStack.GetType();
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::Undo(void)
	{
	if(IsUndoPossible()==false) { return false; }

	int row=mUndoStack.GetRow();
	int col=mUndoStack.GetColumn();
	MUndoStackType type=mUndoStack.GetType();

	if(type==MUST_UNSET)
		{
		return false;
		}

	const char *datapointer=mUndoStack.GetText();
	if(datapointer==NULL)
		{
		return false;
		}

	int count=MStdStrLen(datapointer);

	if(type==MUST_INSERT)
		{
		for(int i=0;i<count;++i)
			{  mRedoStack.InsertChar(datapointer[i],row,col+i); }
		mUndoStack.Pop();  return true;
		}
	else if(type==MUST_DELETE)
		{
		for(int i=0;i<count;++i)
			{  mRedoStack.DelChar(datapointer[i],row,col); }
		mUndoStack.Pop();  return true;
		}
	else if(type==MUST_OVERWRITE)
		{
		mRedoStack.OverWriteChar(datapointer[0],datapointer[1],row,col);
		mUndoStack.Pop(); return true;
		}
	else if(type==MUST_CUT)
		{
		mRedoStack.CutText(datapointer,row,col);
		mUndoStack.Pop();  return true;
		}
	else if(type==MUST_PASTE)
		{
		mRedoStack.PasteText(datapointer,row,col);
		mUndoStack.Pop();  return true;
		}
	else
		{
		return false;
		}
	}


////////////////////////////////////////////////////////////
bool MEditUndoRedo::Redo(void)
	{
	if(IsRedoPossible()==false) { return false; }

	MUndoStackType type=mRedoStack.GetType();
	int row=mRedoStack.GetRow();
	int col=mRedoStack.GetColumn();
	

	if(type==MUST_UNSET)
		{
		return false;
		}

	const char *datapointer=mRedoStack.GetText();
	if(datapointer==NULL)
		{
		return false;
		}

	int count=MStdStrLen(datapointer);
	if(type==MUST_INSERT)
		{
		for(int i=0;i<count;++i)
			{  mUndoStack.InsertChar(datapointer[i],row,col+i); }

		mRedoStack.Pop();  return true;
		}
	else if(type==MUST_DELETE)
		{
		for(int i=0;i<count;++i)
			{  mUndoStack.DelChar(datapointer[i],row,col); }

		mRedoStack.Pop();  return true;
		}
	else if(type==MUST_OVERWRITE)
		{
		mUndoStack.OverWriteChar(datapointer[0],datapointer[1],row,col);
		mRedoStack.Pop();  return true;
		}
	else if(type==MUST_CUT)
		{
		mUndoStack.CutText(datapointer,row,col);
		mRedoStack.Pop();  return true;
		}
	else if(type==MUST_PASTE)
		{
		mUndoStack.PasteText(datapointer,row,col);
		mRedoStack.Pop();  return true;
		}
	else
		{
		return false;
		}	
	}

