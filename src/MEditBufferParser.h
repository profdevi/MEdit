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


//v1.8 copyright Comine.com 20240603M1043
#ifndef MEditBufferParser_h
#define MEditBufferParser_h

///////////////////////////////////////////////////
#include "MILexer.h"
#include "MPointerStack.h"
#include "MStringBuffer.h"
#include "MLangWordLists.h"

//******************************************************
//**  MEditBufferParser class
//**  Class parses an MEditBuffer and sets only the first 4 bits
//**  of the MEditBufferElement values to a specific type
//**  If the type is zero, then the parser traverses the changes
//******************************************************
class MEditBuffer;
struct MEditBufferElement;
enum GState;


////////////////////////////////////////////////////
enum MEditBufferParserType
	{
	MEBT_UNSET=0,				// Text Type has never been set
	MEBPT_SYMBOL=1,
	MEBPT_OPERATOR=2,
	MEBPT_DQSTRING=3,
	MEBPT_SQSTRING=4,
	MEBPT_COMMENT=5,
	MEBPT_KEYWORD=6,
	MEBPT_SPACE=7,
	MEBPT_ERROR=8,
	MEBPT_LINECOMMENT=9
	};


////////////////////////////////////////////////////
class MEditBufferParser:public MILexer
	{
	MEditBuffer *mBuffer;			// reference to MEditBuffer
	GState mState;					// Holds Current State of system
	bool mScreenUpdate;				// =true if screen should be updated
	bool mTypeChanged;				// =true if during parse, the elment type changes

	MPointerStack mStack;			// Holds a buffer of elements for setting type
	MStringBuffer mSymbol;			// Used to build up symbols
	MLangWordLists mKeywordList;	// Holds the keywords of a language

	////////////////////////////////////////////////
	void ClearObject(void);
	bool MoveNextState(int nextchar,MEditBufferElement *element);	// State Transition Heart
	bool SetState(GState state);		// Set the current state of parser
	bool ProcessStack(MEditBufferParserType type);  // Set the type for elements on stack
	bool ProcessSymbol(void);				// Processs symbol on Stack
	bool IsLastLineInComment(int lineno);	// =true if last line is part of a comment	

	////////////////////////////////////////////////
	public:
	MEditBufferParser(void);
	~MEditBufferParser(void);
	bool Create(MEditBuffer &ref,const char *language="C++");
	bool Destroy(void);					// Destroy current object
	bool ParseAll(void);				// Parse whole buffer
	bool Parse(int lineno);				// Parse only starting at current line
	bool IsScreenUpdated(void);			// Check if whole screen need update
	};

#endif // MEditBufferParser_h

