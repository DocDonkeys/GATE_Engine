#include "ModuleResources.h"
#include "Application.h"
#include "Resource.h"

ModuleResources::ModuleResources(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{

}

ModuleResources::~ModuleResources()
{
}

uint32 ModuleResources::Find(const char * file_in_assets) const
{
	return 0;
}

uint32 ModuleResources::ImportFile(const char * new_file_in_assets, bool force)
{
	return 0;
}

uint32 ModuleResources::GenerateNewUID()
{
	return 0;
}

const Resource * ModuleResources::Get(uint32 uid) const
{
	return nullptr;
}

Resource * ModuleResources::Get(uint32 uid)
{
	return nullptr;
}

Resource * ModuleResources::CreateNewResource(Resource::Type type, uint32 force_uid)
{
	return nullptr;
}
