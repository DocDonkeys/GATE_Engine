#include "ComponentScript.h"
#include "ResourceScript.h"

ComponentScript::ComponentScript() : Component()
{
	type = COMPONENT_TYPE::SCRIPT;
}

ComponentScript::~ComponentScript() 
{
}

void ComponentScript::Enable()
{
	active = true;
}

void ComponentScript::Disable()
{
	active = false;
}

void ComponentScript::Save(json & file)
{
}

void ComponentScript::Load(json & file)
{
}
