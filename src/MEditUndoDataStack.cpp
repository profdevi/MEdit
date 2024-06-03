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


//v2.0 copyright Comine.com 20230602F2212
#include "MStdLib.h"

#include "MEditUndoDataStack.h"



//******************************************************
//**  Module Elements
//******************************************************
struct MEditUndoDataStackNode
	{
	MEditUndoDataStackNode *Next;
	char *Data;							// Pointer to the string data
	int Row;							// Row Position of string
	int Column;							// Column Position of string
	int Type;							// Type element of string
	};


////////////////////////////////////////////////////////
static void GReleaseList(MEditUndoDataStackNode *list)
	{
	MEditUndoDataStackNode *tmp;
	while(list!=NULL)
		{
		tmp=list->Next;
		delete[] list->Data;
		delete list;
		list=tmp;
		}
	}
	

//******************************************************
//**  MEditUndoDataStack class
//******************************************************
void MEditUndoDataStack::ClearObject(void)
	{
	mList=NULL;
	}


////////////////////////////////////////////////
MEditUndoDataStack::MEditUndoDataStack(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MEditUndoDataStack::~MEditUndoDataStack(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MEditUndoDataStack::Create(void)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////////
bool MEditUndoDataStack::Destroy(void)
	{
	GReleaseList(mList); 
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MEditUndoDataStack::Push(const char *data,int row,int column,int type)
	{
	MEditUndoDataStackNode *newnode;
	newnode=new(std::nothrow) MEditUndoDataStackNode;
	if(newnode==NULL)
		{
		return false;
		}

	MStdMemSet(newnode,0,sizeof(*newnode));

	// Allocate space for string
	int length=MStdStrLen(data)+1;
	newnode->Data=new(std::nothrow) char[length];
	if(newnode->Data==NULL)
		{
		delete newnode; // release allocated node
		return false;
		}

	MStdStrCpy(newnode->Data,data);  newnode->Row=row; newnode->Column=column;
	newnode->Type=type;  newnode->Next=mList; mList=newnode;
	return true;
	}

////////////////////////////////////////////////
bool MEditUndoDataStack::IsEmpty(void)
	{
	if(mList==NULL) { return true; }
	return false;
	}



////////////////////////////////////////////////
bool MEditUndoDataStack::Pop(void)
	{
	if(mList==NULL)
		{
		return false;
		}

	MEditUndoDataStackNode *tmp=mList;
	mList=mList->Next;  
	delete[] tmp->Data;
	delete tmp;

	return true;
	}


////////////////////////////////////////////////
const char *MEditUndoDataStack::GetString(void)
	{
	if(mList==NULL)
		{
		return NULL;
		}

	return mList->Data;
	}


////////////////////////////////////////////////
int MEditUndoDataStack::GetRow(void)
	{
	if(mList==NULL)
		{
		return 0;
		}

	return mList->Row;
	}


////////////////////////////////////////////////
int MEditUndoDataStack::GetColumn(void)
	{
	if(mList==NULL)
		{
		return 0;
		}

	return mList->Column;
	}


////////////////////////////////////////////////
int MEditUndoDataStack::GetType(void)
	{
	if(mList==NULL)
		{
		return 0;
		}

	return mList->Type;
	}

