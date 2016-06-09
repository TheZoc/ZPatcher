// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#endif // _WIN32

#include "wx/wxprec.h"

#ifdef _WIN32
	#include "targetver.h"
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#include <windows.h>
	#include <malloc.h>
	#include <tchar.h>
#endif // _WIN32

// STD C++ Header Files
#include <string>

// C RunTime Header Files
#include <stdlib.h>
#include <memory.h>

// Every wx header we use.
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/gauge.h>
#include <wx/gbsizer.h>
#include <wx/frame.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <wx/msgdlg.h>

// TODO: reference additional headers your program requires here
