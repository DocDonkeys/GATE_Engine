#ifndef RNGENERATOR_H
#define RNGENERATOR_H

#include <random>

class RNGenerator
{
public:
	RNGenerator();
	RNGenerator(unsigned seed);
	~RNGenerator();

public:
	int RandInt();
	int RangedRandInt(int min, int max);
	std::vector<int> RandIntList(int num);
	std::vector<int> RangedRandIntList(int min, int max, int num);

	template <class T>
	T Rand()
	{
		std::uniform_real_distribution<T> dist;
		return dist(randEngine);
	}

	template <class T>
	T RangedRand(T min, T max)
	{
		std::uniform_real_distribution<T> dist(min, max);
		return dist(randEngine);
	}

	template <class T>
	std::vector<T> RandVec(int size)
	{
		std::vector<T> vec;
		std::uniform_real_distribution<T> dist;

		for (int i = 0; i < size; i++) {
			vec.push_back(dist(randEngine));
		}
		
		return vec;
	}

	template <class T>
	std::vector<T> RandRangedVec(T min, T max, int size)
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
	std::uniform_int_distribution<int> intDist;
};

#endif //RNGENERATOR_H