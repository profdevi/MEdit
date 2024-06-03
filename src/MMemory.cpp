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
//#undef WIN32

//  Memory Block is bounded by three fence registers to check
//  if memory info or data is corrupted
#include "MStdLib.h"

///////////////////////////////////////////////////////////////
#ifdef WIN32
//****Windows Version****
#include <windows.h>
#else
///////////////////////////////////////////////////////////////
//****Linux Version****
#include <pthread.h>
#endif // WIN32

///////////////////////////////////////////////////////////////
#include "MMemory.h"


//****************************************************************
//** Module Elements
//****************************************************************
struct GMemoryBlock;
struct GBlockInfo;
static const char *GBLOCKACTIVE="Comine.com";
static const char GSTARTFENCEDATA[sizeof(void*)]="BEG";		// Start Mem Blk
static const char GMIDDLEFENCEDATA[sizeof(void*)]="MID";	// Middle Mem Blk
static const char GENDFENCEDATA[sizeof(void*)]="END";		// End Mem Blk
static const int GMAXOUTPUTLINE=1024;
static int GTotalBlocksAllocated=0;							// Total Blocks

///////////////////////////////////////////////////////
static bool GIsValid(void *memoryblock); // Quite Version of Is Valid
static GBlockInfo *GLastBlock=NULL;
static bool GMemoryTrackFlag=false;

///////////////////////////////////////////////////////
#ifdef WIN32
//*****Windows Version*******
static volatile LONG GCriticalSectionCounter=0;        // for synching

#else
///////////////////////////////////////////////////////
//*****Linux Version*******
static pthread_mutex_t  GCriticalSectionMutex=PTHREAD_MUTEX_INITIALIZER;

#endif // WIN32
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
static void GGetStatistics(int &totalblocks,int &totalbadblocks
		,int &totalusermemory);
static int GUserMemorySize(int size);
static GBlockInfo *GSearchInfo(void *usermemoryblock);
static void GInitMemoryBlock(void *memoryaddress,int length);
static void GCriticalSectionEnter(void);
static void GCriticalSectionLeave(void);

///////////////////////////////////////////////////////
struct GBlockInfo
	{
	const char *BlockActivity;
	const char *FileName;
	int FileLineNo;
	GMemoryBlock *MemoryBlock;
	int MemoryBlockUserSize;
	struct GBlockInfo *NextBlock;
	struct GBlockInfo *LastBlock;  // BackLink
	};


////////////////////////////////////////////////////////
struct GMemoryBlock		// 20 extra bytes per memory block
	{
	char FenceStart[sizeof(void*)];		// 4 bytes
	struct GBlockInfo *BlockInfo;		// Pointer to info block
	char *FenceEnd;						// Pointer to End Fence
	char FenceMiddle[sizeof(void*)];	// 4 bytes
	char Memory[1];						// Actual memory block
	//char FenceEnd[sizeof(void*)];		// 4 bytes
	};


/////////////////////////////////////////////////////////
static int GMemoryBlockOffset=4*sizeof(void *);	// distance to Mem in Block


/////////////////////////////////////////////////////////
static bool GIsStartFenceBroken(GBlockInfo *blockinfo)
	{
	if(MStdMemCmp(blockinfo->MemoryBlock->FenceStart,GSTARTFENCEDATA
			,sizeof(blockinfo->MemoryBlock->FenceStart) )==0)
		{ return false; }
	return true;
	}


/////////////////////////////////////////////////////////
static bool GIsMiddleFenceBroken(GBlockInfo *blockinfo)
	{
	if(MStdMemCmp(blockinfo->MemoryBlock->FenceMiddle,GMIDDLEFENCEDATA
			,sizeof(blockinfo->MemoryBlock->FenceMiddle) )==0)
		{ return false; }
	return true;
	}


/////////////////////////////////////////////////////////
static bool GIsEndFenceBroken(GBlockInfo *blockinfo)
	{
	if(MStdMemCmp(blockinfo->MemoryBlock->Memory
			+GUserMemorySize(blockinfo->MemoryBlockUserSize)
			,GENDFENCEDATA,sizeof(blockinfo->MemoryBlock->FenceStart) )==0)
		{ return false; }
	return true;
	}

////////////////////////////////////////////////////////
// returns a size aligned on an array of void *
static int GUserMemorySize(int size)  
	{  return ((size-1)/sizeof(void *)+1)*sizeof(void *);  }  


