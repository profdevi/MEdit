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
#ifndef MWordGrouper_h
#define MWordGrouper_h

//*****************************************
//*	This implementation will break apart continuous letters/spaces/not(space+letter)
//*	groupings.
//*****************************************

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MStringBuffer.h"

/////////////////////////////////////////////////
enum MWordGrouperType
		{
		MWGT_UNSET=0,
		MWGT_INSERT=1,
		MWGT_DELETE=2,
		MWGT_OVERWRITE=3,
		MWGT_PASTE=4,
		MWGT_CUT=5
		};


//***************************************************************
//*  MWordGrouper Class
//***************************************************************
enum MWordGrouperGroupingType;
class MWordGrouper
	{
	MStringBuffer mBuffer;
	int mLine,mOffset;
	MWordGrouperType mType;
	MWordGrouperGroupingType mGrouping;
	void (*mOnFlush)(void *argument
			,int row,int col,const char *itemarray,MWordGrouperType type);	// Call back
	void *mOnFlushArgument;													// argument of 
	
	//////////////////////////////////////////////
	void ClearObject(void);
	void SetGrouping(char item);
	bool IsGroupingChanged(char item);
	
	//////////////////////////////////////////////
	public:
	MWordGrouper(void);									// Constructor
	~MWordGrouper(void);								// Destructor
	bool Create(void Flush(void *argument,int row,int col
				,const char *itemarray,MWordGrouperType type)
			,void *argument);							// Create state of object
	bool Destroy(void);									// Destroy object state
	bool InsertChar(int line,int offset,char ch);		// Insert a char at position
	bool DeleteChar(int line,int offset,char oldchar);	// Delete a char at poition
	bool OverWriteChar(int line,int offset,char oldchar,char newchar);	// OverWrite
	bool PasteText(int line,int offset,const char *textdata);	//Paste Text
	bool CutText(int line,int offset,const char *testdata);	// Cut Text
	bool FlushData(void);								// Flush Data to Force OnFlush callback
	bool Clear(void);									// Clear the buffer and state
	};


#endif // MWordGrouper_h