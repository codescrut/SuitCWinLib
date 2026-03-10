//SuitC
//Copyright 2020, Gerrit A. van der Velde, Leiden
// 
//Windows library version created by
//Lars Kappel Hansen
//https://bridge.esmarkkappel.dk


// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#ifdef _WIN32
    BOOL APIENTRY DllMain( HMODULE hModule,
        DWORD  ul_reason_for_call,
        LPVOID lpReserved
    )
    {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    break;
    }
    return TRUE;
}
#else
#endif


