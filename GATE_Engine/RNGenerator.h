#ifndef RNGENERATOR_H
#define RNGENERATOR_H

#include <random>
//#include "libs/MathGeoLib/include/Algorithm/Random/LCG.h"	// MathGeoLib alternative

class RNGenerator
{
public:
	RNGenerator();
	RNGenerator(unsigned seed);
	~RNGenerator();

public:
	// Int, Uint, Long
	template <class T>
	T RandInt()
	{
		std::uniform_int_distribution<T> dist;
		return dist(randEngine);
	}

	template <class T>
	T RangedRandInt(T min, T max)
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(randEngine);
	}

	template <class T>
	std::vector<T> RandVecInt(int size)
	{
		std::vector<T> vec;
		std::uniform_int_distribution<T> dist;

		for (int i = 0; i < size; i++) {
			vec.push_back(dist(randEngine));
		}

		return vec;
	}

	template <class T>
	std::vector<T> RandRangedVecInt(T min, T max, int size)
	{
		std::vector<T> vec;
		std::uniform_int_distribution<T> dist(min, max);

		for (int i = 0; i < size; i++) {
			vec.push_back(dist(randEngine));
		}

		return vec;
	}

	// Float, Dobule, Long Double
	template <class T>
	T RandDouble()
	{
		std::uniform_real_distribution<T> dist;
		return dist(randEngine);
	}

	template <class T>
	T RangedRandDouble(T min, T max)
	{
		std::uniform_real_distribution<T> dist(min, max);
		return dist(randEngine);
	}

	template <class T>
	std::vector<T> RandVecDouble(int size)
	{
		std::vector<T> vec;
		std::uniform_real_distribution<T> dist;

		for (int i = 0; i < size; i++) {
			vec.push_back(dist(randEngine));
		}
		
		return vec;
	}

	template <class T>
	std::vector<T> RandRangedVecDouble(T min, T max, int size)
	{
		std::vector<T> vec;
		std::uniform_real_distribution<T> dist(min, max);

		for (int i = 0; i < size; i++) {
			vec.push_back(dist(randEngine));
		}

		return vec;
	}

private:
	std::default_random_engine randEngine;
	//math::LCG randEngine;
};

#endif //RNGENERATOR_H