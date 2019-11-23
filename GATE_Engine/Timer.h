#ifndef TIMER_H
#define TIMER_H

#include "Globals.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void StartFrom(uint secs);
	void Stop();

	void Resume();

	uint Read() const;
	float ReadSec() const;

private:

	bool running;
	uint started_at;
	uint stopped_at;
	uint paused_at;
};

#endif //TIMER_H