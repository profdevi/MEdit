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


//v0.12 copyright Comine.com 20240603M0807
#include <windows.h>
#include "MStdLib.h"
#include "MCommandArg.h"
#include "MBuffer.h"
#include "MWinEditor.h"

////////////////////////////////////////////////////////
static const char *GApplicationName="MEdit";
static const char *GApplicationVersion="0.12";


///////////////////////////////////////////////////
static void GDisplayHelp(void);

////////////////////////////////////////////////////
//int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR cmdline,int nshowcmd)
int main(int argn,const char *argv[],const char *env[])
	{
	MCommandArg args(argn,argv);

	MWinEditor editor;
	if(editor.Create()==false)
		{
		return 0;
		}

	// Set Editor Language
	const char *language;  int index;
	if(args.GetNameValue("-lang=",language,index)==true)
		{
		// Set Language
		editor.SetLanguage(language);
		args.RemoveArg(index);
		}
	else
		{
		// Set Default Language
		editor.SetLanguage("C++");
		}

	// Load the default file
	if(args.GetArgCount()>=2)
		{
		// Load up the file
		editor.LoadTextFile(args.GetArg(1));
		}
	else
		{
		GDisplayHelp();
		return 0;
		}

	// Set Text File Name
	MBuffer buffer(1000);
	MStdSPrintf(buffer.GetBuffer(),buffer.GetSize(),"%s v%s copyright Comine.com - %s"
			,GApplicationName,GApplicationVersion,args.GetArg(1) );
	SetWindowText(editor.GetHWND(),buffer.GetBuffer());


	editor.Show();
	
	editor.MessageLoop();

	if(editor.IsModified()==true 
				&& MessageBox(NULL,"Would you like to save the file?","Save File"
					,MB_YESNO|MB_DEFBUTTON1)==IDYES)
		{
		if(editor.SaveTextFile(args.GetArg(1))==false)
			{
			}
		}

	editor.Destroy();
	return 0;
	}


////////////////////////////////////////////////////
static void GDisplayHelp(void)
	{
	MBuffer buffer(1000);
	MStdSPrintf(buffer,buffer.GetSize(),
			"\n"
			"   usage:  %s <filename> <-lang=[C++|C#|C|MSQL|NONE> [-?]\n"
			"           v%s copyright Comine.com\n"
			"\n"
			"   Edits a text file with default formating for C++.\n"
			"\n"
			,GApplicationName,GApplicationVersion);

	MStdWindowOutput("Command Usage",buffer);
	}

