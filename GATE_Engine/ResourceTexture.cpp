#include "ResourceTexture.h"

ResourceTexture::ResourceTexture(uint32 uid) : Resource(uid, Resource::TEXTURE)
{
}

ResourceTexture::~ResourceTexture()
{
}

void ResourceTexture::Save(json & config) const
{
}

void ResourceTexture::Load(const json & config)
{
}

bool ResourceTexture::LoadInMemory()
{
	return false;
}


