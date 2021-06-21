//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
//----------------------------------------------------------------------------- 
// FILE_IO  
//    v.2.1.0
//    v.2.8   - warning 5039 windows.h
//    v.2.9   - fence
//    v.2.10  - msc_ver 1914, win 10.0.17
//    v.2.11  - warning fixes 
//    v.2.12  - mutex on all trace::out
//    v.2.13  - fixed warnings on mutex and default copy const / assignment
//----------------------------------------------------------------------------- 

#include <stdio.h>
#include <assert.h>
#include <string.h>

// locals
#include "FileIO.h"


FileIO::FileIO()
	:fileHandle(0)
{
};

FileIO::~FileIO()
{
	if (0 != fileHandle)
	{
		this->privClose();
	}
};

void FileIO::Open(const char * const pFirstName, const char * const pLastName)
{
	assert(pFirstName);
	assert(pLastName);
	privGetInstance()->privOpen(pFirstName, pLastName);
}

void FileIO::Close()
{
	privGetInstance()->privClose();
}

FILE *FileIO::GetHandle()
{
	return privGetInstance()->privGetHandle();
}

FileIO *FileIO::privGetInstance()
{
	static FileIO instance;
	return &instance;
}

void FileIO::privClose()
{
	errno_t fileError;
	assert(fileHandle);

	fileError = fflush(fileHandle);
	assert(!fileError);

	fileError = fclose(fileHandle);
	fileHandle = 0;
	assert(!fileError);
}

void FileIO::privOpen(const char * const pFirstName, const char * const pLastName)
{
	const int length = 256;
	
	char *pFileName = new char[length];
	assert(pFileName);

	errno_t fileError;
	
	// wash the name to 0
	memset(pFileName, 0, length);
	assert(strlen(pFirstName) < length/4);
	assert(strlen(pLastName) < length/4);

	strcat_s(pFileName, length, pFirstName);
	strcat_s(pFileName, length, pLastName);
	strcat_s(pFileName, length, "_");

#ifdef _DEBUG
	const char *debug_string = "Debug.txt";
	strcat_s(pFileName, length, debug_string);
	const char *mode = "DEBUG";
#endif

#ifdef NDEBUG
	const char *debug_string = "Release.txt";
	strcat_s(pFileName, length, debug_string);
	const char *mode = "RELEASE";
#endif

	fileError = fopen_s(&fileHandle, pFileName, "wt");
	assert(fileHandle);

	fprintf(fileHandle, "Program Assignment \n");
	fprintf(fileHandle, "Name: %s%s\n", pFirstName, pLastName);
	fprintf(fileHandle, "Mode: %s\n", mode);
	fprintf(fileHandle, "\n");
	fprintf(fileHandle, "-------------------------------------------------\n");
	fprintf(fileHandle, "\n");

	delete pFileName;
}


FILE *FileIO::privGetHandle()
{
	assert(fileHandle);
	return fileHandle;
}

// ---  End of File ---------------

