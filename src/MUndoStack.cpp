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
#include "MWordGrouper.h"
#include "MEditUndoDataStack.h"
#include "MUndoStack.h"


//*****************************************************************
//** Module Elements
//*****************************************************************


//*****************************************************************
//** MUndoStack Implementation
//*****************************************************************
void MUndoStack::ClearObject(void)
	{
	}


///////////////////////////////////////////////////////////////////////
void MUndoStack::OnFlushHandler(void *objectarg
		,int row,int col,const char *itemarray,MWordGrouperType type)
	{
	MUndoStack *object=(MUndoStack*)objectarg;
	object->OnFlush(row,col,itemarray,type);
	}


//////////////////////////////////////////////////////////////////////
void MUndoStack::OnFlush(int row,int col,const char *itemarray
			,MWordGrouperType type)
	{
	if(mUndoStack.Push(itemarray,row,col,type)==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////////////////////////
MUndoStack::MUndoStack(void)
	{  ClearObject();  }

//////////////////////////////////////////////////////////////////////
MUndoStack::~MUndoStack(void)
	{  Destroy();  }


//////////////////////////////////////////////////////////////////////
bool MUndoStack::Create(void)
	{
	Destroy();
	if(mGrouper.Create(MUndoStack::OnFlushHandler,this)==false)
		{
		Destroy();  return false;
		}

	if(mUndoStack.Create()==false)
		{
		Destroy();
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::Destroy(void)
	{
	mUndoStack.Destroy();
	mGrouper.Destroy();
	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::Pop(void)  // Pops Last Action
	{
	mGrouper.FlushData();  // Make Sure Data is Flushed
	if(mUndoStack.IsEmpty()==true) { return false; }
	
	if(mUndoStack.Pop()==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::InsertChar(char ch,int line,int pos)
	{
	return mGrouper.InsertChar(line,pos,ch);
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::OverWriteChar(char oldch,char newchar,int line,int pos)
	{
	return mGrouper.OverWriteChar(line,pos,oldch,newchar);
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::DelChar(char oldch,int line,int pos)
	{
	return mGrouper.DeleteChar(line,pos,oldch);
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::CutText(const char *data,int line,int pos)
	{
	return mGrouper.CutText(line,pos,data);
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::PasteText(const char *data,int line,int pos)
	{
	return mGrouper.PasteText(line,pos,data);
	}


//////////////////////////////////////////////////////////////////////
int MUndoStack::GetRow(void)
	{
	return mUndoStack.GetRow();
	}


//////////////////////////////////////////////////////////////////////
int MUndoStack::GetColumn(void)
	{
	return mUndoStack.GetColumn();
	}


//////////////////////////////////////////////////////////////////////
MUndoStackType MUndoStack::GetType(void)
	{
	int rettype=mUndoStack.GetType();
	if(rettype==MWGT_UNSET) { return MUST_UNSET; }
	else if(rettype==MWGT_INSERT) { return MUST_INSERT; }
	else if(rettype==MWGT_DELETE) { return MUST_DELETE; }
	else if(rettype==MWGT_OVERWRITE) { return MUST_OVERWRITE; }
	else if(rettype==MWGT_PASTE) { return MUST_PASTE; }
	else if(rettype==MWGT_CUT) { return MUST_CUT; }
	
	return MUST_UNSET; // return the unset type
	}


///////////////////////////////////////////////////////////////////////
const char *MUndoStack::GetText(void)
	{
	return mUndoStack.GetString();
	}


//////////////////////////////////////////////////////////////////////
bool MUndoStack::IsEmpty(void)
	{
	mGrouper.FlushData();
	return mUndoStack.IsEmpty();
	}

