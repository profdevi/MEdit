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


//v1.10 copyright Comine.com 20240603M1032
#include "MStdLib.h"
#include "MMemory.h"
#include "MEditBuffer.h"

//******************************************************
//**  Module Elements
//******************************************************
static const int GMemoryBlockSize=64;

///////////////////////////////////////////////////
struct MEditBufferLine
	{
	MEditBufferElement *Row;			// Point to a row of data
	int Length;							// Number of Row Elements
	int MaxLength;						// Maximum Size before a resize
	};


//////////////////////////////////////////////////////
static int GGetBlocksForElement(int maxlength)
	{  return (sizeof(MEditBufferElement)*maxlength-1)/GMemoryBlockSize +1; }


//////////////////////////////////////////////////////
static int GGetBlocksForLines(int maxlength)
	{  return (sizeof(MEditBufferLine)*maxlength-1)/GMemoryBlockSize + 1;  }


///////////////////////////////////////////////////////
static int GGetMaxElementsForBlock(void)
	{  return GMemoryBlockSize/sizeof(MEditBufferElement);  }


//////////////////////////////////////////////////////
static int GGetMaxRowsForBlock(void)
	{  return GMemoryBlockSize/sizeof(MEditBufferLine);  }
	

//******************************************************
//**  MEditBufferElement class
//******************************************************
int MEditBufferElement::GetTextType(void)
	{
	// Low Nibble
	return ((unsigned int)Type) & 0x0F;
	}


///////////////////////////////////////////////////////
int MEditBufferElement::GetBackType(void)
	{
	// High Nibble
	return (((unsigned int)Type) >>4) & 0x0F;
	}


///////////////////////////////////////////////////////
bool MEditBufferElement::SetTextType(int value)
	{
	// Low Nibble
	Type=((Type & 0xF0) | (value & 0x0F) );
	return true;
	}


///////////////////////////////////////////////////////
bool MEditBufferElement::SetBackType(int value)
	{
	// High Nibble
	Type=((Type & 0x0F) | ((value & 0x0F)<<4) );
	return true;
	}


//******************************************************
//**  MEditBuffer class
//******************************************************
void MEditBuffer::ClearObject(void)
	{
	mRows=NULL; mLength=0;  mMaxLength=0;
	}


