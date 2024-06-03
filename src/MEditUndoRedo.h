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
#ifndef MEditUndoRedo_h
#define MEditUndoRedo_h

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MUndoStack.h"

/////////////////////////////////////////////////
class MEditUndoRedo
	{
	MUndoStack mUndoStack;
	MUndoStack mRedoStack;

	/////////////////////////////////////////////
	void ClearObject(void);

	/////////////////////////////////////////////
	public:
	MEditUndoRedo(void);						// Call Constructor
	~MEditUndoRedo(void);						// Call Destructor
	bool Create(void);							// Create state of object
	bool Destroy(void);							// Destroy state of object
	bool InsertChar(char ch,int row,int col);					// Insert Char
	bool OverWriteChar(char oldch,char newchar,int row,int col);// OverWrite Char
	bool DeleteChar(char ch,int row,int col);			// Delete Char
	bool CutText(const char *text,int row,int col);		// Cut Text Operation
	bool PasteText(const char *text,int row,int col);	// Paste text Operation
	bool IsUndoPossible(void);					// Check if undo is possible
	bool IsRedoPossible(void);					// Check if redo is possible
	const char *GetUndoString(void);			// Get Undo String
	int GetUndoRow(void);						// Get Undo Row
	int GetUndoColumn(void);					// Get Undo Column
	MUndoStackType GetUndoType(void);			// Get Undo String
	bool Undo(void);							// Perform an Undo Operation
	bool Redo(void);							// Perform a Redo operation
	};


#endif // MEditUndoRedo_h