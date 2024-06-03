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
#include "MStdLib.h"
#include "MILexer.h"
#include "MEditBuffer.h"
#include "MPointerStack.h"
#include "MStringBuffer.h"
#include "MLangWordLists.h"
#include "MEditBufferParser.h"


//******************************************************
//** Module Elements
//******************************************************
static const int GMaxStackSize=512;
static const int GMaxSymbolSize=128;
enum GState
		{
		S_START=0,S_SLASH=1,S_SLASH_SLASH=2,S_SLASH_STAR=3
		,S_SLASH_STAR_STAR=4,S_DQUOTE=5,S_SQUOTE=6,S_SPACE=7
		,S_SYMBOL=8,S_DQUOTE_BACKSLASH=9,S_SQUOTE_BACKSLASH=10
		};


///////////////////////////////////////////////////////////////
static bool GIsSpace(char ch)
	{
	if(ch==' ' || ch=='\t')	{ return true; }
	return false;
	}


///////////////////////////////////////////////////////////////
static bool GIsSymbol(char ch)
	{
	if(ch>='A' && ch<='Z') { return true; }
	if(ch>='a' && ch<='z') { return true; }
	if(ch>='0' && ch<='9') { return true; }
	if(ch=='_') { return true; }
	return false;	
	}


//******************************************************
//**  MEditBufferParser class
//******************************************************
void MEditBufferParser::ClearObject(void)
	{
	mBuffer=NULL;
	mState=S_START;
	mScreenUpdate=false;
	mTypeChanged=false;
	}


////////////////////////////////////////////////////////////////
bool MEditBufferParser::MoveNextState(int inp,MEditBufferElement *element)
	{
	int state=mState;
	if(state==S_START)	// Start State ///////////////////////
		{
		if(inp=='/')
			{
			mStack.Push(element);
			SetState(S_SLASH);
			return true;
			}
		if(inp=='\"')
			{
			mStack.Push(element);
			SetState(S_DQUOTE);
			return true;
			}
		if(inp=='\'')
			{
			mStack.Push(element);
			SetState(S_SQUOTE);
			return true;
			}
		if(inp=='\n')
			{
			// New Line
			return true;
			}
		if(GIsSpace(inp)==true)
			{
			mStack.Push(element);
			SetState(S_SPACE);  return true;
			}
		if(GIsSymbol(inp)==true)
			{
			mStack.Push(element);
			SetState(S_SYMBOL);  return true;
			}
		
		element->SetTextType(MEBPT_OPERATOR);
		return true;
		}
	else if(state==S_SLASH) /////////////////////////////////////////////
		{
		if(inp=='/')
			{
			mStack.Push(element);
			SetState(S_SLASH_SLASH);  return true;
			}
		if(inp=='*')
			{
			mStack.Push(element);
			SetState(S_SLASH_STAR); return true;
			}

		ProcessStack(MEBPT_OPERATOR);
		SetState(S_START);  return false;
		}
	else if(state==S_SLASH_SLASH)
		{
		if(inp=='\n')
			{
			ProcessStack(MEBPT_LINECOMMENT);
			SetState(S_START);  return true;
			}
		
		mStack.Push(element);  return true;
		}
	else if(state==S_SLASH_STAR)
		{
		if(inp=='*')
			{
			mStack.Push(element);
			SetState(S_SLASH_STAR_STAR); return true;
			}
		if(inp=='\n')
			{  ProcessStack(MEBPT_COMMENT);  return true;  }

		mStack.Push(element);  return true;
		}
	else if(state==S_SLASH_STAR_STAR)
		{
		if(inp=='*')
			{
			mStack.Push(element);
			return true;
			}

		if(inp=='/')
			{
			mStack.Push(element);
			ProcessStack(MEBPT_COMMENT);
			SetState(S_START);  return true;
			}

		if(inp=='\n')
			{
			ProcessStack(MEBPT_COMMENT);
			SetState(S_SLASH_STAR);  return true;
			}

		mStack.Push(element);  SetState(S_SLASH_STAR);  return true;
		}
	else if(state==S_DQUOTE)
		{
		if(inp=='\"')
			{
			mStack.Push(element);  ProcessStack(MEBPT_DQSTRING);
			SetState(S_START);  return true;
			}
		if(inp=='\n' )  // INput has Mistake in double quotes!!!
			{
			ProcessStack(MEBPT_ERROR);
			SetState(S_START);   return true;
			}
		if(inp=='\\')
			{
			mStack.Push(element);
			SetState(S_DQUOTE_BACKSLASH); return true;
			}

		mStack.Push(element);  return true;
		}
	else if(state==S_SQUOTE)
		{
		if(inp=='\'')
			{
			mStack.Push(element);  ProcessStack(MEBPT_SQSTRING);
			SetState(S_START);  return true;
			}
		if(inp=='\n' )  // Input has Mistake in single quotes!!!
			{
			ProcessStack(MEBPT_ERROR);
			SetState(S_START);   return true;
			}

		if(inp=='\\')
			{
			mStack.Push(element);
			SetState(S_SQUOTE_BACKSLASH); return true;
			}
		mStack.Push(element);  return true;
		}
	else if(state==S_SPACE)
		{
		if(GIsSpace(inp)==true)
			{  mStack.Push(element);  return true; }

		ProcessStack(MEBPT_SPACE);  SetState(S_START); return false;
		}
	else if(state==S_SYMBOL)
		{
		if(GIsSymbol(inp)==true)
			{  mStack.Push(element);  return true; }

		ProcessSymbol();
		SetState(S_START); return false;
		}
	else if(state==S_DQUOTE_BACKSLASH)
		{
		if(inp=='\n') // Mistake in escape character
			{
			ProcessStack(MEBPT_ERROR);
			SetState(S_START);   return true;
			}
		mStack.Push(element);
		SetState(S_DQUOTE); return true;
		}
	else if(state==S_SQUOTE_BACKSLASH)
		{
		if(inp=='\n')	// Mistake in escape character
			{
			ProcessStack(MEBPT_ERROR);
			SetState(S_START);   return true;
			}

		mStack.Push(element);  SetState(S_SQUOTE); return true;
		}
	else
		{
		return false;
		}
	}


