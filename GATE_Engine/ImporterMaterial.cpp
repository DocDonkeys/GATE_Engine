#include "ImporterMaterial.h"
#include "Texture.h"

ImporterMaterial::ImporterMaterial()
{
}

ImporterMaterial::~ImporterMaterial()
{
}

bool ImporterMaterial::Export(const char * path, std::string & output_file, const ImportExportData * ie_data, const char * filename)
{
	return false;
}

bool ImporterMaterial::Load(const char * full_path, Texture* tex)
{
	return false;
}
