#ifndef __COMPONENTSCRIPT_H__
#define __COMPONENTSCRIPT_H__

#include "Component.h"
class ResourceScript;
class ComponentScript : public Component
{
public:
	ComponentScript();
	~ComponentScript();

public:
	void Enable() override;
	void Disable() override;

	void Save(json &file);
	void Load(json &file);

public:
	ResourceScript* script = nullptr;
	std::string script_name;
};


#endif // !__COMPONENTSCRIPT_H__
