
//
// UnWindows.h
//
// $Id: //poco/svn/Foundation/include/Poco/UnWindows.h#2 $
//
// Library: Foundation
// Package: Core
// Module:  UnWindows
//
// A wrapper around the "Poco/UnWindows.h" header file that #undef's some
// of the macros for function names defined by "Poco/UnWindows.h" that
// are a frequent source of conflicts (e.g., GetUserName).
//
// Remember, that most of the WIN32 API functions come in two variants,
// an Unicode variant (e.g., GetUserNameA) and an ASCII variant (GetUserNameW).
// There is also a macro (GetUserName) that's either defined to be the Unicode
// name or the ASCII name, depending on whether the UNICODE macro is #define'd
// or not. POCO always calls the Unicode or ASCII functions directly (depending
// on whether MOOS_POCO_WIN32_UTF8 is #define'd or not), so the macros are not
// ignored.
//
// These macro definitions are a frequent case of problems and naming conflicts,
// especially for C++ programmers. Say, you define a class with a member
// function named GetUserName. Depending on whether "Poco/UnWindows.h" has been
// included by a particular translation unit or not, this might be changed to
// GetUserNameA/GetUserNameW, or not. While, due to naming conventions used,
// this is less of a problem in POCO, some of the users of POCO might use a
// different naming convention where this can become a problem.
//
// To disable the #undef's, compile POCO with the POCO_NO_UNWINDOWS macro
// #define'd.
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifdef WIN32
#include <windows.h>
#endif

#if !defined(POCO_NO_UNWINDOWS)
// A list of annoying macros to #undef.
// Feel free to extend as required.
#undef GetBinaryType
#undef GetShortPathName
#undef GetLongPathName
#undef GetEnvironmentStrings
#undef SetEnvironmentStrings
#undef FreeEnvironmentStrings
#undef FormatMessage
#undef EncryptFile
#undef DecryptFile
#undef CreateMutex
#undef OpenMutex
#undef CreateEvent
#undef OpenEvent
#undef CreateSemaphore
#undef OpenSemaphore
#undef LoadLibrary
#undef GetModuleFileName
#undef CreateProcess
#undef GetCommandLine
#undef GetEnvironmentVariable
#undef SetEnvironmentVariable
#undef ExpandEnvironmentStrings
#undef OutputDebugString
#undef FindResource
#undef UpdateResource
#undef FindAtom
#undef AddAtom
#undef GetSystemDirector
#undef GetTempPath
#undef GetTempFileName
#undef SetCurrentDirectory
#undef GetCurrentDirectory
#undef CreateDirectory
#undef RemoveDirectory
#undef CreateFile
#undef DeleteFile
#undef SearchPath
#undef CopyFile
#undef MoveFile
#undef ReplaceFile
#undef GetComputerName
#undef SetComputerName
#undef GetUserName
#undef LogonUser
#undef GetVersion
#undef GetObject
#endif // POCO_NO_UNWINDOWS
