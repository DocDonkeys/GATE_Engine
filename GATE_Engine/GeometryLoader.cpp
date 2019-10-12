#include "GeometryLoader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "libs/Assimp/include/cimport.h"
#include "libs/Assimp/include/scene.h"
#include "libs/Assimp/include/postprocess.h"
#include "libs/Assimp/include/cfileio.h"

#pragma comment (lib, "libs/Assimp/libx86/assimp.lib")

//#pragma comment (lib, "libs/assimp-5.0.0/libx86/assimp.lib")
/*
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h" 
#include "Assimp/include/postprocess.h" 
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")
*/

GeometryLoader::GeometryLoader(Application * app, const char * name, bool start_enabled) : Module(app, name, start_enabled)
{
}

GeometryLoader::~GeometryLoader()
{
}

bool GeometryLoader::CleanUp()
{
	bool ret = true;

	// detach log stream 
	aiDetachAllLogStreams();

	//Delete data
	for (int i = 0; i < meshes.size(); ++i)
	{
		Mesh_Data* m_todestroy = meshes[i];
		//Delete the allocated memory data inside the mesh
		App->renderer3D->DeleteBuffer(m_todestroy->id_index);
		RELEASE_ARRAY(m_todestroy->index);
		App->renderer3D->DeleteBuffer(m_todestroy->id_vertex);
		RELEASE_ARRAY(m_todestroy->vertex);

		//Delete the allocated memory data for the mesh
		RELEASE(meshes[i]);
	}

	meshes.clear();

	return ret;
}

bool GeometryLoader::Load3DFile(const char* full_path)
{
	bool ret = true;
	//We call assimp to import the file
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		//We loaded the 3D file successfully!

		//We iterate the meshes array
		int nmeshes = scene->mNumMeshes;
		for (int i = 0; i < nmeshes; ++i)
		{
			Mesh_Data* new_mesh = new Mesh_Data;
			
			aiMesh* loaded_mesh = scene->mMeshes[i];

			// Copy VERTICES data (vertex)
			new_mesh->num_vertex = loaded_mesh->mNumVertices;
			new_mesh->vertex = new float[new_mesh->num_vertex * 3];
			memcpy(new_mesh->vertex, loaded_mesh->mVertices, sizeof(float) * new_mesh->num_vertex * 3);
			App->ConsoleLOG("New mesh with %d vertices loaded", new_mesh->num_vertex);

			// Copy INDICES/FACES data (index) (Faces as used in assimp)
			if (loaded_mesh->HasFaces())
			{
				new_mesh->num_index = loaded_mesh->mNumFaces * 3;
				new_mesh->index = new uint[new_mesh->num_index]; // assuming each face is a triangle  
				for (uint j = 0; j < loaded_mesh->mNumFaces; ++j)
				{   
					if(loaded_mesh->mFaces[j].mNumIndices != 3)    
						App->ConsoleLOG("WARNING, geometry face with != 3 indices! Export your 3D file with triangularized faces/polys!");   
					else    
						memcpy(&new_mesh->index[j*3], loaded_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
				}
				App->ConsoleLOG("Mesh has %d indices loaded & %d polys", new_mesh->num_index, new_mesh->num_index/3);
			}
			//Generate the buffers (Vertex and Index) for the VRAM & Drawing
			App->renderer3D->GenerateVertexBuffer(new_mesh->id_vertex, new_mesh->num_vertex, new_mesh->vertex);

			if (new_mesh->index != nullptr)
				App->renderer3D->GenerateIndexBuffer(new_mesh->id_index, new_mesh->num_index, new_mesh->index);

			//Finally add the new mesh to the vector
			meshes.push_back(new_mesh);
		}


		//Once finished we release the original file
		aiReleaseImport(scene);
	}
	else   
		App->ConsoleLOG("Error loading scene %s", full_path);

	return ret;
}

bool GeometryLoader::Init()
{
	// Stream log messages to Debug window 
	
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr); 
	aiAttachLogStream(&stream);

	return true;
}

bool GeometryLoader::Start()
{
	return true;
}

update_status GeometryLoader::Update(float dt)
{
	for (int i = 0; i < meshes.size(); ++i)
		App->renderer3D->DrawMesh(meshes[i]);

	return UPDATE_CONTINUE;
}
