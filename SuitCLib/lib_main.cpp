//SuitC
//Copyright 2020, Gerrit A. van der Velde, Leiden
// 
//Windows library version created by
//Lars Kappel Hansen
//https://bridge.esmarkkappel.dk

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#define CALLCONV __cdecl
#else
#define EXPORT __attribute__((visibility("default")))
#define CALLCONV
#endif

extern "C" EXPORT const char* CALLCONV

version()
{
	static const char ver[] = "1.0.0.1";  // Ensure memory remains valid
	return ver;
}

extern "C"
{
#include "header.h"
}
#include <signal.h>

extern "C" EXPORT void CALLCONV
free_memory()
{
	suitc_free_memory();
}

char WideToNarrowChar(wchar_t w)
{
	//char* chars = new char[5];
	//chars[2] = (char)w;
	//return chars[2];
	return (char)w;
}

void signal_handler(int sig)
{
	printf("Exception caught: Segmentation fault (signal %d)!\n", sig);
	exit(1);  // Exit safely
}

//returnvalue
// 0: OK
// 1: Error
extern "C" EXPORT int CALLCONV
 call_suitc(wchar_t** ppcharInput, int inputSize, 
		   wchar_t** ppcharOutput, int* pCharOutputSize, 
		   wchar_t** ppcharOptimumDetails, int* pcharOptimumDetailsSize)
{
	signal(SIGSEGV, signal_handler);  // Catch segmentation faults
	int* argLen = new int[1000];
	int argNo = 0;
	bool searching = true;
	for (int i = 0; i < inputSize; i++)
	{
		if (searching)
		{
			if ((*ppcharInput)[i] != L' ')
			{
				//found and entry
				argNo++;
				argLen[argNo] = 1;
				searching = false;
			}
			else // == ' '
			{
				//keep searching
			}
		}
		else //!searching
		{
			if ((*ppcharInput)[i] != L' ')
			{
				//keep reading argument
				(argLen[argNo])++;
			}
			else // == ' '
			{
				//start search for new parameter
				searching = true;
			}
		}
	}
	int argc = argNo + 1;

	if (argc == 0)
	{
		*pCharOutputSize = 0;
		return 1;
	}
	char** argv = new char* [argc];
	argv[0] = new char[6];
	strcpy(argv[0], "Dummy");
	for (int i = 1; i < argc; i++)
	{
		argv[i] = new char[argLen[i] + 1];
	}

	searching = true;
	argNo = 0;
	int pos = 0;
	for (int i = 0; i < inputSize; i++)
	{
		if (searching)
		{
			if ((*ppcharInput)[i] != L' ')
			{
				//found and entry
				argNo++;
				searching = false;
				pos = 0;
				(argv[argNo])[pos] = WideToNarrowChar((*ppcharInput)[i]);
			}
			else // == ' '
			{
				//keep searching
			}
		}
		else //!searching
		{
			if ((*ppcharInput)[i] != L' ')
			{
				//keep reading argument
				pos++;
				(argv[argNo])[pos] = WideToNarrowChar((*ppcharInput)[i]);
			}
			else // == ' '
			{
				//start search for new parameter
				searching = true;
			}
		}
	}

	for (int i = 1; i < argc; i++)
	{
		(argv[i])[argLen[i]] = '\0';
	}

	// static, not on the stack: 2 x 128 KB overflows small host thread stacks
	// (an IIS request thread has 256 KB). suitc_main is not reentrant anyway.
	static char output[JSON_BUFFER_SIZE];
	output[0] = '\0';
	//strncat(test, "abc", 20);

	static char optimumDetails[JSON_BUFFER_SIZE];
	optimumDetails[0] = '\0';


	int suitcResult = suitc_main(argc, argv, output, optimumDetails);
	int outputLen = strlen(output);
	for (int i = 0; i < outputLen; i++)
	{
		(*ppcharOutput)[i] = output[i];
	}
	(*ppcharOutput)[outputLen] = L'\0';
	*pCharOutputSize = outputLen;

	outputLen = strlen(optimumDetails);
	for (int i = 0; i < outputLen; i++)
	{
		(*ppcharOptimumDetails)[i] = optimumDetails[i];
	}
	(*ppcharOptimumDetails)[outputLen] = L'\0';
	*pcharOptimumDetailsSize = outputLen;

	for (int i = 0; i < argc; i++) { // Note: argv[0] was also allocated
		delete[] argv[i];
	}
	delete[] argv;
	delete[] argLen;

	return suitcResult == 0 ? 0 : 1;
}



