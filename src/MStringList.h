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
#ifndef MStringList_h
#define MStringList_h

///////////////////////////////////////////////////
class MStringList
	{
	/////////////////////////////////////////////
	struct GNode *mFirstNode;
	struct GNode *mLastNode;
	struct GNode *mCurrent;
	int mCount;

	//////////////////////////////////////////////
	void ClearObject(void);

	//////////////////////////////////////////////
	public:
	MStringList(void);
	MStringList(bool create);
	MStringList(MStringList &reflist);	// Initialize from another list
	~MStringList(void);
	bool Create(void);
	bool Create(MStringList &reflist);	// Initialize from another list
	bool Destroy(void);
	bool AddString(const char *string); // Add to the end of the list
	bool AddList(MStringList &list);	// Add existing list
	bool ReadReset(void);				// Will Reset to the first item again
	const char *ReadString(void);		// Read a string untill NULL
	bool IsMember(const char *string,bool casematch=true);	// Check if string is a member
	int GetCount(void);					// Get Count of strings
	bool Remove(const char *string);	// Remove the string from list
	bool Swap(MStringList &list);		// Swap Two Lists
	bool Print(void);					// Print out strings
	};

#endif // MStringList_h

