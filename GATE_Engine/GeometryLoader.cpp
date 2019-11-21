#include "GeometryLoader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneIntro.h"
#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "TextureLoader.h"
#include "Importer.h"

#include "libs/Assimp/include/cimport.h"
#include "libs/Assimp/include/scene.h"
#include "libs/Assimp/include/postprocess.h"
#include "libs/Assimp/include/cfileio.h"

#include "libs/MathGeoLib/include/MathGeoLib.h"
#include "libs/MathGeoLib/include/MathBuildConfig.h"

#pragma comment (lib, "libs/Assimp/libx86/assimp.lib")

#include "libs/par/par_shapes.h"
#include "Mesh.h"
#include "Texture.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

// Memory Leak Detection
#include "MemLeaks.h"

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

	return ret;
}

void AssimpLOGCallback(const char * msg, char * userData)
{
	//We LOG to Console
	LOG(msg,userData);
}

bool GeometryLoader::Load3DFile(const char* full_path)
{
	bool ret = true;

	float biggestMeshSize = -1.0f;

	//Default GameObject names based on trimmed filename
	std::string filename = App->SubtractString(std::string(full_path), "\\", true, false);
	std::string objName = App->SubtractString(std::string(filename), ".", false, true) + "_";
	uint counter = 0;

	//We extract the Absolute path from the full path (everything until the actual file)
	std::string str = full_path;
	std::string absolute_path;

	std::size_t found = str.find_last_of("/\\"); //Find last\\  (right before the filename) //
	absolute_path = str.substr(0, found + 1);

	App->file_system->DuplicateFile(full_path,ASSETS_FOLDER);

	//We call assimp to import the file
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		//We loaded the 3D file successfully!
		//We load all nodes inside the root node, respecting parenting in gameobjects
		aiNode* root = scene->mRootNode;
		GameObject* go = LoadAssimpNode(scene,root,absolute_path.c_str(),filename.c_str(),full_path,objName.c_str(),counter);
		GOFunctions::ReParentGameObject(go, App->scene_intro->root);
		
		//Once finished we release the original file
		aiReleaseImport(scene);

		//Test gameobject saving as .model
	}
	else
		LOG("Error loading scene meshes %s", full_path);

	return ret;
}