////////////////////////////////////////////////////////
static void GGetStatistics(int &totalblocks,int &totalbadblocks
		,int &totalusermemory)
	{
	totalblocks=0;  totalbadblocks=0;  totalusermemory=0;
	for(GBlockInfo *p=GLastBlock;p!=NULL;p=p->NextBlock)
		{
		totalblocks +=1; totalusermemory += p->MemoryBlockUserSize;
		if(GIsValid(p->MemoryBlock->Memory)==false) { totalbadblocks+=1; }
		}
	}


///////////////////////////////////////////////////////////
static GBlockInfo *GSearchInfo(void *usermemoryblock)
	{
	for(GBlockInfo *p=GLastBlock;p!=NULL;p=p->NextBlock)
		{
		if(((void *)p->MemoryBlock->Memory) == usermemoryblock)
			{  return p; }
		}

	return NULL;
	}

///////////////////////////////////////////////////////////
static void GInitMemoryBlock(void *memoryaddress,int length)
	{
	//Simply write 0 for new memory block
	MStdMemZero(memoryaddress,length);  
	}


//****************************************************************
//** MMemory
//****************************************************************
void * MMemory::Alloc(int size,const char *filename,int linenumber)
	{
	GCriticalSectionEnter();
	////////////////////////////////////////////////////////////
	if(GMemoryTrackFlag==false)
		{
		void *newmemory=MStdMalloc(size);
		if(newmemory!=NULL)
			{ GTotalBlocksAllocated=GTotalBlocksAllocated+1; }
		GCriticalSectionLeave();
		return newmemory;
		}

	///////////////////////////////////////////////////////////
	if(size<=0)
		{
		GCriticalSectionLeave(); return NULL;
		}
	
	struct GBlockInfo *blockinfo=
			(struct GBlockInfo *)MStdMalloc(sizeof(struct GBlockInfo));
	if(blockinfo==NULL)
		{
		GCriticalSectionLeave();  return NULL;
		}

	int usermemorysize=GUserMemorySize(size);

	blockinfo->MemoryBlock=(struct GMemoryBlock *)MStdMalloc(GMemoryBlockOffset
			+sizeof(void*)+usermemorysize);
	if(blockinfo->MemoryBlock==NULL)
		{
		MStdFree(blockinfo);
		GCriticalSectionLeave();  return NULL;
		}

	GInitMemoryBlock(blockinfo->MemoryBlock
			,usermemorysize+GMemoryBlockOffset+sizeof(void*) );
	blockinfo->BlockActivity=GBLOCKACTIVE;
	blockinfo->NextBlock=GLastBlock;
	blockinfo->LastBlock=NULL;
	blockinfo->MemoryBlockUserSize=size;
	blockinfo->FileName=filename;
	blockinfo->FileLineNo=linenumber;
	blockinfo->MemoryBlock->BlockInfo=blockinfo;
	MStdMemCpy(blockinfo->MemoryBlock->FenceStart,GSTARTFENCEDATA,sizeof(void*));
	MStdMemCpy(blockinfo->MemoryBlock->FenceMiddle,GMIDDLEFENCEDATA
			,sizeof(void*));
	MStdMemCpy(blockinfo->MemoryBlock->Memory+usermemorysize
			,GENDFENCEDATA,sizeof(void*));
	blockinfo->MemoryBlock->FenceEnd
			=blockinfo->MemoryBlock->Memory+usermemorysize;

	if(blockinfo->NextBlock!=NULL)
		{ blockinfo->NextBlock->LastBlock=blockinfo; }
	GLastBlock=blockinfo;	
	
	void *memoryblock=blockinfo->MemoryBlock->Memory;
	GTotalBlocksAllocated=GTotalBlocksAllocated+1;  // Update block count
	GCriticalSectionLeave();  return memoryblock;
	}


