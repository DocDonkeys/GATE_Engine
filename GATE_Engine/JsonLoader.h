#ifndef JSONLOADER_H
#define JSONLOADER_H

#include "libs/json/json.hpp"
using json = nlohmann::json;

class JsonLoader
{
public:
	// Constructor
	JsonLoader() {};

	bool Save(json obj, const char* file) const;
	json Load(const char* file) const;
};

#endif //TIMER_H