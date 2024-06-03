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


//v1.5 copyright Comine.com 20240603M1100
// This class holds keywords for the C,C++,C#,MSQL Languages
#ifndef MLangWordLists_h
#define MLangWordLists_h

/////////////////////////////////////////////////////////
#include "MStdLib.h"

/////////////////////////////////////////////////////////
class MLangWordLists
	{
	const char *(*mWordList)[];
	int mWordCount;
	bool mCaseInsensitive;

	/////////////////////////////////////////////////////
	void ClearObject(void);

	/////////////////////////////////////////////////////
	public:
	MLangWordLists(void);
	~MLangWordLists(void);
	bool Create(const char *language);	//language="C"|"C++"|"C#"|"MSQL"
	bool Destroy(void);
	int Search(const char *symbol);		// return index of item ==-1 if not found
	const char *Get(int index);			// Return a symbol at index
	int GetCount(void);					// Get Count of key words in the language
	};

#endif // MLangWordLists_h