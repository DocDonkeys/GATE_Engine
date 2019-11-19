#include "Component.h"
#include <string>

void Component::Save(json & file)
{
	file["UID"] = this->UID;
	file["Active"] = this->active;
}

void Component::Load(json & file)
{
}
