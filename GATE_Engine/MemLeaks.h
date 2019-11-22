#pragma once

// Settings
#define _MMGR_MEM_LEAK
//#define _VISUAL_MEM_LEAK

// Setup
#ifdef _DEBUG

#ifdef _MMGR_MEM_LEAK
//#define TEST_MEMORY_MANAGER
#include "libs/mmgr/mmgr.h"
#else
#ifdef _VISUAL_MEM_LEAK
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#endif

// Deletes a buffer
template<class T>
inline void RELEASE(T*& ptr) { if (ptr != nullptr) { delete ptr; ptr = nullptr; } }

// Deletes an array of buffers
template<class T>
inline void RELEASE_ARRAY(T*& ptr) { if (ptr != nullptr) { delete[] ptr; ptr = nullptr; } }