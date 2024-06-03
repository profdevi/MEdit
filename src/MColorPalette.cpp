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


//v1.4 copyright Comine.com 20240603M1039
#include <windows.h>
#include "MStdLib.h"
#include "MMemory.h"
#include "MColorPalette.h"


//******************************************************
//**  MColorPalette class
//******************************************************
void MColorPalette::ClearObject(void)
	{
	mArray=NULL;  mLength=0; 
	}


////////////////////////////////////////////////
MColorPalette::MColorPalette(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MColorPalette::MColorPalette(COLORREF array[],int length)
	{
	ClearObject();
	if(Create(array,length)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MColorPalette::~MColorPalette(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MColorPalette::Create(int length)
	{
	Destroy();

	if(length<=0)
		{
		return false;
		}

	mArray=(COLORREF *)MMemory::Alloc(sizeof(COLORREF)*length,__FILE__,__LINE__);
	if(mArray==NULL)
		{
		return false;
		}

	mLength=length;

	// Set of a linear gradiant between white and black
	for(int i=0;i<mLength;++i)
		{  mArray[i]=(int) double( i*1.0/mLength*RGB(255,255,255) ); }

	return true;
	}


////////////////////////////////////////////////
bool MColorPalette::Create(COLORREF array[],int length)
	{
	Destroy();
	if(length<=0)
		{
		return false;
		}

	mArray=(COLORREF *)MMemory::Alloc(sizeof(COLORREF)*length,__FILE__,__LINE__);
	if(mArray==NULL)
		{
		return false;
		}

	mLength=length;

	// Copy elements over
	for(int i=0;i<mLength;++i)
		{  mArray[i]=array[i];  }

	return true;
	}


////////////////////////////////////////////////
bool MColorPalette::Destroy(void)
	{
	if(mArray!=NULL) { MMemory::Free(mArray);  }
	ClearObject();  return true;
	}


////////////////////////////////////////////////
bool MColorPalette::Set(int index,COLORREF ref)
	{
	if(index<0 || index>=mLength)
		{
		return false;
		}

	mArray[index]=ref;  return true;
	}


//////////////////////////////////////////////
COLORREF MColorPalette::Get(int index)
	{
	return mArray[index];
	}


//////////////////////////////////////////////
int MColorPalette::GetLength(void)
	{
	return mLength;
	}

