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
#ifndef MUndoStack_h
#define MUndoStack_h

/////////////////////////////////////////////////////////
#include "MStdLib.h"
#include "MWordGrouper.h"
#include "MEditUndoDataStack.h"

/////////////////////////////////////////////////////////
enum MUndoStackType
	{
	MUST_UNSET=0,
	MUST_INSERT=1,
	MUST_DELETE=2,
	MUST_OVERWRITE=3,
	MUST_CUT=4,
	MUST_PASTE=5
	};


/////////////////////////////////////////////////////////
class MUndoStack
	{
	MEditUndoDataStack mUndoStack;
	MWordGrouper mGrouper;					// Performs grouping of text

	//////////////////////////////////////////////////////
	void ClearObject(void);
	static void OnFlushHandler(void *objectarg
			,int row,int col,const char *itemarray,MWordGrouperType type);
	void OnFlush(int row,int col,const char *itemarray,MWordGrouperType type);

	//////////////////////////////////////////////////////
	public:
	MUndoStack(void);
	~MUndoStack(void);
	bool Create(void);
	bool Destroy(void);
	bool Pop(void);  // Pops Last Action
	bool InsertChar(char ch,int line,int pos);
	bool OverWriteChar(char oldch,char newchar,int line,int pos);
	bool DelChar(char oldch,int line,int pos);
	bool CutText(const char *data,int line,int pos);
	bool PasteText(const char *data,int line,int pos);

	///// Get Top Item ////////////////////////////////
	int GetRow(void);				// return row of current top item
	int GetColumn(void);			// return column of current top item
	MUndoStackType GetType(void);	// return type of current top item
	const char *GetText(void);		// return string of current top item
	bool IsEmpty(void);				// check if undo stack is empty
	};

#endif // MUndoStack_h