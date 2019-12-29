// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"
#include "libs/SDL/include/SDL_timer.h"

// Memory Leak Detection
#include "MemLeaks.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
}

void Timer::StartFrom(uint secs)
{
	running = true;
	started_at = SDL_GetTicks() + secs * 1.000f;
}

void Timer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
}

void Timer::Resume()
{
	running = true;
	uint diff = SDL_GetTicks() - stopped_at;
	started_at += diff;
}

// ---------------------------------------------
uint Timer::Read() const
{
	if (running == true)
		return (SDL_GetTicks() - started_at);
	else
		return stopped_at - started_at;
}

// ---------------------------------------------
float Timer::ReadSec() const
{
	if (running == true)
		return (float)(SDL_GetTicks() - started_at) / 1000.0f;
	else
		return (float)(stopped_at - started_at) / 1000.0f;
}

float Timer::AlterateFlow(float multiplier)	//CHANGE/FIX: Doesn't work (the higher the multiplier the longer the lifespan when using this, it should to the opposite, but how?)
{
	float diff = Read() - Read() * multiplier;
	if (float((int)started_at - diff) > 0)
		started_at -= diff;	// By pushing started_at back and forth, we can manually change how much time has passed for the timer
	return diff;
}