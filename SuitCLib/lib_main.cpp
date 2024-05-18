
extern "C"
{
#include "header.h"
}


char WideToNarrowChar(wchar_t w)
{
	char* chars = new char[5];
	chars[2] = (char)w;
	return chars[2];
}


extern "C"
__declspec(dllexport)
int 
__cdecl

//returnvalue
// 0: OK
// 1: Error
call_suitc(wchar_t** ppcharInput, int inputSize, 
		   wchar_t** ppcharOutput, int* pCharOutputSize, 
		   wchar_t** ppcharOptimumDetails, int* pcharOptimumDetailsSize)
{
	int* argLen = new int[100];
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

	char output[JSON_BUFFER_SIZE];
	output[0] = '\0';
	//strncat(test, "abc", 20);

	char optimumDetails[JSON_BUFFER_SIZE];
	optimumDetails[0] = '\0';


	suitc_main(argc, argv, output, optimumDetails);

	int outputLen = strlen(output);
	int count = 0;
	for (int i = 0; i < outputLen; i++)
	{
		(*ppcharOutput)[i] = output[i];
	}
	(*ppcharOutput)[outputLen] = L'\0';
	*pCharOutputSize = outputLen;

	outputLen = strlen(optimumDetails);
	count = 0;
	for (int i = 0; i < outputLen; i++)
	{
		(*ppcharOptimumDetails)[i] = optimumDetails[i];
	}
	(*ppcharOptimumDetails)[outputLen] = L'\0';
	*pcharOptimumDetailsSize = outputLen;



	return 0;
}


extern "C"             //No name mangling
__declspec(dllexport)  //Tells the compiler to export the function
void                    //Function return type     
__cdecl                //Specifies calling convention, cdelc is default, 
					   //so this can be omitted 

test_suitc()
{
	//-EK A 2 KQJT9876543
	int argc = 5;

	char** argv = new char* [argc];
	argv[0] = new char[6];
	strcpy(argv[0], "Dummy");

	argv[1] = new char[50];
	strcpy(argv[1], "-EK");

	argv[2] = new char[50];
	strcpy(argv[2], "A");

	argv[3] = new char[50];
	strcpy(argv[3], "2");

	argv[4] = new char[50];
	strcpy(argv[4], "KQJT9876543");

	char output[JSON_BUFFER_SIZE];
	output[0] = '\0';
	//strncat(test, "abc", 20);
	char optimumdetails[JSON_BUFFER_SIZE];
	optimumdetails[0] = '\0';

	suitc_main(argc, argv, output, optimumdetails);
}


