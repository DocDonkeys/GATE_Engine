#ifndef __IMPORTER_MATERIAL_H__
#define __IMPORTER_MATERIAL_H__

#include "Importer.h"

class Texture;
class ImporterMaterial : public Importer
{
public:
	ImporterMaterial();
	~ImporterMaterial();

public:
	bool Export(const char* path, std::string& output_file, const ImportExportData* ie_data, const char* filename = "");
	bool Load(const char* full_path, Texture* tex);
};


#endif
