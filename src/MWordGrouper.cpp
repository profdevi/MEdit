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

/*
	This implementation will break apart continuous letters/spaces/not(space+letter)
	groupings.
*/
#include "MStdLib.h"
#include "MStringBuffer.h"
#include "MWordGrouper.h"



//***************************************************************
//*  Module Elements
//***************************************************************
static const int GMaxBufferSize=512;		// hold the maximume lexeme size

////////////////////////////////////////////////////////
static bool GIsLetter(char ch)
	{
	if(ch=='_') { return true; }
	if(ch>='A' && ch <='Z') { return true; }
	if(ch>='a' && ch <='z') { return true; }
	if(ch>='0' && ch <='9') { return true; }
	return false;
	}


////////////////////////////////////////////////////////
static bool GIsSpace(char ch)
	{
	if(ch==' ' || ch=='\t' || ch=='\r' || ch=='\n') { return true; }
	return false;
	}


////////////////////////////////////////////////////////
static bool GIsNonLetter(char item)
	{
	if(GIsLetter(item)==true || GIsSpace(item)==true) { return false; }
	return true;
	}


	//**************************************
	//** Default mOnFlushHandler
	//**************************************
static void GOnFlushDefault(void *argument,int row,int col
					,const char *itemarray,MWordGrouperType type)
	{
	}

//***************************************************************
//*  MWordGrouper Class
//***************************************************************
enum MWordGrouperGroupingType{ MUTGT_UNSET,MUTGT_LETTER,MUTGT_SPACE
		,MUTGT_NONLETTERSPACE };


///////////////////////////////////////////////////////
void MWordGrouper::ClearObject(void)
	{
	mGrouping=MUTGT_UNSET;
	mType=MWGT_UNSET;
	mLine=0; mOffset=0;
	mOnFlush=GOnFlushDefault;
	mOnFlushArgument=NULL;	
	}

///////////////////////////////////////////////////////
void MWordGrouper::SetGrouping(char ch)
	{
	if(GIsLetter(ch)==true)
		{ mGrouping=MUTGT_LETTER; }
	else if(GIsSpace(ch)==true)
		{ mGrouping=MUTGT_SPACE; }
	else
		{mGrouping=MUTGT_NONLETTERSPACE; }
	}


///////////////////////////////////////////////////////
bool MWordGrouper::IsGroupingChanged(char ch)
	{
	if(mGrouping==MUTGT_LETTER && GIsLetter(ch)==false) { return true; }
	if(mGrouping==MUTGT_SPACE && GIsSpace(ch)==false) { return true; }
	if(mGrouping==MUTGT_NONLETTERSPACE && GIsNonLetter(ch)==false ){  return true;  }
	return false;
	}


////////////////////////////////////////////////////////
MWordGrouper::MWordGrouper(void)
	{  ClearObject();  }


////////////////////////////////////////////////////////
MWordGrouper::~MWordGrouper(void)
	{  Destroy();  }


////////////////////////////////////////////////////////
bool MWordGrouper::Create(void Flush(void *argument,int row,int col
					,const char *itemarray,MWordGrouperType type)
			,void *argument)
	{
	Destroy();
	mLine=0; mOffset=0;  mType=MWGT_UNSET; mGrouping=MUTGT_UNSET;

	if(mBuffer.Create(GMaxBufferSize)==false)
		{
		return false;
		}

	mOnFlush=Flush;  mOnFlushArgument=argument;
	return true;
	}


////////////////////////////////////////////////////////
bool MWordGrouper::Destroy(void)
	{
	mBuffer.Destroy();
	ClearObject();
	return true;
	}


////////////////////////////////////////////////////////
bool MWordGrouper::InsertChar(int line,int offset,char ch)
	{	bool ret;
	if(mType!=MWGT_UNSET)
		{
		if(mType!=MWGT_INSERT || line!=mLine 
				|| offset-mOffset!=mBuffer.GetStringLength() 
				|| IsGroupingChanged(ch)==true  )
			{  FlushData(); }
		}


	if(mType==MWGT_UNSET)
		{
		ret=mBuffer.Clear();

		ret=mBuffer.Add(ch);

		mLine=line;  mOffset=offset;  mType=MWGT_INSERT;
		SetGrouping(ch); return true;
		}
	
	ret=mBuffer.Add(ch);
	return true;
	}


////////////////////////////////////////////////////////
bool MWordGrouper::DeleteChar(int line,int offset,char oldchar)
	{	bool ret;
	if(mType!=MWGT_UNSET)
		{
		if(mType!=MWGT_DELETE || line!=mLine
				|| IsGroupingChanged(oldchar)==true )
			{  FlushData();  }
		else if(offset!=mOffset && offset!=mOffset-1)
			{  FlushData(); }
		}


	if(mType==MWGT_UNSET)
		{
		ret=mBuffer.Clear();

		ret=mBuffer.Add(oldchar);
		mLine=line;  mOffset=offset;  mType=MWGT_DELETE;
		SetGrouping(oldchar); return true;
		}
	
	if(offset==mOffset)
		{
		ret=mBuffer.Add(oldchar);
		return true;
		}
	else
		{
		ret=mBuffer.Add(0,oldchar);
		mOffset -=1;
		return true;
		}
	}


////////////////////////////////////////////////////////
bool MWordGrouper::OverWriteChar(int line,int offset,char oldchar,char newchar)
	{	bool ret;
	if(mType!=MWGT_UNSET) {  FlushData();  }

	ret=mBuffer.Clear();
	ret=mBuffer.Add(oldchar);
	ret=mBuffer.Add(newchar);
	mLine=line;  mOffset=offset;  mType=MWGT_OVERWRITE;
	FlushData();  return true;
	}


////////////////////////////////////////////////////////
bool MWordGrouper::PasteText(int line,int offset,const char *textdata)
	{
	FlushData();
	
	mOnFlush(mOnFlushArgument,line,offset,textdata,MWGT_PASTE);  // Call Back
	return true;
	}


////////////////////////////////////////////////////////
bool MWordGrouper::CutText(int line,int offset,const char *textdata)
	{
	FlushData();

	mOnFlush(mOnFlushArgument,line,offset,textdata,MWGT_CUT);
	return true;
	}


////////////////////////////////////////////////////////
bool MWordGrouper::FlushData(void)
	{
	if(mType==MWGT_UNSET) { return false; }
	const char *data=mBuffer.Get();

	mOnFlush(mOnFlushArgument,mLine,mOffset,data,mType);

	mBuffer.Clear();
	mType=MWGT_UNSET;  mLine=0; mOffset=0; mGrouping=MUTGT_UNSET;
	return true;
	}


////////////////////////////////////////////////////////
bool MWordGrouper::Clear(void)
	{	bool ret;
	ret=mBuffer.Clear();

	mType=MWGT_UNSET;  mLine=0;  mOffset=0;
	mGrouping=MUTGT_UNSET;  return true;
	}