////////////////////////////////////////////////////////
void *MMemory::Realloc(void *oldblock,int newsize
			,const char *file,int lineno)
	{
	GCriticalSectionEnter();
	if(GMemoryTrackFlag==false)
		{
		void *memoryblock=MStdReAlloc(oldblock,newsize);
		GCriticalSectionLeave();
		return memoryblock;
		}
	if(newsize<=0)
		{
		GCriticalSectionLeave();  return NULL;
		}

	if(GIsValid(oldblock)==false)	{  return NULL;  }

	GMemoryBlock *block=(GMemoryBlock*)(((char *)oldblock)-GMemoryBlockOffset);
	MStdAssert(block!=NULL);

	int blocksize= GUserMemorySize(newsize) +GMemoryBlockOffset+sizeof(void*);
	GMemoryBlock *newblock=(GMemoryBlock *)MStdReAlloc(block,blocksize);
	if(newblock==NULL)
		{
		GCriticalSectionLeave();  return NULL;
		}

	newblock->BlockInfo->MemoryBlockUserSize=newsize;
	newblock->BlockInfo->MemoryBlock=newblock;
	MStdMemCpy(newblock->Memory+GUserMemorySize(newsize),GENDFENCEDATA,sizeof(void*));

	// Update end pointer
	newblock->BlockInfo->MemoryBlock->FenceEnd=
			newblock->BlockInfo->MemoryBlock->Memory+blocksize;
	newblock->BlockInfo->FileName=file;
	newblock->BlockInfo->FileLineNo=lineno;

	void *memoryblock=newblock->Memory;
	GCriticalSectionLeave();  return memoryblock;
	}


////////////////////////////////////////////////////////
bool MMemory::Free(void *memoryblock)
	{
	GCriticalSectionEnter();
	if(GMemoryTrackFlag==false)
		{
		MStdFree(memoryblock);
		GTotalBlocksAllocated=GTotalBlocksAllocated-1; // Update block count
		GCriticalSectionLeave();
		return true;
		}

	if(memoryblock==NULL)
		{
		// releasing NULL memory is fine
		GCriticalSectionLeave();
		return true;
		}

	GBlockInfo *pinfo=NULL;

	// Make Sure no problem in critical section when calling IsValid

	GCriticalSectionLeave();
	bool isvalid=MMemory::IsValid(memoryblock);
	GCriticalSectionEnter();

	if(isvalid==false)
		{

		// Perform a slow linear memory search to release memory
		pinfo=GSearchInfo(memoryblock);
		if(pinfo==NULL)
			{
			GCriticalSectionLeave();  return false;
			}
		}
	else
		{
		pinfo=((GMemoryBlock *)(( (char*)memoryblock)-GMemoryBlockOffset))->BlockInfo;
		}

	
	// Overwrite All of the memory block
	MStdMemZero(pinfo->MemoryBlock,GMemoryBlockOffset+sizeof(void*)
			+GUserMemorySize(pinfo->MemoryBlockUserSize));
	MStdFree(pinfo->MemoryBlock);  // Release the memory block

	if(pinfo==GLastBlock) { GLastBlock=GLastBlock->NextBlock; }
	if(pinfo->LastBlock!=NULL)
		{ pinfo->LastBlock->NextBlock=pinfo->NextBlock; }
	if(pinfo->NextBlock!=NULL)
		{ pinfo->NextBlock->LastBlock=pinfo->LastBlock; }
	MStdMemZero(pinfo,sizeof(GBlockInfo));  MStdFree(pinfo);  
	GTotalBlocksAllocated=GTotalBlocksAllocated-1;  // update block count

	GCriticalSectionLeave();  return true;
	}


/////////////////////////////////////////////////////////
bool MMemory::IsValid(void *memoryblock) // Checks if memory block is valid
	{
	GCriticalSectionEnter();

	if(GMemoryTrackFlag==false)
		{
		if(memoryblock!=NULL) { GCriticalSectionLeave();  return true; }
		GCriticalSectionLeave();  return false;
		}

	///////////////////////////////////////////////////////
	if(memoryblock==NULL)
		{
		GCriticalSectionLeave();  return false;
		}

	GMemoryBlock *blockdata
			=(GMemoryBlock *)(((char *)memoryblock)-GMemoryBlockOffset);

	if(MStdMemCmp(blockdata->FenceStart,GSTARTFENCEDATA
			,sizeof(blockdata->FenceStart) )!=0)
		{
		GCriticalSectionLeave();  return false;
		}

	if(MStdMemCmp(blockdata->FenceMiddle,GMIDDLEFENCEDATA
			,sizeof(blockdata->FenceMiddle) )!=0)
		{
		GCriticalSectionLeave();  return false;
		}


	if(blockdata->BlockInfo==NULL)
		{
		GCriticalSectionLeave();  return false;
		}

	if(blockdata->BlockInfo->BlockActivity!=GBLOCKACTIVE)
		{
		GCriticalSectionLeave();  return false;
		}

	if(blockdata->BlockInfo->MemoryBlock==NULL)
		{
		GCriticalSectionLeave();  return false;
		}

	if(blockdata->BlockInfo->MemoryBlock->Memory!=memoryblock)
		{
		GCriticalSectionLeave();  return false;
		}

	if(blockdata->BlockInfo->MemoryBlockUserSize<=0)
		{
		GCriticalSectionLeave();  return false;
		}

	int usermemorysize
			=GUserMemorySize(blockdata->BlockInfo->MemoryBlockUserSize);

	if(MStdMemCmp(((char *)memoryblock)+usermemorysize,GENDFENCEDATA
			,sizeof(blockdata->FenceStart) )!=0)
		{
		GCriticalSectionLeave();  return false;
		}

	GCriticalSectionLeave();  return true;
	}


