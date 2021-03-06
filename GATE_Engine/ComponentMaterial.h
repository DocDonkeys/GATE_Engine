#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"
#include "Globals.h"
#include "libs/MathGeoLib/include/Math/float4.h"

class ResourceTexture;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	~ComponentMaterial();

public:
	void Enable() override;
	void Disable() override;

	void Save(json& file);
	void Load(json& file);

	//Assign a texture to a Component
	void AssignTexture(ResourceTexture* texture);

public:
	float4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
	ResourceTexture* active_texture = nullptr; //Currently used texture (either texture ID or checkers ID)
	ResourceTexture* loaded_texture = nullptr; //Loaded texture Id
	bool use_default_texture = false;
	uint checkers_texture_id = 0; //Checkers texture is our debug Texture
};

#endif // !__COMPONENTMATERIAL_H__
