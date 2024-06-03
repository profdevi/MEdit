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
#include "MStdLib.h"
#include "MMemory.h"
#include "MLangWordLists.h"


//*****************************************************************
//* Module Elements
//*****************************************************************
static const char *GCLanguage[]=
		{
		"if","else","void","long","int","unsigned","short","while","do"
		,"for","char","float","double","static","const","volatile"
		,"break","continue","switch","case","default","return",
		
		// Preprocessing Statements
		"include","ifdef","define","ifndef","undef","elif","endif"
		};


////////////////////////////////////////////////////////
static const char *GCSharpLanguage[]=
	{
	"abstract","as","base","break","byte","case","catch","char","checked",
	"class","const","continue","decimal","default","delegate","do",
	"double","else","enum","event","explicit","extern","false","finally",
	"fixed","float","for","foreach","goto","if","implicit","in","int",
	"interface","internal","is","lock","long","namespace","new","null",
	"object","operator","out","override","params","partial","private","protected",
	"public","readonly","ref","return","sbyte","sealed","short","sizeof",
	"stackalloc","static","string","struct","switch","this","throw",
	"true","try","typeof","uint","ulong","unchecked","unsafe","ushort",
	"using","virtual","volatile","void","while",

	// Preprocessing Statements
	"elif","endif","define","undef","warning","error","line",
	"region","endregion"
	};


////////////////////////////////////////////////////////
static const char *GCPlusPlusLanguage[]=
	{
	"__abstract","__alignof","__asm","__assume",
	"__based","__box","__cdecl","__declspec",
	"__delegate","__event","__except","__fastcall",
	"__finally","__forceinline","__gc","__hook",
	"__identifier","__if_exists","__if_not_exists","__inline",
	"__int8","__int16","__int32","__int64",
	"__interface","__leave","__m64","__m128",
	"__m128d","__m128i","__multiple_inheritance","__nogc",
	"__noop","__pin","__property","__raise",
	"__sealed","__single_inheritance","__stdcall","__super",
	"__try_cast","__try","__unhook","__uuidof",
	"__value","__virtual_inheritance","__w64","bool",
	"break","case","catch","char",
	"class","const","const_cast","continue",
	"default","delete","deprecated","dllexport",
	"dllimport","do","double","dynamic_cast",
	"else","enum","explicit","extern",
	"false","float","for","friend",
	"goto","if","inline","int",
	"long","mutable","naked","namespace",
	"new","noinline","noreturn","nothrow",
	"novtable","operator","private","property",
	"protected","public","register","reinterpret_cast",
	"return","selectany","short","signed",
	"sizeof","static","static_cast","struct",
	"switch","template","this","thread",
	"throw","true","try","typedef",
	"typeid","typename","union","unsigned",
	"using","uuid","virtual","void",
	"volatile","__wchar_t","while",

	// Preprocessing Statements
	"include","ifdef","define","ifndef","undef","elif","endif"
	};


////////////////////////////////////////////////////////
static const char *GMSQLLanguage[]=
	{
	"identity","encryption","order",
	"add","end","outer",
	"all","errlvl","over",
	"alter","escape","percent",
	"and","except","plan",
	"any","exec","precision",
	"as","execute","primary",
	"asc","exists","print",
	"authorization","exit","proc",
	"avg","expression","procedure",
	"backup","fetch","public",
	"begin","file","raiserror",
	"between","fillfactor","read",
	"break","for","readtext",
	"browse","foreign","reconfigure",
	"bulk","freetext","references",
	"by","freetexttable","replication",
	"cascade","from","restore",
	"case","full","restrict",
	"check","function","return",
	"checkpoint","goto","revoke",
	"close","grant","right",
	"clustered","group","rollback",
	"coalesce","having","rowcount",
	"collate","holdlock","rowguidcol",
	"column","identity","rule",
	"commit","identity_insert","save",
	"compute","identitycol","schema",
	"constraint","if","select",
	"contains","in","session_user",
	"containstable","index","set",
	"continue","inner","setuser",
	"convert","insert","shutdown",
	"count","intersect","some",
	"create","into","statistics",
	"cross","is","sum",
	"current","join","system_user",
	"current_date","key","table",
	"current_time","kill","textsize",
	"current_timestamp","left","then",
	"current_user","like","to",
	"cursor","lineno","top",
	"database","load","tran",
	"databasepassword","max","transaction",
	"dateadd","min","trigger",
	"datediff","national","truncate",
	"datename","nocheck","tsequal",
	"datepart","nonclustered","union",
	"dbcc","not","unique",
	"deallocate","null","update",
	"declare","nullif","updatetext",
	"default","of","use",
	"delete","off","user",
	"deny","offsets","values",
	"desc","on","varying",
	"disk","open","view",
	"distinct","opendatasource","waitfor",
	"distributed","openquery","when",
	"double","openrowset","where",
	"drop","openxml","while",
	"dump","option","with",
	"else","or","writetext"
	};


