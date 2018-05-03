#pragma once

#define DLL_FUNCTION __declspec(dllexport)

///////////////////////////////////////////////////////////////////////////////
// FlexScan3D

#define FS3D_RESULT_OK            0
#define FS3D_RESULT_ERROR        -1
#define FS3D_RESULT_UNKNOWN      -2
#define FS3D_RESULT_EXECUTING    -3
#define FS3D_RESULT_WRONGTYPE    -4

typedef void* FS3D_Handle;

extern "C"
{
   DLL_FUNCTION int FS3D_Init(const char* a_PathName);
   DLL_FUNCTION int FS3D_Command(const char* a_Command);
   DLL_FUNCTION int FS3D_CommandAsync(const char* a_Command);
   DLL_FUNCTION int FS3D_AsyncResult();
   DLL_FUNCTION const char* FS3D_ScriptQuery(const char* a_Query);
   DLL_FUNCTION int FS3D_Abort();
   DLL_FUNCTION int FS3D_Exit();
   
   // if FlexScan3D is already running, use FS3D_Attach/FS3D_Detach instead of FS3D_Init/FS3D_Exit
   // NOTE: only FS3D_RegisterCallback and FS3D_UnregisterCallback are currently supported
   DLL_FUNCTION int FS3D_Attach();
   DLL_FUNCTION int FS3D_Detach();

   // callbacks
   DLL_FUNCTION int FS3D_RegisterCallback(const char* a_FunctionName, void* userContext, void (*a_Callback)(void* userContext, FS3D_Handle handle));
   DLL_FUNCTION int FS3D_UnregisterCallback(const char* a_FunctionName);
}
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Direct Memory Accessors
//
// The following functions can only be used within a user-defined callback.
// Pointers will only be valid within the scope of the callback function.
extern "C"
{
   // Get details on currently available items
   DLL_FUNCTION int FS3D_GetNumItems(const FS3D_Handle handle, int* numItems);
   DLL_FUNCTION int FS3D_GetItem(const FS3D_Handle handle, const int itemIndex, char** itemName, char** itemType);

   // Get values by item name and type
   DLL_FUNCTION int FS3D_GetString(const FS3D_Handle handle, const char* itemName, char** value);
   DLL_FUNCTION int FS3D_GetDouble(const FS3D_Handle handle, const char* itemName, double* value);
   DLL_FUNCTION int FS3D_GetFloat(const FS3D_Handle handle, const char* itemName, float* value);
   DLL_FUNCTION int FS3D_GetInt(const FS3D_Handle handle, const char* itemName, int* value);
   DLL_FUNCTION int FS3D_GetDoubleArray(const FS3D_Handle handle, const char* itemName, int* numValues, double** values);
   DLL_FUNCTION int FS3D_GetFloatArray(const FS3D_Handle handle, const char* itemName, int* numValues, float** values);
   DLL_FUNCTION int FS3D_GetIntArray(const FS3D_Handle handle, const char* itemName, int* numValues, int** values);
   DLL_FUNCTION int FS3D_GetByteArray(const FS3D_Handle handle, const char* itemName, int* numValues, unsigned char** values);
}

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Multi-computer configurations

extern "C"
{
   DLL_FUNCTION void FS3D_SetServer(const char* serverName);
}

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// stdcall wrappers (for languages which require stdcall instead of cdecl)
extern "C"
{
   DLL_FUNCTION int _stdcall FS3Ds_Init(const char* a_PathName);
   DLL_FUNCTION int _stdcall FS3Ds_Command(const char* a_Command);
   DLL_FUNCTION int _stdcall FS3Ds_CommandAsync(const char* a_Command);
   DLL_FUNCTION int _stdcall FS3Ds_AsyncResult();
   DLL_FUNCTION const char* _stdcall FS3Ds_ScriptQuery(const char* a_Query);
   DLL_FUNCTION int _stdcall FS3Ds_Abort();
   DLL_FUNCTION int _stdcall FS3Ds_Exit();
   
   DLL_FUNCTION int _stdcall FS3Ds_Attach();
   DLL_FUNCTION int _stdcall FS3Ds_Detach();

   DLL_FUNCTION int _stdcall FS3Ds_RegisterCallback(const char* a_FunctionName, void* userContext, void (*a_Callback)(void* userContext, FS3D_Handle handle));
   DLL_FUNCTION int _stdcall FS3Ds_UnregisterCallback(const char* a_FunctionName);
   

   DLL_FUNCTION int _stdcall FS3Ds_GetNumItems(const FS3D_Handle handle, int* numItems);
   DLL_FUNCTION int _stdcall FS3Ds_GetItem(const FS3D_Handle handle, const int itemIndex, char** itemName, char** itemType);
   
   DLL_FUNCTION int _stdcall FS3Ds_GetString(const FS3D_Handle handle, const char* itemName, char** value);
   DLL_FUNCTION int _stdcall FS3Ds_GetDouble(const FS3D_Handle handle, const char* itemName, double* value);
   DLL_FUNCTION int _stdcall FS3Ds_GetFloat(const FS3D_Handle handle, const char* itemName, float* value);
   DLL_FUNCTION int _stdcall FS3Ds_GetInt(const FS3D_Handle handle, const char* itemName, int* value);
   DLL_FUNCTION int _stdcall FS3Ds_GetDoubleArray(const FS3D_Handle handle, const char* itemName, int* numValues, double** values);
   DLL_FUNCTION int _stdcall FS3Ds_GetFloatArray(const FS3D_Handle handle, const char* itemName, int* numValues, float** values);
   DLL_FUNCTION int _stdcall FS3Ds_GetIntArray(const FS3D_Handle handle, const char* itemName, int* numValues, int** values);
   DLL_FUNCTION int _stdcall FS3Ds_GetByteArray(const FS3D_Handle handle, const char* itemName, int* numValues, unsigned char** values);

   
   DLL_FUNCTION void _stdcall FS3Ds_SetServer(const char* serverName);
}

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// KScan3D

#define KS3D_RESULT_OK            0
#define KS3D_RESULT_ERROR        -1
#define KS3D_RESULT_UNKNOWN      -2
#define KS3D_RESULT_EXECUTING    -3
#define KS3D_RESULT_WRONGTYPE    -4

extern "C"
{
   DLL_FUNCTION int KS3D_Init(const char* a_PathName);
   DLL_FUNCTION int KS3D_Command(const char* a_Command);
   DLL_FUNCTION int KS3D_CommandAsync(const char* a_Command);
   DLL_FUNCTION int KS3D_AsyncResult();
   DLL_FUNCTION const char* KS3D_ScriptQuery(const char* a_Query);
   DLL_FUNCTION int KS3D_Abort();
   DLL_FUNCTION int KS3D_Exit();
   
   // stdcall wrappers (for languages which require stdcall instead of cdecl)
   DLL_FUNCTION int _stdcall KS3Ds_Init(const char* a_PathName);
   DLL_FUNCTION int _stdcall KS3Ds_Command(const char* a_Command);
   DLL_FUNCTION int _stdcall KS3Ds_CommandAsync(const char* a_Command);
   DLL_FUNCTION int _stdcall KS3Ds_AsyncResult();
   DLL_FUNCTION const char* _stdcall KS3Ds_ScriptQuery(const char* a_Query);
   DLL_FUNCTION int _stdcall KS3Ds_Abort();
   DLL_FUNCTION int _stdcall KS3Ds_Exit();
}
///////////////////////////////////////////////////////////////////////////////