/////////////////////////////////////////////////
bool MEditBuffer::ReMapPosition(int &row,int &col)
	{
	if(mLength<=0)
		{
		//=We cannot remap a position from an empty buffer
		return false;
		}

	//=Atlease One Line Exists

	if(row<0)
		{
		row=0;
		col=0;

		if(mRows[0].Length<=0)
			{ return false; }

		return true;
		}
	else if(row>=mLength)
		{
		row=mLength-1;
		while(row>=0 && mRows[row].Length==0)
			{  row=row-1;  }

		if(row>=0 && mRows[row].Length>0)
			{
			col=mRows[row].Length-1;
			return true;
			}

		return false;
		}

	//=We have a correct row number

	// Check if row length makes sense
	if(mRows[row].Length>0)
		{
		if(col<0)
			{  col=0;  }
		else if(col>=mRows[row].Length)
			{  col=mRows[row].Length-1; }

		return true;
		}

	// Current Row does not have any lines
	col=0;

	// Move to a row which does have a length
	while(row<mLength && mRows[row].Length<=0)
		{
		row=row+1;
		}

	if(row>=mLength)
		{
		// Nothing to set
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MEditBuffer::ReOrder(int &startrow,int &startcol
		,int &endrow,int &endcol)
	{
	if(endrow<startrow)
		{
		int tmp=startrow;
		startrow=endrow;
		endrow=tmp;

		tmp=startcol;
		startcol=endcol;
		endcol=tmp;
		}
	else if(endrow==startrow)
		{
		if(startcol>endcol)
			{  
			int tmp;
			tmp=startcol;
			startcol=endcol;
			endcol=tmp;
			return true;
			}

		return true;
		}

	return true;
	}


/////////////////////////////////////////////
MEditBuffer::MEditBuffer(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MEditBuffer::~MEditBuffer(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MEditBuffer::Create(void)
	{
	Destroy();

	// Allocate space for mRows
	mMaxLength=GGetMaxRowsForBlock();
	if(mMaxLength<=0)
		{
		Destroy();  return false;
		}

	mLength=0;
	mRows=(MEditBufferLine *)MMemory::Alloc(GMemoryBlockSize,__FILE__,__LINE__);
	if(mRows==NULL)
		{
		Destroy();  return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MEditBuffer::Destroy(void)
	{
	if(mRows!=NULL)
		{
		for(int i=0;i<mLength;++i)
			{
			if(mRows[i].Row!=NULL)
				{ MMemory::Free(mRows[i].Row);  }
			}

		MMemory::Free(mRows);
		mRows=NULL;
		}

	
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
MEditBufferElement *MEditBuffer::Get(int row,int col)
	{
	int length=mRows[row].Length;
	return mRows[row].Row+col;
	}


////////////////////////////////////////////////
int MEditBuffer::GetRows(void)
	{
	return mLength;
	}


////////////////////////////////////////////////
int MEditBuffer::GetCols(int row)
	{
	if(row<0 || row>=mLength)
		{  return 0; }

	return mRows[row].Length;
	}


////////////////////////////////////////////////
bool MEditBuffer::AppendRow(void)
	{
	// Check if space already exists in rows
	if(mLength<mMaxLength)
		{
		MStdMemSet(mRows+mLength,0,sizeof(MEditBufferLine));
		mLength=mLength+1;
		return true;
		}

	// Need to resize whole array
	int newblocksneeded=GGetBlocksForLines(mMaxLength)+1;

	MEditBufferLine *newbuffer;
	newbuffer=(MEditBufferLine *)MMemory::Realloc(mRows,newblocksneeded*GMemoryBlockSize
			,__FILE__,__LINE__);
	if(newbuffer==NULL)
		{
		return false;
		}
	
	// We got the memory block

	mRows=newbuffer;
	mMaxLength=newblocksneeded*GMemoryBlockSize/sizeof(MEditBufferLine);

	// Add the new element
	MStdMemSet(mRows+mLength,0,sizeof(MEditBufferLine));
	mLength=mLength+1;
	return true;
	}


/////////////////////////////////////////////////
bool MEditBuffer::AppendRow(int rowdst,int rowsrc)		// append a row src/row dst
	{
	// Get Reference to each line
	MEditBufferLine *srcline=mRows+rowsrc;
	MEditBufferLine *dstline=mRows+rowdst;

	int newlength=dstline->Length+srcline->Length;

	// Check if we have enough space in the current block
	if(newlength<=dstline->MaxLength)
		{
		if(srcline->Length>0)
			{
			MStdMemCpy(dstline->Row+dstline->Length,srcline->Row
				,srcline->Length * sizeof(MEditBufferElement) );
			}

		dstline->Length=newlength;
		return true;
		}

	//We Need More Space in Current Block
	MEditBufferElement *newblock=(MEditBufferElement *)MMemory::Realloc(dstline->Row
				,GGetBlocksForElement(newlength)*GMemoryBlockSize,__FILE__,__LINE__);

	if(newblock==NULL)
		{
		return false;
		}

	dstline->Row=newblock;

	// Append the extra data
	MStdMemCpy(dstline->Row+dstline->Length,srcline->Row
		,srcline->Length * sizeof(MEditBufferElement) );
	dstline->Length=newlength;
	dstline->MaxLength=GGetBlocksForElement(newlength)
				*GMemoryBlockSize/sizeof(MEditBufferElement);
	return true;
	}


////////////////////////////////////////////////
bool MEditBuffer::AppendCell(int row)
	{
	MEditBufferLine *line=mRows+row;
	int maxlength=line->MaxLength;
	int length=line->Length;

	// Check if space exists to extend length
	if(length<maxlength)
		{
		MStdMemSet(line->Row+length,0,sizeof(MEditBufferElement) );
		line->Length = line->Length + 1;
		return true;
		}

	// Need to append another memory block

	//Check if line is empty
	if(line->Row==NULL) //
		{
		line->Row=(MEditBufferElement *)MMemory::Alloc(
				GGetMaxElementsForBlock()*sizeof(MEditBufferElement),__FILE__
				,__LINE__);
		if(line->Row==NULL)
			{
			return false;
			}

		line->Length=1;  line->MaxLength=GGetMaxElementsForBlock();
		MStdMemSet(line->Row,0,sizeof(MEditBufferElement) );
		return true;
		}

	// We Need to extend the block
	int newblockcount=GGetBlocksForElement(line->MaxLength)+1;
	MEditBufferElement *newblock;
	newblock=(MEditBufferElement *)MMemory::Realloc(line->Row
			,newblockcount*GMemoryBlockSize,__FILE__,__LINE__);
	if(newblock==NULL)
		{
		return false;
		}

	// Old data has been copied
	line->Row=newblock;
	MStdMemSet((line->Row)+(line->Length),0,sizeof(MEditBufferElement) );
	line->Length += 1;
	line->MaxLength = newblockcount*GMemoryBlockSize/sizeof(MEditBufferElement);
	return true;
	}


/////////////////////////////////////////////////////////////
bool MEditBuffer::Insert(int row)
	{
	if(row<0 || row>mLength)
		{
		return false;
		}
	
	if(AppendRow()==false)
		{
		return false;
		}

	// Save last element
	MEditBufferLine lineelement=mRows[mLength-1];

	// Move over every row by 1
	MStdMemMove(mRows+row+1,mRows+row,(mLength-row-1)*sizeof(MEditBufferLine) );

	mRows[row]=lineelement;
	return true;
	}


///////////////////////////////////////////////////////////////
bool MEditBuffer::InsertCell(int row,int cell)
	{
	// If Last Empty line
	if(row==mLength)
		{
		if(AppendRow()==false)
			{
			return false;
			}
		}
	
	MEditBufferLine *line=mRows+row;

	if(AppendCell(row)==false)
		{
		return false;
		}

	// 
	MEditBufferElement lastelement;
	lastelement=line->Row[line->Length-1];

	// Move Over the elements
	MStdMemMove((line->Row)+cell+1,(line->Row)+cell
			,((line->Length)-cell-1)*sizeof(MEditBufferElement) );

	line->Row[cell]=lastelement;
	return true;
	}


//////////////////////////////////////////////////////////////////
bool MEditBuffer::RemoveRow(int row)
	{
	MEditBufferLine *line=mRows+row;
	if(line->Row!=NULL) {  MMemory::Free(line->Row); }
	MStdMemMove(mRows+row,mRows+row+1,sizeof(MEditBufferLine)*(mLength-row-1) );
	mLength=mLength-1;
	return true;
	}


//////////////////////////////////////////////////////////////////
bool MEditBuffer::RemoveCell(int row,int cell)
	{
	MEditBufferLine *line=mRows+row;

	// If last character, then append last line
	if(line->Length==cell)
		{
		// Check if there is a new line
		if(row+1==mLength)
			{
			if(mLength>0) { return false; }

			MMemory::Free(mRows[row].Row); // Release current line
			mLength=mLength-1;  return true;
			}

		// First append next line to end of current line
		if(AppendRow(row,row+1)==false) {  return false;  }

		// Remove the next line
		return RemoveRow(row+1);
		}


	MStdMemCpy(line->Row+cell,line->Row+cell+1
			,(line->Length-cell-1)*sizeof(MEditBufferElement));
	line->Length=line->Length-1;
	return true;
	}


///////////////////////////////////////////////////////////////
bool MEditBuffer::SplitLine(int row,int cell) // Split a line at current position
	{
	if(row==mLength) { return AppendRow();  }

	MEditBufferLine *line=mRows+row;

	if(Insert(row+1)==false)
		{
		return false;
		}

	// Update line reference. We may have a new memory block
	line=mRows+row;
	if(line->Length==0) { return true; }

	int charcount=line->Length-cell;
	
	MEditBufferElement *srcline=Get(row,0);
	MEditBufferElement *dstelement;

	// Append to next line
	int i;
	for(i=0;i<charcount;++i)
		{
		if(AppendCell(row+1)==false)
			{
			return false;
			}

		dstelement=Get(row+1,i);
		*dstelement=srcline[cell+i]; // Bitwise copy whole struct
		}


	// Delete End characters
	for(i=0;i<charcount;++i)
		{
		if(RemoveCell(row,cell)==false)
			{
			return false;
			}
		}

	return true;
	}


///////////////////////////////////////////////////////
bool MEditBuffer::SetTextType(int value)
	{
	int i;
	for(i=0;i<mLength;++i)
		{
		int  length=mRows[i].Length;
		MEditBufferElement *array=mRows[i].Row;
		
		if(array==NULL)
			{
			continue;
			}
		
		for(int k=0;k<length;++k)
			{
			array[k].SetTextType(value);
			}
		}

	return true;
	}


/////////////////////////////////////////////////
bool MEditBuffer::SetBackType(int value)
	{
	int i;
	for(i=0;i<mLength;++i)
		{
		int  length=mRows[i].Length;
		MEditBufferElement *array=mRows[i].Row;
		
		if(array==NULL)
			{
			continue;
			}
		
		for(int k=0;k<length;++k)
			{
			array[k].SetBackType(value);
			}
		}

	return true;
	}


///////////////////////////////////////////////////
bool MEditBuffer::SetBackType(int value
		,int startrow,int startcol
		,int endrow,int endcolumn )
	{
	if(ReOrder(startrow,startcol,endrow, endcolumn)==false)
		{
		return false;
		}

	if(ReMapPosition(startrow,startcol)==false
			|| ReMapPosition(endrow, endcolumn)==false)
		{
		return false;
		}

	//=We have correct ordering

	// Check if on one line
	if(startrow==endrow)
		{
		int i;
		for(i=startcol;i<= endcolumn;++i)
			{
			MEditBufferElement *element=Get(startrow,i);
			if(element==NULL)
				{
				return false;
				}

			element->SetBackType(value);
			}

		return true;
		}

	//=We have more then 1 line

	// Finish through the whole line
	int i;
	for(i=startcol;i< mRows[startrow].Length;++i)
		{
		MEditBufferElement *element=Get(startrow,i);
		if(element==NULL)
			{
			return false;
			}

		element->SetBackType(value);
		}

	//=We have finished the first line

	// Fill In All of the in middle lines (if any)
	for(i=startrow+1;i<=endrow-1;++i)
		{
		int k;
		for(k=0;k<mRows[i].Length;++k)
			{
			MEditBufferElement *element=Get(i,k);
			if(element==NULL)
				{
				return false;
				}

			element->SetBackType(value);
			}
		}

	for(i=0;i< endcolumn;++i)
		{
		MEditBufferElement *element=Get(endrow,i);
		if(element==NULL)
			{
			return false;
			}

		element->SetBackType(value);
		}

	return true;
	}


/////////////////////////////////////////////////
bool MEditBuffer::SetTextType(int value,int startrow,int startcol
		,int endrow,int endcolumn)
	{
	if(ReOrder(startrow,startcol,endrow, endcolumn)==false)
		{
		return false;
		}

	if(ReMapPosition(startrow,startcol)==false
			|| ReMapPosition(endrow, endcolumn)==false)
		{
		return false;
		}

	//=We have correct ordering

	// Check if on one line
	if(startrow==endrow)
		{
		int i;
		for(i=startcol;i<= endcolumn;++i)
			{
			MEditBufferElement *element=Get(startrow,i);
			if(element==NULL)
				{
				return false;
				}

			element->SetTextType(value);
			}
		}

	//=We have more then 1 line

	// Finish through the whole line
	int i;
	for(i=startcol;i< mRows[startrow].Length;++i)
		{
		MEditBufferElement *element=Get(startrow,i);
		if(element==NULL)
			{
			return false;
			}

		element->SetTextType(value);
		}

	//=We have finished the first line

	// Fill In All of the in middle lines (if any)
	for(i=startrow+1;i<=endrow-1;++i)
		{
		int k;
		for(k=0;k<mRows[i].Length;++k)
			{
			MEditBufferElement *element=Get(startrow,i);
			if(element==NULL)
				{
				return false;
				}

			element->SetTextType(value);
			}
		}

	for(i=0;i< endcolumn;++i)
		{
		MEditBufferElement *element=Get(endrow,i);
		if(element==NULL)
			{
			return false;
			}

		element->SetTextType(value);
		}

	return true;
	}