GameObject* GeometryLoader::LoadAssimpNode(const aiScene* scene, const aiNode* node,
	const char* absolute_path, const char* filename, const char* full_path, 
	const char* objName, uint counter)
{
	float biggestMeshSize = -1.0f;

	float4x4 meshTrs;
	//Before creating Game Object, obtain local transformation respect parent
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			meshTrs[i][j] = node->mTransformation[i][j];
		}
	}

	//We create the gameobject for the node
	GameObject* ret_go = App->scene_intro->CreateEmptyGameObject(node->mName.C_Str());

	// Transform
	ComponentTransform* trs_component = (ComponentTransform*)ret_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs_component->SetLocalMat(meshTrs);

	if (node != nullptr && node->mNumMeshes > 0)
	{
		int nmeshes = node->mNumMeshes;
		for (int i = 0; i < nmeshes; ++i)
		{
			Mesh* new_mesh = new Mesh();
			aiMesh* loaded_mesh = scene->mMeshes[node->mMeshes[i]];
			
			//LOAD!
			new_mesh->LoadVertices(loaded_mesh);	//Vertices
			new_mesh->LoadIndices(loaded_mesh);		//Indices
			new_mesh->LoadNormals(loaded_mesh);		//Normals
			new_mesh->LoadTexCoords(loaded_mesh);	// UV's			

			//Generate the buffers (Vertex and Index) for the VRAM & Drawing
			App->renderer3D->GenerateVertexBuffer(new_mesh->id_vertex, new_mesh->num_vertex, new_mesh->vertex);

			if (new_mesh->index != nullptr)
				App->renderer3D->GenerateIndexBuffer(new_mesh->id_index, new_mesh->num_index, new_mesh->index);

			//Generate the buffer for the tex_coordinates
			App->renderer3D->GenerateVertexBuffer(new_mesh->id_tex_coords, new_mesh->num_tex_coords * 2, new_mesh->tex_coords);

			
			//Finally add the new mesh to the vector
			//meshes.push_back(new_mesh);

			//We create a game object for the current mesh
			/*GameObject* go = App->scene_intro->CreateEmptyGameObject(std::string(objName + std::to_string(counter++)).c_str());
			go->ReParent(ret_go);*/
			
			// Mesh
			ComponentMesh* mesh_component = (ComponentMesh*)ret_go->CreateComponent(COMPONENT_TYPE::MESH);
			mesh_component->mesh = new_mesh;
			mesh_component->mesh->path = full_path;
			mesh_component->mesh->filename = filename;
			mesh_component->mesh->LoadMeshBounds();
			ret_go->size = mesh_component->mesh->GetSize();

			float currMeshSize = Length(new_mesh->GetSize());
			if (biggestMeshSize < currMeshSize) {
				App->camera->CenterToObject(ret_go);
				biggestMeshSize = currMeshSize;
			}

			// Material
			ComponentMaterial* material_component = (ComponentMaterial*)ret_go->CreateComponent(COMPONENT_TYPE::MATERIAL);
			Texture* tex = LoadMaterial(scene, loaded_mesh, absolute_path);
			if (tex == nullptr || tex->id == 0) {
				LOG("[Warning]: The FBX has no embeded texture, could was not found, or could not be loaded!");
			}
			else {
				material_component->AssignTexture(tex);
			}
		}
	}

	if (node->mNumChildren > 0)
		for (int i = 0; i < node->mNumChildren; ++i)
		{
			GameObject* child = LoadAssimpNode(scene, node->mChildren[i], absolute_path, filename, full_path, objName, counter);
			GOFunctions::ReParentGameObject(child, ret_go);
		}

	return ret_go;
}

void GeometryLoader::LoadPrimitiveShape(const par_shapes_mesh_s * p_mesh, const char* name)
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

	LoadPrimitiveNormals(new_mesh,p_mesh);


	//Copy the par_shapes texture coordinates
	for (int i = 0; i < new_mesh->num_tex_coords * 2; ++i)
		new_mesh->tex_coords[i] = p_mesh->tcoords[i];

	//Generate Buffers
	App->renderer3D->GenerateVertexBuffer(new_mesh->id_vertex, new_mesh->num_vertex, new_mesh->vertex);
	App->renderer3D->GenerateIndexBuffer(new_mesh->id_index, new_mesh->num_index, new_mesh->index);
	App->renderer3D->GenerateVertexBuffer(new_mesh->id_tex_coords, new_mesh->num_tex_coords * 2,new_mesh->tex_coords);

	//We create a game object for the current mesh
	GameObject* go = App->scene_intro->CreateEmptyGameObject(name);

	ComponentMesh* mesh_component = (ComponentMesh*)go->CreateComponent(COMPONENT_TYPE::MESH);
	mesh_component->mesh = new_mesh;
	mesh_component->mesh->LoadMeshBounds();
	go->size = mesh_component->mesh->GetSize();
	go->UpdateBoundingBox();

	go->CreateComponent(COMPONENT_TYPE::MATERIAL); //Create default material (will have checkers if Teacher wants to us it to check uv's)
}

