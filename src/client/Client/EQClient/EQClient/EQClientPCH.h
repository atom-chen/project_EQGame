/******************************************************************************
* 作者: 
* 时间: 
* 描述: 
******************************************************************************/

#pragma warning(disable:4819)

#ifndef EQClientPCH_H
#define EQClientPCH_H

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

//
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// reference additional headers your program requires here

#include "EQMainPrerequisites.h"

//一些不变又常用的头文件
#include "EQException.h"
#include "EQLog.h"


#ifdef _DEBUG
	#	pragma comment( lib,"OgreMain_d.lib" )
	#	pragma comment( lib,"OIS_d.lib" )
	#	pragma comment( lib,"CEGUIBase_d.lib" )
	#	pragma comment( lib,"CEGUILua_d.lib" )
	#	pragma comment( lib,"lua_d.lib" )
	#	pragma comment( lib,"QWBlogAPI.lib" )
	//#	pragma comment( lib,"luabind-0.8.1.lib" )
	#	pragma comment( lib,"fmodvc.lib" )
    #	pragma comment( lib,"ParticleUniverse_d.lib" )

#else
	#	pragma comment( lib, "OgreMain.lib" )
	#	pragma comment( lib, "OIS.lib" )
	#	pragma comment( lib,"CEGUIBase.lib" )
	#	pragma comment( lib,"CEGUILua.lib" )
	#	pragma comment( lib,"lua.lib" )
	#	pragma comment( lib,"QWBlogAPI.lib" )
	//#	pragma comment( lib,"luabind-0.8.1.lib" )
	#	pragma comment( lib,"fmodvc.lib" )
	#	pragma comment( lib,"ParticleUniverse.lib" )
#endif

#pragma comment(lib,"ws2_32.lib")

void DebugMessage(const char *msg,...);

#endif