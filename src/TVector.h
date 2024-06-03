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


//v2.6 copyright Comine.com 20231129W2335
#ifndef TVector_h
#define TVector_h

////////////////////////////////////////////
#include "MStdLib.h"
#include "MIReader.h"
#include "MIWriter.h"

////////////////////////////////////////////
template <typename Data>
class TVector
	{
	Data *mArray;
	int mCount;

	/////////////////////////////////////////
	void ClearObject(void);

	public:
	/////////////////////////////////////////
	TVector(void);
	explicit TVector(int count);
	explicit TVector(const TVector<Data> &other);
	explicit TVector(MIReader &reader);
	TVector(const Data *data,int len);
	TVector(TVector &&ref);
	~TVector(void);
	bool Create(int count);
	bool Create(const TVector<Data> &other);
	bool Create(MIReader &reader);
	bool Create(const Data *arr,int count);
	bool Destroy(void);
	Data *Get(void) const;
	int GetCount(void) const;
	Data &operator[](int index) const;
	Data & Get(int index) const;
	Data *GetItemPtr(int index) const;
	bool Set(int index,const Data &ref);
	bool Set(const Data &ref);
	bool Swap(TVector<Data> &ref);
	bool Swap(int index1,int index2);
	bool Write(MIWriter &writer) const;
	bool Copy(Data *target,int len);
	bool Reverse(void);
	bool Resize(int newsize);
	bool operator=(const TVector<Data>  &copy);
	};


////////////////////////////////////////////////
// Template Function Definitions
////////////////////////////////////////////////
template <typename Data>
void TVector<Data>::ClearObject(void)
	{
	mArray=0;
	mCount=0;
	}	


//////////////////////////////////////////////
template <typename Data>
TVector<Data>::TVector(void)
	{
	ClearObject();
	}


/////////////////////////////////////////
template <typename Data>
TVector<Data>::TVector(int count)
	{
	ClearObject();
	if(Create(count)==false)
		{
		return;
		}
	}


/////////////////////////////////////////
template <typename Data>
TVector<Data>::TVector(const TVector<Data> &other)
	{
	ClearObject();
	if(Create(other)==false)
		{
		return;
		}
	}


/////////////////////////////////////////
template <typename Data>
TVector<Data>::TVector(MIReader &reader)
	{
	ClearObject();
	if(Create(reader)==false)
		{
		return;
		}
	}

/////////////////////////////////////////
template <typename Data>
TVector<Data>::TVector(const Data *data,int len)
	{
	ClearObject();
	if(Create(data,len)==false)
		{
		}
	}


////////////////////////////////////////
template <typename Data>
TVector<Data>::TVector(TVector &&refobj)
	{
	ClearObject();
	mArray=refobj.mArray;
	mCount=refobj.mCount;
	refobj.mArray=nullptr;
	refobj.mCount=0;
	}


/////////////////////////////////////////
template <typename Data>
TVector<Data>::~TVector(void)
	{  Destroy();  }


