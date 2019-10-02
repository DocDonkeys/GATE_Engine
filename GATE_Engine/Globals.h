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

#define RETURNED_LOG(format, ...) returned_log(__FILE__, __LINE__, format, __VA_ARGS__);
const char* returned_log(const char file[], int line, const char* format, ...);


////Redefines NULL (Careful!)
//#ifdef NULL
//#undef NULL
//#endif
//#define NULL  0

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
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))	//If X is between lower and upper, returns X. Else, if X < lower, returns lower. Else, if X > upper, returns upper.
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define TO_BOOL( a )  ( (a != 0) ? true : false )

// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192

//https://wiki.libsdl.org/CategoryAssertions
//#define SDL_ASSERT_LEVEL 1

typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1024
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "GATE (Garrigo-Approved Template Engine)"