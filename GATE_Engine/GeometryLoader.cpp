#include "GeometryLoader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "libs/Assimp/include/cimport.h"
#include "libs/Assimp/include/scene.h"
#include "libs/Assimp/include/postprocess.h"
#include "libs/Assimp/include/cfileio.h"

#pragma comment (lib, "libs/Assimp/libx86/assimp.lib")

#include "libs/par/par_shapes.h"

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
			new_mesh->vertex = new float3[new_mesh->num_vertex];
			memcpy(new_mesh->vertex, loaded_mesh->mVertices, sizeof(float3) * new_mesh->num_vertex);
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

			// Copy NORMALS (Vertices)
			if (loaded_mesh->HasNormals())
			{
				new_mesh->normals_vector = new float3[new_mesh->num_vertex];
				for (int j = 0; j < new_mesh->num_vertex; ++j)
				{
					new_mesh->normals_vector[j].x = loaded_mesh->mNormals[j].x;
					new_mesh->normals_vector[j].y = loaded_mesh->mNormals[j].y;
					new_mesh->normals_vector[j].z = loaded_mesh->mNormals[j].z;
				}

				//The normals are just the vectors of each point, so we must add them to the position of each vertex
				new_mesh->normals_vertex = new float3[new_mesh->num_vertex * 2];
				for (int j = 0; j < new_mesh->num_vertex; ++j)
				{
					int k = j * 2; //Since the array we fill will be double the size we multiply by 2 make things easier when placing data onto the array

					new_mesh->normals_vertex[k] = new_mesh->vertex[j]; // Original position of the line

					//Position of the line + vector (this way we can )
					new_mesh->normals_vertex[k + 1].x = new_mesh->vertex[j].x + new_mesh->normals_vector[j].x;
					new_mesh->normals_vertex[k + 1].y = new_mesh->vertex[j].y + new_mesh->normals_vector[j].y;
					new_mesh->normals_vertex[k + 1].z = new_mesh->vertex[j].z + new_mesh->normals_vector[j].z;
				}
			}

			//ASK: We should try and make a system to actually load all possible tex coords for each texture
			/*for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++j)
			{
				if (loaded_mesh->HasTextureCoords(j))
				{
				}
			}*/

			if (loaded_mesh->HasTextureCoords(0)) // Check only the fisrt texture tex_coords
			{
				new_mesh->num_tex_coords = new_mesh->num_vertex;
				new_mesh->tex_coords = new float3[new_mesh->num_tex_coords];

				for (int j = 0; j < new_mesh->num_tex_coords; ++j)
				{
					new_mesh->tex_coords[j].x = loaded_mesh->mTextureCoords[0][j].x;
					new_mesh->tex_coords[j].y = loaded_mesh->mTextureCoords[0][j].y;
					new_mesh->tex_coords[j].z = loaded_mesh->mTextureCoords[0][j].z;
				}
			}


			//Generate the buffers (Vertex and Index) for the VRAM & Drawing
			App->renderer3D->GenerateVertexBuffer(new_mesh->id_vertex, new_mesh->num_vertex, new_mesh->vertex);

			if (new_mesh->index != nullptr)
				App->renderer3D->GenerateIndexBuffer(new_mesh->id_index, new_mesh->num_index, new_mesh->index);

			//Generate buffer for Normals
			glGenBuffers(1, (GLuint*) &(new_mesh->id_normals));
			glBindBuffer(GL_ARRAY_BUFFER, new_mesh->id_normals);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * new_mesh->num_vertex, new_mesh->normals_vertex, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Generate the buffer for the tex_coordinates
			App->renderer3D->GenerateVertexBuffer(new_mesh->id_tex_coords, new_mesh->num_tex_coords, new_mesh->tex_coords);

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

void GeometryLoader::LoadPrimitiveShape(par_shapes_mesh_s * p_mesh)
{
	Mesh_Data* new_mesh = new Mesh_Data();

	//Get sizes
	new_mesh->num_vertex = p_mesh->npoints;
	new_mesh->num_index = p_mesh->ntriangles * 3;

	//Alloc memory
	new_mesh->vertex = new float3[new_mesh->num_vertex];
	new_mesh->index = new uint[new_mesh->num_index];

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
	App->ConsoleLOG("Created Primitive with %d vertices & %d indices.", new_mesh->num_vertex, new_mesh->num_index);

	//Generate Buffers
	App->renderer3D->GenerateVertexBuffer(new_mesh->id_vertex, new_mesh->num_vertex, new_mesh->vertex);
	App->renderer3D->GenerateIndexBuffer(new_mesh->id_index, new_mesh->num_index, new_mesh->index);

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
		primitive_mesh = par_shapes_create_cube();
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
		App->ConsoleLOG("Failed to create primitive! Invalid primitive enum value received");
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
