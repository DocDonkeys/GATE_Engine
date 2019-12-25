#include "Resource.h"

Resource::Resource(uint32 uid, Resource::Type type) : UID(uid), type(type)
{
}

Resource::~Resource()
{
}

Resource::Type Resource::GetType() const
{
	return type;
}

uint32 Resource::GetUID() const
{
	return UID;
}

const char * Resource::GetFilePath() const
{
	return file_path.data();
}

const char * Resource::GetImportedFile() const
{
	return nullptr;
}

const char * Resource::GetName() const
{
	return name.data();
}

bool Resource::IsLoadedToMemory() const
{
	return loaded > 0;
}

bool Resource::LoadToMemory()
{
	return false;
}

uint Resource::CountReferences() const
{
	return references;
}

void Resource::Save(json & config) const
{
}

void Resource::Load(const json & config)
{
}