void GeometryLoader::LoadPrimitiveNormals(Mesh * new_mesh, const par_shapes_mesh_s * p_mesh)
{
	//Load the Normals of the par_shape
	if (p_mesh->normals != NULL)
	{
		new_mesh->normals_vector = new float3[new_mesh->num_vertex];
		memcpy(new_mesh->normals_vector, p_mesh->normals, sizeof(float3) * new_mesh->num_vertex);

		//Calculate the positions and vectors of the face Normals
		new_mesh->normals_faces = new float3[new_mesh->num_index];
		new_mesh->normals_faces_vector = new float3[new_mesh->num_index];
		for (int j = 0; j < new_mesh->num_index; j += 3)
		{
			// 3 points of the triangle/face
			float3 vert1 = new_mesh->vertex[new_mesh->index[j]];
			float3 vert2 = new_mesh->vertex[new_mesh->index[j + 1]];
			float3 vert3 = new_mesh->vertex[new_mesh->index[j + 2]];

			//Calculate starting point of the normal
			new_mesh->normals_faces[j] = (vert1 + vert2 + vert3) / 3;

			//Calculate Cross product of 2 edges of the triangle to obtain Normal vector
			float3 edge_a = vert2 - vert1;
			float3 edge_b = vert3 - vert1;

			float3 normal;
			normal = Cross(edge_a, edge_b);
			normal.Normalize();

			new_mesh->normals_faces_vector[j] = normal * 0.25f;
		}
	}
}

Texture* GeometryLoader::LoadMaterial(const aiScene * scene, const aiMesh * loaded_mesh, const std::string & absolute_path)
{
	Texture* ret = nullptr;
	if (scene->HasMaterials())
	{
		aiString tex_path;
		aiMaterial* material = scene->mMaterials[loaded_mesh->mMaterialIndex]; // For now we are just required to use 1 diffse texture

		material->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path);

		if (tex_path.length > 0)
		{
			std::string relative_path = tex_path.C_Str();

			std::size_t found = relative_path.find_last_of("/\\");
			if (found > 0)
			{
				relative_path = relative_path.substr(found + 1, relative_path.size());
			}
			std::string texture_path = absolute_path.data() + relative_path;
			ret = App->texture_loader->LoadTextureFile(texture_path.data());

			if (ret == nullptr || ret->id == 0) {
				LOG("[Error]: Texture loading failed in path %s.", absolute_path);
			}
		}
		else
		{
			LOG("[Error]: No diffuse texture or path loading failed.");
		}
	}
	else
		LOG("[Info]: File has no linked materials to load.");

	return ret;
}

void GeometryLoader::CreatePrimitive(PRIMITIVE p, int slices, int stacks, float radius)
{
	par_shapes_mesh* primitive_mesh = nullptr;
	const char* name;
	switch (p)
	{
	case PRIMITIVE::PLANE:
		primitive_mesh = par_shapes_create_plane(slices,stacks);
		name = "Plane";
		break;
	case PRIMITIVE::CUBE:
		primitive_mesh = par_shapes_create_primitive_cube();
		name = "Cube";
		break;
	case PRIMITIVE::SPHERE:
		primitive_mesh = par_shapes_create_parametric_sphere(slices, stacks);
		name = "Sphere";
		break;
	case PRIMITIVE::HEMISPHERE:
		primitive_mesh = par_shapes_create_hemisphere(slices, stacks);
		name = "Hemisphere";
		break;
	case PRIMITIVE::CYLINDER:
		primitive_mesh = par_shapes_create_cylinder(slices, stacks);
		name = "Cylinder";
		break;
	case PRIMITIVE::CONE:
		primitive_mesh = par_shapes_create_cone(slices, stacks);
		name = "Cone";
		break;
	case PRIMITIVE::TORUS:
		primitive_mesh = par_shapes_create_torus(slices, stacks,radius);
		name = "Torus";
		break;
	default:
		break;
	}

	//Push into the meshes vector
	if (primitive_mesh != nullptr)
		LoadPrimitiveShape(primitive_mesh, name);
	else
		LOG("Failed to create primitive! Invalid primitive enum value received");
}

bool GeometryLoader::Init()
{
	// Stream log messages to Debug window
	//struct aiLogStream log_stream; //ASSIMP DEBUG IS CRASHING CODE for Joints message!
	//log_stream.callback = AssimpLOGCallback;
	//aiAttachLogStream(&log_stream);

	return true;
}

bool GeometryLoader::Start()
{
	return true;
}

update_status GeometryLoader::Update(float dt)
{

	return UPDATE_CONTINUE;
}
