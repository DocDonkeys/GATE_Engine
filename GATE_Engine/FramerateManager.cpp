#include "FramerateManager.h"
#include "Application.h"

// Brofiler
#include "libs/Brofiler/Brofiler.h"

void FramerateManager::Start()
{
	time_since_start.Start();
	frame_time.Start();
	sec_frame_time.Start();
}

void FramerateManager::Stop()
{
	time_since_start.Stop();
	frame_time.Stop();
	sec_frame_time.Stop();
	prev_sec_frame_count = curr_sec_frame_count;
	frame_count = curr_sec_frame_count = 0;
}

void FramerateManager::Resume()
{
	time_since_start.Resume();
	frame_time.Resume();
	sec_frame_time.Resume();
}

void FramerateManager::FrameStart(float dtMultiplier)
{
	frame_count++;
	curr_sec_frame_count++;

	dt = (float)frame_time.ReadSec() * dtMultiplier;
	frame_time.Start();
}

void FramerateManager::FrameEnd()
{
	//Framerate Calcs
	if (sec_frame_time.Read() > 1000) {
		sec_frame_time.Start();
		prev_sec_frame_count = curr_sec_frame_count;
		curr_sec_frame_count = 0;
	}

	//For performance information purposes
	avg_fps = float(frame_count) / time_since_start.ReadSec();
	secs_since_start = time_since_start.ReadSec();
	frame_ms = frame_time.Read();
}