///////////////////////////////////////
template <typename Data>
bool TVector<Data>::Create(int count)
	{
	Destroy();
	MStdAssert(count>0);
	mCount=count;
	mArray = new(std::nothrow) Data[mCount];
	if(mArray==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


///////////////////////////////////////
template <typename Data>
bool TVector<Data>::Create(const TVector<Data> &other)
	{
	Destroy();
	if(Create(other.GetCount())==false)
		{
		return false;
		}

	// Now Copy the data elements
	const Data *src=other.Get();
	Data *target=mArray;
	int count=other.GetCount();

	int i;
	for(i=0;i<count;++i)
		{
		// assignment constructor
		*target++ = *src++;
		}

	return true;
	}

	
///////////////////////////////////////
template <typename Data>
bool TVector<Data>::Create(MIReader &reader)
	{
	int count;
	if(reader.Read(count)==false)
		{
		Destroy();
		return false;
		}

	if(Create(count)==false)
		{  return false;  }

	// Now read the array
	int i;
	for(i=0;i<mCount;++i)
		{
		if(MIReaderRead(reader,mArray[i])==false)
			{
			Destroy();
			return false;
			}
		}
		
	return true;
	}


////////////////////////////////////////
template <typename Data>
bool TVector<Data>::Create(const Data *arr,int count)
	{
	if(Create(count)==false)
		{
		return false;
		}

	int i;
	for(i=0;i<count;++i)
		{
		// assignment copy
		mArray[i] = arr[i];
		}

	return true;
	}


////////////////////////////////////////
template<typename Data>
bool TVector<Data>::Destroy(void)
	{
	if(mArray!=0)
		{
		delete[] mArray;
		mArray=0;
		}

	ClearObject();
	return true;
	}


////////////////////////////////////////
template<typename Data>
Data *TVector<Data>::Get(void) const
	{
	return mArray;
	}


///////////////////////////////////////
template<typename Data>
int TVector<Data>::GetCount(void) const
	{
	return mCount;
	}


////////////////////////////////////////
template <typename Data>
Data &TVector<Data>::operator[](int index) const
	{
	return mArray[index];
	}

////////////////////////////////////////
template <typename Data>
Data & TVector<Data>::Get(int index) const
	{
	return mArray[index];
	}


////////////////////////////////////////
template <typename Data>
Data *TVector<Data>::GetItemPtr(int index) const
	{
	return mArray+index;
	}


////////////////////////////////////////
template <typename Data>
bool TVector<Data>::Set(int index,const Data &ref)
	{
	Data tmpobj(ref);
	mArray[index]=tmpobj;
	return true;
	}


////////////////////////////////////////
template <typename Data>
bool TVector<Data>::Set(const Data &ref)
	{
	int i;
	for(i=0;i<mCount;++i)
		{
		Data tmpobj(ref);
		mArray[i]=tmpobj;
		}

	return true;
	}


////////////////////////////////////////
template <typename Data>
bool TVector<Data>::Swap(TVector<Data> &ref)
	{
	MStdSwap(mArray,ref.mArray);
	MStdSwap(mCount,ref.mCount);
	return true;
	}


/////////////////////////////////////////
template <typename Data>
bool TVector<Data>::Swap(int index1,int index2)
	{
	Data tmp=mArray[index1];
	mArray[index1]=mArray[index2];
	mArray[index2]=tmp;
	return true;
	}


/////////////////////////////////////////
template<typename Data>
bool TVector<Data>::Write(MIWriter &writer) const
	{
	if(writer.Write(mCount)==false)
		{
		return false;
		}

	int i;
	for(i=0;i<mCount;++i)
		{
		if(MIWriterWrite(writer,mArray[i])==false)
			{
			return false;
			}
		}

	return true;
	}

////////////////////////////////////////
template<typename Data>
bool TVector<Data>::Copy(Data *target,int len)
	{
	MStdAssert(mCount<=len && target!=NULL );
	int i;
	for(i=0;i<mCount;++i)
		{
		target[i] = mArray[i];
		}

	return true;
	}


///////////////////////////////////////
// Reverse the array contents
template<typename Data>
bool TVector<Data>::Reverse(void)
	{
	int begin,end;
	begin=0;
	end=mCount-1;
	while(begin<end)
		{
		if(Swap(begin,end)==false)
			{
			return false;
			}

		begin = begin + 1;
		end = end - 1;
		}

	return true;
	}


///////////////////////////////////////
template <typename Data>
bool TVector<Data>::Resize(int newsize)
	{
	MStdAssert(newsize>0);
	Data *newarray;
	newarray = new(std::nothrow) Data[newsize];
	if(newarray==NULL)
		{
		return false;
		}
		
	// Copy Old Data to new Memory
	const int mincopylength=MStdGetMin(newsize,mCount);
	int i;
	for(i=0;i<mincopylength;++i)
		{
		// Copy Constructor
		newarray[i] = mArray[i];
		}

	// Release old objects and memory
	delete[] mArray;

	// Point to new array
	mArray=newarray;
	mCount=newsize;

	return true;
	}


///////////////////////////////////////
template <typename Data>
bool TVector<Data>::operator=(const TVector<Data>  &copy)
	{
	if(Create(copy)==true) { return true; }
	return false;
	}

#endif //TVector_h

