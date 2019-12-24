#include "ComponentScript.h"

ComponentScript::ComponentScript() : Component()
{
	type = COMPONENT_TYPE::SCRIPT;
}

ComponentScript::~ComponentScript() 
{
}

void ComponentScript::Enable()
{
}

void ComponentScript::Disable()
{
}

void ComponentScript::Save(json & file)
{
}

void ComponentScript::Load(json & file)
{
}
