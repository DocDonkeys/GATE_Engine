#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"
#include "Globals.h"

class Texture;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	~ComponentMaterial();

public:
	void Enable() override;
	void Update() override;
	void Disable() override;

	//Assign a texture to a Component
	void AssignTexture(Texture* texture);

public:
	Texture* active_texture = nullptr; //Currently used texture (either texture ID or checkers ID)
	Texture* loaded_texture = nullptr; //Loaded texture Id
	
private:
	uint checkers_texture_id = 0; //Checkers texture is our debug Texture
};

#endif // !__COMPONENTMATERIAL_H__
