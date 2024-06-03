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
#ifndef MEditBuffer_h
#define MEditBuffer_h

//******************************************************
//**  MEditBuffer Element Structure
//******************************************************
struct MEditBufferElement
	{
	char Value;									// Character Value
	char Type;									// Character Type
	
	/////////////////////////////////////////////////////
	int GetTextType(void);						// Get Text Type
	int GetBackType(void);						// Get Text Type
	bool SetTextType(int value);				// Set Text Type
	bool SetBackType(int value);				// Set Text Type
	};


//******************************************************
//**  MEditBuffer class
//******************************************************
struct MEditBufferLine;
class MEditBuffer
	{
	MEditBufferLine *mRows;						// Holds Array of lines
	int mLength;								// Number of rows in buffer
	int mMaxLength;								// Maximum Buffer Size

	////////////////////////////////////////////////
	void ClearObject(void);
	bool ReMapPosition(int &row,int &col);		// Reset Position to correct item
	bool ReOrder(int &startrow,int &startcol
			,int &endrow,int &endcol);
	
	////////////////////////////////////////////////
	public:
	MEditBuffer(void);
	~MEditBuffer(void);
	bool Create(void);
	bool Destroy(void);
	MEditBufferElement *Get(int row,int col);
	int GetRows(void);							// =Total rows in buffer
	int GetCols(int row);						// =Gets cells in each row
	bool AppendRow(void);						// append a blank row to end
	bool AppendRow(int rowdst,int rowsrc);		// append a row src/row dst
	bool AppendCell(int row);					// append a blank cell
	bool Insert(int row);						// row can equal==mLength(same as append)
	bool InsertCell(int row,int cell);			// Inserts a new cell
	bool RemoveRow(int row);					// removes the current row
	bool RemoveCell(int row,int cell);			// removes the current cell
	bool SplitLine(int row,int cell);			// Split a line at current position
	bool SetTextType(int value);				// Set the text type for document
	bool SetBackType(int value);				// Set the text type for document
	bool SetBackType(int value,int startrow,int startcol
			,int endrow,int endcol);
	bool SetTextType(int value,int startrow,int startcol
			,int endrow,int endcol);
	};	


#endif // MEditBuffer_h
