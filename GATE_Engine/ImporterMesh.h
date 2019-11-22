#ifndef __IMPORTERMESH_H__
#define __IMPORTERMESH_H__

#include "Importer.h"

class ResourceMesh;

struct IEMeshData : ImportExportData
{
	ResourceMesh* mesh = nullptr;
};

class ImporterMesh : public Importer 
{
public:
	ImporterMesh();
	~ImporterMesh();

public:
	bool Export(const char* path, std::string& output_file, const ImportExportData* ie_data, const char* filename = "");
	bool Load(const char* full_path, ResourceMesh* mesh);
	bool CreateMeta(const char* original_file_full_path, IEMeshData* ie_data);
	bool GenMeshBuffers(ResourceMesh* mesh);
};










#endif
