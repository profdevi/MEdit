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


//v1.19 copyright Comine.com 20180626T1146
#include "MStdLib.h"
#include "MStringList.h"


//********************************************************************
//** Module Elements
//********************************************************************
struct GNode
	{
	char *Data;
	GNode *Next;
	};


//********************************************************************
//** MStringList
//********************************************************************
void MStringList::ClearObject(void)
	{
	mFirstNode=NULL;
	mLastNode=NULL;
	mCurrent=NULL;
	mCount=0;
	}


//////////////////////////////////////////////////////////////
MStringList::MStringList(void)
	{  ClearObject();  }


//////////////////////////////////////////////////////////////
MStringList::MStringList(bool create)
	{
	ClearObject();
	if(create==true && Create()==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////////////////
MStringList::MStringList(MStringList &reflist)
	{
	ClearObject();
	if(Create(reflist)==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////////////////
MStringList::~MStringList(void)
	{  Destroy();  }


//////////////////////////////////////////////////////////////
bool MStringList::Create(void)
	{
	Destroy();
	return true;
	}

//////////////////////////////////////////////////////////////
bool MStringList::Create(MStringList &reflist)
	{
	Destroy();
	if(Create()==false)
		{
		Destroy();
		return false;
		}

	// Iterate through the list
	GNode *p;
	for(p=reflist.mFirstNode;p!=NULL;p=p->Next)
		{
		MStdAssert(p->Data!=NULL);
		if(AddString(p->Data)==false)
			{
			Destroy();
			return false;
			}
		}

	return true;
	}


/////////////////////////////////////////////////////////////
bool MStringList::Destroy(void)
	{
	GNode *pnode;
	GNode *tmp;
	for(pnode=mFirstNode;pnode!=NULL; pnode=tmp)
		{	
		tmp=pnode->Next;  

		if(pnode->Data!=NULL)
			{
			delete[] pnode->Data;
			pnode->Data=NULL;
			}

		delete pnode;
		}

	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////////
bool MStringList::AddString(const char *string) // Add to the end of the list
	{
	if(string==NULL)
		{
		return false;
		}

	GNode *newnode;
	newnode=new(std::nothrow) GNode;
	if(newnode==NULL)
		{
		return false;
		}

	newnode->Data=new(std::nothrow) char[MStdStrLen(string)+1];
	if(newnode->Data==NULL)
		{
		delete newnode;
		return false;
		}

	MStdStrCpy(newnode->Data,string);
	newnode->Next=NULL;

	// Increment count
	mCount = mCount + 1;

	if(mFirstNode==NULL)
		{  mCurrent=mFirstNode=mLastNode=newnode;  return true;  }

	mLastNode->Next=newnode;  mLastNode=newnode; return true;
	}


////////////////////////////////////////////////////////////
bool MStringList::AddList(MStringList &list)
	{
	GNode *p;
	for(p=list.mFirstNode;p!=NULL;p=p->Next)
		{
		if(AddString(p->Data)==false)
			{
			return false;
			}
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MStringList::ReadReset(void)  // Will Reset to the first item again
	{
	mCurrent=mFirstNode;
	return true;
	}


////////////////////////////////////////////////////////////
const char *MStringList::ReadString(void)  // Read a string untill NULL
	{
	if(mCurrent==NULL) { return NULL; }
	const char *retstr=mCurrent->Data;
	mCurrent=mCurrent->Next;
	return retstr;
	}


/////////////////////////////////////////////////////////////
bool MStringList::IsMember(const char *string,bool casematch)
	{
	if(string==NULL) { return false; }

	// Search through all items
	if(casematch==true)
		{
		GNode *p;
		for(p=mFirstNode;p!=NULL;p=p->Next)
			{
			if(MStdStrCmp(p->Data,string)==0)
				{ return true; }
			}
		}
	else
		{
		GNode *p;
		for(p=mFirstNode;p!=NULL;p=p->Next)
			{
			if(MStdStrICmp(p->Data,string)==0)
				{ return true; }
			}
		}

	return false;
	}


////////////////////////////////////////////////////////////////
int MStringList::GetCount(void)
	{
	return mCount;
	}


///////////////////////////////////////////////////////////////
bool MStringList::Remove(const char *string)
	{
	if(mFirstNode==NULL)
		{
		// List is empty
		return false;
		}

	// Check if first node is the element to remove
	if(MStdStrCmp(mFirstNode->Data,string)==0)
		{
		//=We have found the string in the first node
		GNode *tmpnode=mFirstNode;
		mFirstNode=tmpnode->Next;
		
		if(mFirstNode==NULL)
			{
			mLastNode=mFirstNode;
			}

		// Update current node
		if(mCurrent==tmpnode)
			{
			mCurrent=mFirstNode;
			}

		// Release memory
		delete [] tmpnode->Data;

		delete tmpnode;

		mCount=mCount-1;
		return true;
		}
	
	// Remove a node
	GNode *p;
	for(p=mFirstNode;p->Next!=NULL;p=p->Next)
		{
		if(MStdStrCmp(p->Next->Data,string)==0)
			{
			GNode *tmpnode=p->Next;

			// Link past node
			p->Next=tmpnode->Next;

			// Update current node
			if(mCurrent==tmpnode)
				{
				mCurrent=p->Next;
				}

			// Update the last node
			if(mLastNode==tmpnode)
				{  mLastNode=p;	 }

			// Release memory
			delete []tmpnode->Data;

			delete tmpnode;

			mCount=mCount-1;
			return true;
			}

		//=Keep Searching in next node
		}
	
	// We have not found the string
	return false;
	}


////////////////////////////////////////////////////////
bool MStringList::Swap(MStringList &list)
	{
	GNode *tmp;

	tmp=mFirstNode;  
	mFirstNode=list.mFirstNode;
	list.mFirstNode=tmp;

	tmp=mLastNode;
	mLastNode=list.mLastNode;
	list.mLastNode=tmp;

	tmp=mCurrent;
	mCurrent=list.mCurrent;
	list.mCurrent=tmp;

	int tmpcount;
	tmpcount=mCount;
	mCount=list.mCount;
	list.mCount=tmpcount;

	return true;
	}


////////////////////////////////////////////////////////////////////////
bool MStringList::Print(void)
	{
	int count=1;		
	for(GNode *p=mFirstNode;p!=NULL;p=p->Next)
		{
		MStdPrintf("\t%2d : %s\n",count,p->Data);
		count=count+1;
		}
	
	return true;
	}


