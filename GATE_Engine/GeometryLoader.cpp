#include "GeometryLoader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "libs/Assimp/include/cimport.h"
#include "libs/Assimp/include/scene.h"
#include "libs/Assimp/include/postprocess.h"
#include "libs/Assimp/include/cfileio.h"

#include "libs/MathGeoLib/include/MathGeoLib.h"
#include "libs/MathGeoLib/include/MathBuildConfig.h"


#pragma comment (lib, "libs/Assimp/libx86/assimp.lib")

#include "libs/par/par_shapes.h"
#include "Mesh.h"

//#pragma comment (lib, "libs/assimp-5.0.0/libx86/assimp.lib")		//CHANGE/FIX: Remove? @Didac do we need to keep this here just in case?
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
		Mesh* m_todestroy = meshes[i];
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
	//We extract the Absolute path from the full path (everything until the actual file)
	std::string str = full_path;
	std::string absolute_path;

	std::size_t found = str.find_last_of("/\\"); //Find last\\  (right before the filename) //
	absolute_path = str.substr(0, found + 1);

	//We call assimp to import the file
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		//We loaded the 3D file successfully!
		//We iterate the meshes array
		int nmeshes = scene->mNumMeshes;
		for (int i = 0; i < nmeshes; ++i)
		{
			Mesh* new_mesh = new Mesh();

			aiMesh* loaded_mesh = scene->mMeshes[i];
			//LOAD!
			new_mesh->LoadVertices(loaded_mesh); //Vertices
			new_mesh->LoadIndices(loaded_mesh); //Indices
			new_mesh->LoadNormals(loaded_mesh); //Normals
			new_mesh->LoadTexCoords(loaded_mesh); // UV's
			new_mesh->LoadMaterials(scene, loaded_mesh, absolute_path); //Materials

			//Generate the buffers (Vertex and Index) for the VRAM & Drawing
			App->renderer3D->GenerateVertexBuffer(new_mesh->id_vertex, new_mesh->num_vertex, new_mesh->vertex);

			if (new_mesh->index != nullptr)
				App->renderer3D->GenerateIndexBuffer(new_mesh->id_index, new_mesh->num_index, new_mesh->index);

			//Generate the buffer for the tex_coordinates
			App->renderer3D->GenerateVertexBuffer(new_mesh->id_tex_coords, new_mesh->num_tex_coords * 2, new_mesh->tex_coords);

			//Finally add the new mesh to the vector
			meshes.push_back(new_mesh);
		}
		//Once finished we release the original file
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene meshes %s", full_path);

	return ret;
}

void GeometryLoader::LoadPrimitiveShape(const par_shapes_mesh_s * p_mesh)
{
	Mesh* new_mesh = new Mesh();

	//Get sizes
	new_mesh->num_vertex = p_mesh->npoints;
	new_mesh->num_index = p_mesh->ntriangles * 3;
	new_mesh->num_tex_coords = p_mesh->npoints;

	//Alloc memory
	new_mesh->vertex = new float3[new_mesh->num_vertex];
	new_mesh->index = new uint[new_mesh->num_index];
	new_mesh->tex_coords = new float[new_mesh->num_tex_coords * 2];

	//Copy the par_shape_mesh vertex array and index array into Mesh_Data
	for (int i = 0; i < new_mesh->num_vertex; i++)
	{
		int j = i * 3;
		new_mesh->vertex[i].x = p_mesh->points[j];
		new_mesh->vertex[i].y = p_mesh->points[j + 1];
		new_mesh->vertex[i].z = p_mesh->points[j + 2];
	}

	for (int i = 0; i < new_mesh->num_index; i++)
	{
		new_mesh->index[i] = (uint)p_mesh->triangles[i];
	}
	LOG("Created Primitive with %d vertices & %d indices.", new_mesh->num_vertex, new_mesh->num_index);

	//Copy the par_shapes texture coordinates
	for (int i = 0; i < new_mesh->num_tex_coords * 2; ++i)
		new_mesh->tex_coords[i] = p_mesh->tcoords[i];
	
	//Generate Buffers
	App->renderer3D->GenerateVertexBuffer(new_mesh->id_vertex, new_mesh->num_vertex, new_mesh->vertex);
	App->renderer3D->GenerateIndexBuffer(new_mesh->id_index, new_mesh->num_index, new_mesh->index);
	App->renderer3D->GenerateVertexBuffer(new_mesh->id_tex_coords, new_mesh->num_tex_coords * 2,new_mesh->tex_coords);
	//Push into the meshes vector
	meshes.push_back(new_mesh);
}

void GeometryLoader::CreatePrimitive(PRIMITIVE p, int slices, int stacks, float radius)
{
	par_shapes_mesh* primitive_mesh = nullptr;

	switch (p)
	{
	case PRIMITIVE::PLANE:
		primitive_mesh = par_shapes_create_plane(slices,stacks);
		break;
	case PRIMITIVE::CUBE:
		primitive_mesh = par_shapes_create_primitive_cube();
		break;
	case PRIMITIVE::SPHERE:
		primitive_mesh = par_shapes_create_parametric_sphere(slices, stacks);
		break;
	case PRIMITIVE::HEMISPHERE:
		primitive_mesh = par_shapes_create_hemisphere(slices, stacks);
		break;
	case PRIMITIVE::CYLINDER:
		primitive_mesh = par_shapes_create_cylinder(slices, stacks);
		break;
	case PRIMITIVE::CONE:
		primitive_mesh = par_shapes_create_cone(slices, stacks);
		break;
	case PRIMITIVE::TORUS:
		primitive_mesh = par_shapes_create_torus(slices, stacks,radius);
		break;
	default:
		break;
	}

	//Push into the meshes vector
	if (primitive_mesh != nullptr)
		LoadPrimitiveShape(primitive_mesh);
	else
		LOG("Failed to create primitive! Invalid primitive enum value received");
}

bool GeometryLoader::Init()
{
	// Stream log messages to Debug window

	log_stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&log_stream);

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