////////////////////////////////////////////////
bool MEditBufferParser::SetState(GState state)
	{  mState=state; return true;  }


////////////////////////////////////////////////
bool MEditBufferParser::ProcessStack(MEditBufferParserType type)
	{
	while(mStack.IsEmpty()==false)
		{
		MEditBufferElement *element=(MEditBufferElement *)mStack.GetTop();

		// Check if element is ok
		if(element->GetTextType()!=type)
			{ mTypeChanged=true; }

		element->SetTextType(type);
		mStack.Pop();
		}

	return true;
	}


/////////////////////////////////////////////////
bool MEditBufferParser::ProcessSymbol(void)
	{
	mSymbol.Clear();
	int length=mStack.GetSize();

	MEditBufferElement *element;
	for(int i=0;i<length;++i)
		{
		element=(MEditBufferElement *)mStack.Get(i);
		mSymbol.Add(char(element->Value) );
		}

	MEditBufferParserType symtype;
	if(mKeywordList.Search(mSymbol.Get() ) >=0) // Found Symbol	
		{  symtype=MEBPT_KEYWORD;   }
	else
		{ symtype=MEBPT_SYMBOL;  }

	// Process the symbol as type
	ProcessStack(symtype);
	return true;
	}

////////////////////////////////////////////////
bool MEditBufferParser::IsLastLineInComment(int lineno)
	{
	if(lineno==0) { return false; }

	// Find the first prior non zero length line
	int lastlinelength,lastline;
	for(lastline=lineno-1;lastline>=0;lastline -=1 )
		{
		lastlinelength=mBuffer->GetCols(lastline);
		if(lastlinelength>0) { break; }
		}

	//lastline<0 => all lines above lineno are 0
	if(lastline<0){ return false; }

	// find elements of the last line
	MEditBufferElement *elements=mBuffer->Get(lastline,0);

	//1 char length line
	if(lastlinelength==1)
		{
		if((elements+lastlinelength-1)->GetTextType()==MEBPT_COMMENT)
			{  return true;  }

		return false;
		}

	//=linelength>=2

	//Check if last characer is not in comment
	if((elements+lastlinelength-1)->GetTextType()!=MEBPT_COMMENT)
		{  return false;  }

	//=We are in a comment, but we still need to check if
	// comment end is there

	// if end!="*/", then not in comment
	if(elements[lastlinelength-1].Value=='/' 
			&&  elements[lastlinelength-2].Value=='*' )
		{  return false; }

	//=we are in a comment
	return true;
	}