//*****************************************************
//** MLangWordLists
//*****************************************************
void MLangWordLists::ClearObject(void)
	{
	mWordList=0;
	mWordCount=0;
	mCaseInsensitive=false;
	}


///////////////////////////////////////////////////////
MLangWordLists::MLangWordLists(void)
	{  ClearObject();  }


///////////////////////////////////////////////////////
MLangWordLists::~MLangWordLists(void)
	{  Destroy();  }


///////////////////////////////////////////////////////
bool MLangWordLists::Create(const char *language)
	{
	Destroy();

	// Init temp variables
	const char **tablepointer;
	int tablesize;

	if(MStdStrICmp(language,"C#")==0)
		{
		tablepointer=(const char **)&GCSharpLanguage;
		tablesize=sizeof(GCSharpLanguage)/sizeof(char*);
		}
	else if(MStdStrICmp(language,"C++")==0)
		{
		tablepointer=(const char **)&GCPlusPlusLanguage;
		tablesize=sizeof(GCPlusPlusLanguage)/sizeof(char*);
		}
	else if(MStdStrICmp(language,"C")==0)
		{
		tablepointer=(const char **)&GCLanguage;
		tablesize=sizeof(GCLanguage)/sizeof(char*);		
		}
	else if(MStdStrICmp(language,"MSQL")==0)
		{
		tablepointer=(const char **)&GMSQLLanguage;
		tablesize=sizeof(GMSQLLanguage)/sizeof(char*);		
		mCaseInsensitive=true;
		}
	else
		{
		Destroy();
		return false;
		}

	// Allocate Space for strings
	mWordList=(const char *(*)[])MMemory::Alloc(sizeof(char *)*tablesize
			,__FILE__,__LINE__);
	if(mWordList==NULL)
		{
		Destroy();
		return false;
		}

	mWordCount=tablesize;

	// Copy the string into new space
	int i;
	for(i=0;i<mWordCount;++i)
		{
		(*mWordList)[i]=tablepointer[i];
		}

	// Now we have to sort the items
	for(i=0;i<mWordCount-1;++i)
		{
		int k;
		for(k=i+1;k<mWordCount;++k)
			{
			if(MStdStrICmp((*mWordList)[i],(*mWordList)[k])>0)
				{
				const char *tmp=(*mWordList)[i];
				(*mWordList)[i]=(*mWordList)[k];
				(*mWordList)[k]=tmp;
				}
			}
		}
	return true;
	}


//////////////////////////////////////////////////////
bool MLangWordLists::Destroy(void)
	{
	if(mWordList!=NULL)
		{
		MMemory::Free(mWordList);
		mWordList=NULL;
		}

	ClearObject();
	return true;
	}


///////////////////////////////////////////////////////
int MLangWordLists::Search(const char *item)
	{
	int minindex=0;
	int maxindex=mWordCount-1;

	if(mCaseInsensitive==true)
		{
		for(;;)
			{
			int middle;
			middle=(minindex+maxindex)/2;
			
			int ret=MStdStrICmp((*mWordList)[middle],item);
			if(ret==0)
				{  return middle;  }

			if(minindex>=maxindex)
				{  return -1;  }

			else if(ret<0)
				{  minindex=middle+1;  }
			else
				{  maxindex=middle-1;  }
			}
		}
	else
		{
		for(;;)
			{
			int middle;
			middle=(minindex+maxindex)/2;
			
			int ret=MStdStrCmp((*mWordList)[middle],item);
			if(ret==0)
				{  return middle;  }

			if(minindex>=maxindex)
				{  return -1;  }

			else if(ret<0)
				{  minindex=middle+1;  }
			else
				{  maxindex=middle-1;  }
			}
		}

	return -1;
	}


///////////////////////////////////////////////////////
const char *MLangWordLists::Get(int index)
	{
	if(index<0 || index>=mWordCount)
		{
		return NULL;
		}

	return (*mWordList)[index];
	}


///////////////////////////////////////////////////////
int MLangWordLists::GetCount(void)
	{
	return mWordCount;
	}


