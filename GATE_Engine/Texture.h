#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "Globals.h"

class Texture {	//CHANGE/FIX: Move to component system as a part of material
public:
	Texture() {};
	Texture(uint id, const char* filename) : id(id), filename(filename) {};

	uint id = 0;
	std::string filename;
};

#endif	//!TEXTURE_H