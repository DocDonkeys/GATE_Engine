#include "ResourceScript.h"

ResourceScript::ResourceScript(uint32 uid) : Resource(uid, Resource::MESH)
{
}

ResourceScript::~ResourceScript()
{
}

void ResourceScript::Save(json & config) const
{
}

void ResourceScript::Load(const json & config)
{
}

bool ResourceScript::LoadInMemory()
{
	return false;
}
