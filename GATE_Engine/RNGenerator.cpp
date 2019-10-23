#include <chrono>
#include "RNGenerator.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

//Constructor & Destructor
RNGenerator::RNGenerator() : randEngine(std::chrono::system_clock::now().time_since_epoch().count()) {}
RNGenerator::RNGenerator(unsigned seed) : randEngine(seed) {}

RNGenerator::~RNGenerator() {}

//Int RNG Methods
int RNGenerator::RandInt()
{
	return intDist(randEngine);
}

int RNGenerator::RangedRandInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(randEngine);
}

std::vector<int> RNGenerator::RandIntList(int size)
{
	std::vector<int> vec;
	std::uniform_int_distribution<int> dist;

	for (int i = 0; i < size; i++) {
		vec.push_back(dist(randEngine));
	}

	return vec;
}

std::vector<int> RNGenerator::RangedRandIntList(int min, int max, int size)
{
	std::vector<int> vec;
	std::uniform_int_distribution<int> dist(min, max);

	for (int i = 0; i < size; i++) {
		vec.push_back(dist(randEngine));
	}

	return vec;
}
