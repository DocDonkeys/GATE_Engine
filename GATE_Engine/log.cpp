#pragma once
#include "Globals.h"
#include "Application.h"

// Memory Leak Detection
#include "MemLeaks.h"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap = nullptr;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);
	
	if (App != NULL)
	{
		sprintf_s(tmp_string2, 4096, "\n%s", tmp_string);
		App->ConsoleLOG(tmp_string2);
	}
}
