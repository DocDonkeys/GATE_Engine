#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"
#include "Globals.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	~ComponentMaterial();

public:
	void Enable() override;
	void Update() override;
	void Disable() override;

public:
	uint active_texture_id = 0;

private:
	uint texture_id = 0;
	uint checkers_texture_id = 0; //Checkers texture is our debug Texture
};

#endif // !__COMPONENTMATERIAL_H__
