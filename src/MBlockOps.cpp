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


//v1.5 copyright Comine.com 20240603M0941
#include "MStdLib.h"
#include "MBlockOps.h"


//******************************************************
//**  MBlockOps class
//******************************************************
void MBlockOps::ClearObject(void)
	{
	mBlockStartLine=mBlockStartLineOffset=0;
	mBlockStopLine=mBlockStopLineOffset=0;
	}


/////////////////////////////////////////////////////////////////////////
bool MBlockOps::IsBlockOrdered(void)
	{
	if(mBlockStartLine<mBlockStopLine)
		{ return true; }
	else if(mBlockStartLine>mBlockStopLine)
		{ return false; }
	else if(mBlockStartLineOffset<=mBlockStopLineOffset)
		{  return true; }
	else
		{  return false;  }
	}


//////////////////////////////////////////////////////////////////////////
void MBlockOps::ReOrder(int &start,int &startoffset,int &end,int &endoffset)
	{
	if(IsBlockOrdered()==true)
		{
		start=mBlockStartLine;
		startoffset=mBlockStartLineOffset;
		end=mBlockStopLine;
		endoffset=mBlockStopLineOffset;
		}
	else
		{
		end=mBlockStartLine;
		endoffset=mBlockStartLineOffset;
		start=mBlockStopLine;
		startoffset=mBlockStopLineOffset;
		}
	}


////////////////////////////////////////////////
MBlockOps::MBlockOps(void)
	{	ClearObject();  }


////////////////////////////////////////////////
MBlockOps::~MBlockOps(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MBlockOps::Create(void)
	{
	Destroy();
	mBlockStartLine=mBlockStartLineOffset=0;
	mBlockStopLine=mBlockStopLineOffset=0;
	return true;
	}


////////////////////////////////////////////////
bool MBlockOps::Destroy(void)
	{
	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////////////////////
bool MBlockOps::BlockStop(int cursorx,int cursory)
	{
	mBlockStopLine=cursorx;  mBlockStopLineOffset=cursory;
	return true;
	}


///////////////////////////////////////////////////////////////////////////
bool MBlockOps::BlockStart(int cursorx,int cursory) // Marks region of block
	{
	if(IsBlocked()==true) { mBlockStoped=true; } else { mBlockStoped=false; }
	mBlockStartLine=cursorx;  mBlockStartLineOffset=cursory;
	mBlockStopLine=cursorx;  mBlockStopLineOffset=cursory;
	return true;
	}


//////////////////////////////////////////////////////////////////////////
bool MBlockOps::IsBlocked(void)
	{
	if(mBlockStartLine==mBlockStopLine && 
			mBlockStartLineOffset==mBlockStopLineOffset )
		{  return false;  }

	return true;
	}


/////////////////////////////////////////////////////////////////////////
bool MBlockOps::IsBlocked(int line,int offset)
	{
	//Move start before end
	int start,startoffset,end,endoffset;
	ReOrder(start,startoffset,end,endoffset);

	//Now check if blocked
	if(line<start || line>end) { return false; }
	else if(start==end && line==start)
		{
		if(offset>=startoffset && offset<endoffset) { return true; }
		return false;
		}
	else if(start<line && line<end) { return true; }
	else if(start==line && offset>=startoffset) { return true; }
	else if(end==line && offset<endoffset) { return true; }

	return false;
	}


//////////////////////////////////////////////////////////////////////
bool MBlockOps::IsBlockStopped(void)		// Checks if Last operations caused block stop
	{
	return mBlockStoped;
	}


/////////////////////////////////////////////////////////////////////
bool MBlockOps::IsBlockInWindow(int rowmin,int rowmax)
	{
	int start,startoffset,end,endoffset;
	ReOrder(start,startoffset,end,endoffset);
	
	//Now check if blocked
	if(end<start || start>end) { return false; }
	return true;
	}


////////////////////////////////////////////////////////////////////
bool MBlockOps::GetBlockedRegion(int &start,int &startoffset
		,int &end,int &endoffset)
	{
	if(IsBlocked()==false) { return false; }
	ReOrder(start,startoffset,end,endoffset);
	return true;
	}

