#ifndef RNGenerator_H
#define RNGenerator_H

#include "Globals.h"

#include <random>
#include <cmath>
#include "pcg-cpp-0.98/include/pcg_random.hpp"

class RNGenerator
{
public:
	// Constructor & Destructor
	RNGenerator()
	{
		SetRandEngine();
		LOG("Randomly-chosen mean: %d", mean);
	};

	RNGenerator(float mean_min, float mean_max, float deviation)
	{
		SetRandEngine(mean_min, mean_max, deviation);
		LOG("Randomly-chosen mean: %d", mean);
	};

	~RNGenerator() {};

private:
	void SetRandEngine(float mean_min = 1, float mean_max = 6, float deviation = 2)
	{
		//Seed with a real random value, if available
		pcg_extras::seed_seq_from<std::random_device> seed_source;

		//Make a random number engine
		rng(seed_source);

		//Choose a random mean between the set limits
		std::uniform_int_distribution<float> uniform_dist(mean_min, mean_max);

		//Get normal distribution parameters
		mean = uniform_dist(rng);
		this->deviation = deviation;
	}

	float GetRandFloat()
	{
		//Generate a normal distribution around the mean
		std::normal_distribution<> normal_dist(mean, deviation);

		//Make a copy of the RNG state to use later
		rng_checkpoint = rng;
		
		return normal_dist(rng);
	}

	int GetRandInt()
	{
		return std::round(GetRandFloat());
	}

	float GetLimitedFloat(float min, float max)
	{
		float ret = GetRandFloat();
		return fmod(ret, (max - min + 1)) + min;	//TODO: Make sure the resulting number is always under the rand number be able to do the module?
	}

	int GetLimitedInt(int min, int max)
	{
		int ret = GetRandInt();
		return ret % (max - min + 1) + min;	//TODO: Make sure the resulting number is always under the rand number be able to do the module?
	}

	void GetRandList()
	{

	}

	void GetRandLimitedList()
	{

	}

private:
	pcg32 rng;				// RNG Engine, to set the basis for the RNG
	pcg32 rng_checkpoint;	// RNG Recorder, to measure the amount of numbers needed for a given result
	float mean;			// Randomly chosen mean, for a normal distribution
	float deviation;		// Deviation from the mean in the normal distribution

};

#endif // RNGenerator_H

//Original Reference code from PCG C++
/*
// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine
	pcg32 rng(seed_source);

	// Choose a random mean between 1 and 6
	std::uniform_int_distribution<int> uniform_dist(1, 6);
	int mean = uniform_dist(rng);
	std::cout << "Randomly-chosen mean: " << mean << '\n';

	// Generate a normal distribution around that mean
	std::normal_distribution<> normal_dist(mean, 2);

	// Make a copy of the RNG state to use later
	pcg32 rng_checkpoint = rng;

	// Produce histogram
	std::map<int, int> hist;
	for (int n = 0; n < 10000; ++n) {
		++hist[std::round(normal_dist(rng))];
	}
	std::cout << "Normal distribution around " << mean << ":\n";
	for (auto p : hist) {
		std::cout << std::fixed << std::setprecision(1) << std::setw(2)
				  << p.first << ' ' << std::string(p.second/30, '*') << '\n';
	}

	// Produce information about RNG usage
	std::cout << "Required " << (rng - rng_checkpoint) << " random numbers.\n";
*/