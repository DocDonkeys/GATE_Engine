#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(uint32 uid) : Resource(uid,Resource::MESH)
{
}

ResourceMesh::~ResourceMesh()
{
}

bool ResourceMesh::LoadInMemory()
{
	return false;
}

void ResourceMesh::Save(json & config) const
{
}

void ResourceMesh::Load(const json & config)
{
}
