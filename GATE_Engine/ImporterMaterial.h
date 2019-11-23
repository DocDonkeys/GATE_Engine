#ifndef __IMPORTER_MATERIAL_H__
#define __IMPORTER_MATERIAL_H__

#include "Importer.h"

class ResourceTexture;
class ImporterMaterial : public Importer
{
public:
	ImporterMaterial();
	~ImporterMaterial();

public:
	bool Export(const char* path, std::string& output_file, const ImportExportData* ie_data, const char* filename = "");
	bool Load(const char* full_path, ResourceTexture* tex);
	bool CreateMeta(const char* original_file_full_path, ImportExportData* ie_data);
	bool LoadMeta(const char* full_path, bool game_path = false);
};


#endif
