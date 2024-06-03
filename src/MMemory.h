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


//v1.33 copyright Comine.com 20240603M1004
#ifndef MMemory_h
#define MMemory_h

////////////////////////////////////////////////////////////////
#include "MStdLib.h"

////////////////////////////////////////////////////////////////
class MMemory
	{

	////////////////////////////////////////////////////////////
	public:
	static void *Alloc(int size,const char *file,int linenumber);
	static void *Realloc(void *oldblock,int newsize,const char *file,int linenumber);
	static bool Free(void *memoryblock);
	static bool IsValid(void *memoryblock);			// Checks if memory block is valid
	static int GetTotalBlocks(void);				// Get Total Memory Blocks
	};


#if _MSC_VER>1300
//****************************************************************
//** Overload for C++ Ease (does not work for vs6.0)
//****************************************************************
void *operator new(size_t);
void operator delete(void *);
#endif // _MSC_VER

/////////////////////////////////////////////////
template <class Simple>
Simple *MMemoryAlloc(const char *file,int lineno)
	{
	Simple *mem;
	mem=(Simple *)MMemory::Alloc(sizeof(Simple),file,lineno);
	if(mem==0)
		{
		MStdLog("Unable to alloc memory object",file,lineno);
		return 0;
		}

	MStdMemZero(mem,sizeof(Simple) );
	return mem;
	}


////////////////////////////////////////////////
template <class Simple>
bool MMemoryFree(Simple  *&ptr)
	{
	if(ptr==0)
		{
		return true;
		}

	ptr->Destroy();
	MMemory::Free(ptr);
	ptr=0;
	return true;
	}

#endif // MMemory_h

