#ifndef FRAMERATEMANAGER_H
#define FRAMERATEMANAGER_H

#include "Globals.h"
#include "Timer.h"

class FramerateManager
{
public:
	void Start();
	void Stop();
	void Resume();

	void FrameStart(float dtMultiplier = 1.f);
	void FrameEnd();

public:
	uint	frame_count = 0;			// Total frame count
	Timer	time_since_start;			// Time since first execution (Overall time since start)
	Timer	frame_time;					// Time since last frame start (Time taken by a single frame)
	Timer	sec_frame_time;				// Single-second timer

	uint	curr_sec_frame_count = 0;	// Frame count per current second
	uint	prev_sec_frame_count = 0;	// Stored frame count from last second
	float	dt = 0.0f;					// Delta time


	// For performance information purposes
	float avg_fps;
	float secs_since_start;
	uint frame_ms;
};

#endif //FRAMERATEMANAGER_H