#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include <string>
class Texture;

class Importer { 
public:
	Importer();
	~Importer();
public:
	bool Import(const char* file, const char* path, std::string& output_file); 
	bool Import(const void* buffer, uint size, std::string& output_file);
	bool Load(const char* exported_file, Texture* resource); 
	bool LoadCheckers(Texture* resource); 
};

#endif