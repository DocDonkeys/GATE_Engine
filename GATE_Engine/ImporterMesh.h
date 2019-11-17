#ifndef __IMPORTERMESH_H__
#define __IMPORTERMESH_H__

#include "Importer.h"

class Mesh;

struct IEMeshData : ImportExportData
{
	Mesh* mesh = nullptr;
};

class ImporterMesh : public Importer 
{
public:
	ImporterMesh();
	~ImporterMesh();

public:
	bool Export(const char* path, std::string& output_file, const ImportExportData* ie_data, const char* filename = "");
	bool Load(const char* full_path, Mesh* mesh);
};










#endif
