#ifndef TIMER_H
#define TIMER_H

#include "Globals.h"
#include "libs/SDL/include/SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();
	void StartFrom(uint secs);

	uint Read() const;
	float ReadSec() const;

private:

	bool	running;
	uint	started_at;
	uint	stopped_at;
};

#endif //TIMER_H