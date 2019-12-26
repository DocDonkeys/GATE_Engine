#ifndef __RESOURCESCRIPT_H__
#define __RESOURCESCRIPT_H__

#include "Resource.h"

class ResourceScript : public Resource
{
public:
	ResourceScript(uint32 uid);
	~ResourceScript();

public:
	void Save(json& config) const override;
	void Load(const json& config) override;
	bool LoadInMemory() override;
public:
	std::string absolute_path;
	std::string relative_path;
	std::string script_name;

};
#endif // !__RESOURCESCRIPT_H__

