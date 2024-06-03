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
#ifndef MBlockOps_h
#define MBlockOps_h

//******************************************************
//**  MBlockOps class
//******************************************************
class MBlockOps
	{
	int mBlockStartLine;					// Start Line of Blocked Region
	int mBlockStartLineOffset;				// Start Line Offset of Blocked Region
	int mBlockStopLine;						// Stop Line of Blocked Region
	int mBlockStopLineOffset;				// Stop Line Offset of Blocked Region
	bool mBlockStoped;						// Tracks if Last operation block is stopped
	
	////////////////////////////////////////////////
	void ClearObject(void);
	bool IsBlockOrdered(void);				// Check if start line comes before end
	void ReOrder(int &start,int &startoffset
			,int &end,int &endoffset);		// Reorder start/stop into proper order

	////////////////////////////////////////////////
	public:
	MBlockOps(void);							// Constructor
	~MBlockOps(void);							// Destructor
	bool Create(void);							// Create state of object
	bool Destroy(void);							// Destroy Current state of object
	bool BlockStart(int row,int column);		// Start position of a block
	bool BlockStop(int row,int column);			// Stop position of a block
	bool IsBlocked(void);						// Checks System is in blocked
	bool IsBlocked(int row,int column);			// Checks if system is blocked at row/col
	bool IsBlockStopped(void);					// Checks if Last operations caused block stop
	bool IsBlockInWindow(int rowmin,int rowmax);	// =true if block is in window
	bool GetBlockedRegion(int &start,int &startoffset
				,int &end,int &endoffset);		// Display the blocked region
	};

#endif // MBlockOps_h

