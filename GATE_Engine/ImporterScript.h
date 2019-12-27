#ifndef __IMPORTERSCRIPT_H__
#define __IMPORTERSCRIPT_H__

#include "Importer.h"

class ComponentTransform;
class GameObject;

class ImporterScript : public Importer
{
public:
	ImporterScript();
	~ImporterScript();
public:
	virtual bool CreateMeta(const char* original_file_full_path, ImportExportData* ie_data);
	virtual bool LoadMeta(const char* full_path, bool game_path);
	bool ImportableResource(const char* full_path);

public:
	ImportExportData ie_data;
};

#endif  //__IMPORTERSCRIPT_H__
