#include <chrono>
#include "RNGenerator.h"

//Constructor & Destructor
RNGenerator::RNGenerator() : randEngine(std::chrono::system_clock::now().time_since_epoch().count()) {}
RNGenerator::RNGenerator(unsigned seed) : randEngine(seed) {}
RNGenerator::~RNGenerator() {}