/////////////////////////////////////////////////////////
static bool GIsValid(void *memoryblock) // Quite Version of Is Valid
	{
	if(GMemoryTrackFlag==false)
		{
		if(memoryblock!=NULL) { return true; }
		return false;
		}

	if(memoryblock==NULL) {  return false; }

	GMemoryBlock *blockdata
			=(GMemoryBlock *)(((char *)memoryblock) - GMemoryBlockOffset);

	if(MStdMemCmp(blockdata->FenceStart,GSTARTFENCEDATA
				,sizeof(blockdata->FenceStart))!=0)
		{ return false;}

	if(blockdata->BlockInfo==NULL){ return false;}

	if(blockdata->BlockInfo->BlockActivity!=GBLOCKACTIVE){ return false;}

	if(blockdata->BlockInfo->MemoryBlock==NULL){ return false;}

	if(blockdata->BlockInfo->MemoryBlock->Memory!=memoryblock)
		{ return false;}

	if(blockdata->BlockInfo->MemoryBlockUserSize<=0)
		{ return false;}

	int usermemorysize
			=GUserMemorySize(blockdata->BlockInfo->MemoryBlockUserSize);

	if(MStdMemCmp(blockdata->FenceStart,GSTARTFENCEDATA
			,sizeof(blockdata->FenceStart) )!=0)
		{  return false;  }

	if(MStdMemCmp(blockdata->FenceMiddle,GMIDDLEFENCEDATA
			,sizeof(blockdata->FenceMiddle) )!=0)
		{  return false;  }

	if(MStdMemCmp(((char *)memoryblock)+usermemorysize,GENDFENCEDATA
			,sizeof(blockdata->FenceStart) )!=0)
		{ return false;}

	return true;
	}



#ifdef WIN32
/////////////////////////////////////////////////////////////////////
// Windows Version
static void GCriticalSectionEnter(void)
	{
	for(;;)
		{	long newvalue;
		newvalue=InterlockedIncrement((LPLONG)&GCriticalSectionCounter);
		if(newvalue==1) { return; }
		InterlockedDecrement((LPLONG)&GCriticalSectionCounter);
		}
	}
#else
/////////////////////////////////////////////////////////////////////
//Linux Version
static void GCriticalSectionEnter(void)
	{
	if(pthread_mutex_lock(&GCriticalSectionMutex)!=0)
		{
		return;
		}
	}
#endif // WIN32



#ifdef WIN32
/////////////////////////////////////////////////////////////////////
// Windows Version
static void GCriticalSectionLeave(void)
	{
	InterlockedDecrement((LPLONG)&GCriticalSectionCounter);
	}
#else
/////////////////////////////////////////////////////////////////////
//Linux Version
static void GCriticalSectionLeave(void)
	{
	if(pthread_mutex_unlock(&GCriticalSectionMutex)!=0)
		{
		return;
		}
	}

#endif // WIN32


/////////////////////////////////////////////////////////////////
int MMemory::GetTotalBlocks(void)			// Get Total Memory Blocks
	{	return GTotalBlocksAllocated;  }


#if _MSC_VER>1300
/////////////////////////////////////////////////////////////////
void *operator new(size_t amount)
	{
	// Call Our Memory
	void *mem=MMemory::Alloc(amount,"new op " __FILE__,__LINE__);
	if(mem==NULL)
		{  return nullptr;  }

	// Init New Memory to Zero
	MStdMemZero(mem,amount);

	return mem;
	}


/////////////////////////////////////////////////////////////////
void operator delete(void *memory)
	{
	// Call our Free
	MMemory::Free(memory);
	}
#endif // _MSC_VER