////////////////////////////////////////////////
MEditBufferParser::MEditBufferParser(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MEditBufferParser::~MEditBufferParser(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MEditBufferParser::Create(MEditBuffer &ref,const char *language)
	{
	Destroy();
	mBuffer=&ref;
	if(mStack.Create(GMaxStackSize)==false)
		{
		Destroy();  return false;
		}

	if(mSymbol.Create(GMaxSymbolSize)==false)
		{
		Destroy();  return false;
		}

	// Set Default Language
	if(language==NULL)
		{  language="C++";  }

	if(mKeywordList.Create(language)==false)
		{
		Destroy();  return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MEditBufferParser::Destroy(void)
	{
	mStack.Destroy();
	mSymbol.Destroy();
	mKeywordList.Destroy();
	ClearObject();
	return true;
	}


///////////////////////////////////////////////
bool MEditBufferParser::ParseAll(void)
	{
	//Hold Current State in Processing
	int linecount=mBuffer->GetRows();
	MEditBufferElement *elements;

	SetState(S_START);
	for(int line=0;line<linecount;++line)
		{
		int linelength=mBuffer->GetCols(line);

		if(linelength!=0) 
			{ elements=mBuffer->Get(line,0); }
		else
			{ elements=NULL; }

		for(int offset=0;offset<linelength;++offset)
			{
			// Process Here every character
			while(MoveNextState((elements+offset)->Value,elements+offset)==false){ }
			}
		MoveNextState('\n',NULL);
		}
	
	return true;
	}


///////////////////////////////////////////////
bool MEditBufferParser::Parse(int lineno)
	{
	//Set Current State for processing current line
	if(IsLastLineInComment(lineno)==true)
		{  SetState(S_SLASH_STAR); }
	else
		{  SetState(S_START);  }

	int linecount=mBuffer->GetRows();

	// Last Line has nothing
	if(linecount==lineno) { return true; }

	// Process the first line completely
	int linelength=mBuffer->GetCols(lineno);	// Get Current line length

	// if Current line is empty, then stop
	if(linelength==0) { return true; }

	// Get Current line elements
	MEditBufferElement *elements=mBuffer->Get(lineno,0);

	mTypeChanged=false;  mScreenUpdate=false;

	// Parse current line
	int i;
	for(i=0;i<linelength;++i)
		{
		while(MoveNextState((elements+i)->Value,elements+i)==false){  }
		}

	// Set end of line
	while(MoveNextState('\n',NULL)==false) { }

	//=Type has changed we should continue parsing

	// Keep Processing lines till we get the stop
	for(int line=lineno+1;line<linecount;++line)
		{
		linelength=mBuffer->GetCols(line);

		if(linelength!=0) 
			{
			elements=mBuffer->Get(line,0);
			mTypeChanged=false; 
			}
		else
			{ elements=NULL; }

		for(i=0;i<linelength;++i)
			{
			// if any element's type is not set
			if((elements+i)->GetTextType()==0)
				{ mTypeChanged=true; }

			while(MoveNextState((elements+i)->Value,elements+i)==false){  }
			}

		// Set End of Line
 		while(MoveNextState('\n',NULL)==false) { }

		if(mTypeChanged==false) { return true; }
		mScreenUpdate=true; // More than one line has been parsed
		}

	return true;
	}


///////////////////////////////////////////////////////////
bool MEditBufferParser::IsScreenUpdated(void)
	{  return mScreenUpdate;  }

