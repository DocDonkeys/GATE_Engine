#ifndef PERFTIMER_H
#define PERFTIMER_H

#include "Globals.h"

class PerfTimer
{
public:

	// Constructor
	PerfTimer();

	void Start();
	double ReadMs() const;
	ulong ReadTicks() const;

private:
	ulong			started_at;
	static ulong	frequency;
};

#endif //PERFTIMER_H