#ifndef __COMPONENTSCRIPT_H__
#define __COMPONENTSCRIPT_H__

#include "Component.h"

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
};


#endif // !__COMPONENTSCRIPT_H__
