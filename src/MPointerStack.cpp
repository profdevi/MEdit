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


//v1.5 copyright Comine.com 20131227F1415
#include "MStdLib.h"
#include "MPointerStack.h"



//******************************************************
//**  MPointerStack class
//******************************************************
void MPointerStack::ClearObject(void)
	{
	mArray=NULL;
	mMaxSize=0;
	mTop=-1;
	}


/////////////////////////////////////////////////
MPointerStack::MPointerStack(void)
	{  ClearObject(); }


////////////////////////////////////////////////
MPointerStack::MPointerStack(int size)
	{
	ClearObject();
	if(Create(size)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MPointerStack::~MPointerStack(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MPointerStack::Create(int maxsize)
	{
	Destroy();
	mArray=new(std::nothrow) void *[maxsize];
	if(mArray==NULL)
		{
		Destroy();  return false;
		}

	mMaxSize=maxsize;
	return true;
	}


////////////////////////////////////////////////
bool MPointerStack::Destroy(void)
	{
	if(mArray!=NULL)
		{
		delete[] mArray;
		mArray=NULL;
		}

	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MPointerStack::IsInitialized(void)
	{  return mInitialized;  }


///////////////////////////////////////////////
bool MPointerStack::Clear(void)
	{
	mTop=-1; return true;
	}

///////////////////////////////////////////////
bool MPointerStack::Push(void *element)
	{
	if(mTop==mMaxSize-1)
		{
		return false;
		}
	
	mTop += 1;  mArray[mTop]=element;  return true;
	}


///////////////////////////////////////////////
bool MPointerStack::Pop(void)
	{
	if(mTop==-1)
		{
		return false;
		}

	mTop -= 1;
	return true;
	}


///////////////////////////////////////////////
void *MPointerStack::GetTop(void)
	{
	if(mTop==-1)
		{
		return 0;
		}

	return mArray[mTop];
	}


///////////////////////////////////////////////
int MPointerStack::GetSize(void)
	{
	return mTop+1;
	}


///////////////////////////////////////////////
int MPointerStack::GetMaxSize(void)
	{
	return mMaxSize;
	}

///////////////////////////////////////////////
bool MPointerStack::IsEmpty(void)
	{
	if(mTop==-1) { return true; }
	return false;
	}

///////////////////////////////////////////////
bool MPointerStack::IsFull(void)
	{
	if(mTop>=mMaxSize-1) { return true; }
	return false;
	}


//////////////////////////////////////////////
void *MPointerStack::Get(int index)
	{
	return mArray[index];
	}


