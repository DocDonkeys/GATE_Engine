#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Globals.h"
#include "Resource.h"
#include "JsonLoader.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(uint32 uid);
	~ResourceTexture();

public:
	void Save(json& config) const override;
	void Load(const json& config) override;
	bool LoadInMemory() override;

public:
	uint id = 0;
};

#endif // !__RESOURCE_TEXTURE_H__

