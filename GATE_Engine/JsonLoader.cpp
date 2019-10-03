#include "JsonLoader.h"
#include "Globals.h"

#include "libs/SDL/include/SDL_assert.h"

#include <fstream>
#include <iomanip>

bool JsonLoader::Save(json obj, const char* file)
{
	bool ret = true;

	if (file == nullptr) {
		SDL_TriggerBreakpoint();	//TODO: Prompt a "Save As" UI
	}
	else {
		std::ofstream stream(file);
		SDL_assert(stream.is_open());

		stream << std::setw(4) << obj << std::endl;
		stream.close();
	}
	
	return ret;
}

json JsonLoader::Load(const char* file) const
{
	SDL_assert(file != nullptr);

	json obj;
	std::ifstream stream(file);
	SDL_assert(stream.is_open());

	try {
		obj = json::parse(stream);
	}
	catch (json::parse_error& e) {
		LOG("Parse Error while Loading File: %c", e.what());
	}

	stream.close();

	return obj;
}