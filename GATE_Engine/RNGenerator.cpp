//#include "RNGenerator.h"
//
////Constructor & Destructor
//RNGenerator::RNGenerator()
//{
//
//}
//
//RNGenerator::~RNGenerator()
//{
//
//}
//
////Free RNG
//int RNGenerator::GetRandInt()
//{
//	return intDis(randGenerator);
//}
//
//float RNGenerator::GetRandFloat()
//{
//	return floatDis(randGenerator);
//}
//
//double RNGenerator::GetRandDouble()
//{
//	return doubleDis(randGenerator);
//}
//
////Ranged RNG
//int RNGenerator::GetRangedRandInt(int min, int max)
//{
//	std::uniform_int_distribution<int> temp(min, max);
//	return temp(randGenerator);
//}
//
//float RNGenerator::GetRangedRandFloat(float min, float max)
//{
//	std::uniform_real_distribution<float> temp(min, max);
//	return temp(randGenerator);
//}
//
//double RNGenerator::GetRangedRandDouble(double min, double max)
//{
//	std::uniform_real_distribution<double> temp(min, max);
//	return temp(randGenerator);
//}
