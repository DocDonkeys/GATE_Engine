#pragma once

#include <windows.h>
#include <stdio.h>

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

// Disable STL exceptions
//#ifndef _HAS_EXCEPTIONS
//#define _HAS_EXCEPTIONS 0
//#endif
//#define _STATIC_CPPLIB

//LOG
#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);
void log(const char file[], int line, const char* format, ...);

////Redefines NULL (Careful!)
//#ifdef NULL
//#undef NULL
//#endif
//#define NULL 0

// Deletes a buffer
#define RELEASE( x ) \
    {                        \
    if( x != NULL )        \
	    {                      \
      delete x;            \
	  x = NULL;              \
	    }                      \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                              \
    if( x != NULL )              \
	    {                            \
      delete[] x;                \
	  x = NULL;                    \
	    }                            \
                              \
    }

//Utilities
#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )	//Returns if within range
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))	//If X is between lower and upper, returns X. Else, if X < lower, returns lower. Else, if X > upper, returns upper.
#define TO_BOOL( a )  ( (a != 0) ? true : false )

// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192

//https://wiki.libsdl.org/CategoryAssertions
//#define SDL_ASSERT_LEVEL 1

// Typedefs
typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;
typedef signed __int64		int64;
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned __int64	ulong;

// Update flags
enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

//Memory Leak Detection
#define _MMGR_MEM_LEAK
//#define _VISUAL_MEM_LEAK

//Only arbitrary value that can't be managed by JSON
#define CONFIG_FILENAME "settings/config.json"
#define DEFAULT_CONFIG_FILENAME "settings/default_config.json"

//COMMENTS DOCUMENTATION
/*
--> TODO: Usual "needs to be done", but instead we use the following one as many other imported libraries use the same keywords.
--> CHANGE/FIX: Personal flag for something that needs to change pronto, either important or directly non-optional.
--> IMPROVE: Flag for a piece of code that could be improved, however it is not essential and can be ignored without much trouble.
--> NOTE: Important information about a piece of code, system, or functionality.
--> WARNING: Warning about a malfunction, a potential problem if code is edited carelessly, or any other information which can prevent future mistakes.
--> ALERT: Same as above.
*/