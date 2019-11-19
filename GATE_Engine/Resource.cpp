#include "Resource.h"

Resource::Resource(uint32 uid, Resource::Type type)
{
}

Resource::~Resource()
{
}

Resource::Type Resource::GetType() const
{
	return Resource::Type();
}

uint32 Resource::GetUID() const
{
	return uint32();
}

const char * Resource::GetFile() const
{
	return nullptr;
}

const char * Resource::GetImportedFile() const
{
	return nullptr;
}

bool Resource::IsLoadedToMemory() const
{
	return false;
}

bool Resource::LoadToMemory()
{
	return false;
}

uint Resource::CountReferences() const
{
	return uint();
}

void Resource::Save(json & config) const
{
}

void Resource::Load(const json & config)
{
}
