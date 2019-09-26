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
	void StartFrom(Uint32 secs);

	Uint32 Read() const;
	float ReadSec() const;

private:

	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
};

#endif //TIMER_H