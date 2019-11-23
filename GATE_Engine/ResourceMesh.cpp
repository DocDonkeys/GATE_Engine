#include "ResourceMesh.h"
#include "ModuleRenderer3D.h"
#include "Application.h"
#include "MemLeaks.h"
#include "libs/Assimp/include/mesh.h"

ResourceMesh::ResourceMesh(uint32 uid) : Resource(uid,Resource::MESH)
{
}

ResourceMesh::~ResourceMesh()
{
	//Delete Buffer & data
	App->renderer3D->DeleteBuffer(id_vertex);
	RELEASE_ARRAY(vertex);

	App->renderer3D->DeleteBuffer(id_index);
	RELEASE_ARRAY(index);

	App->renderer3D->DeleteBuffer(id_normals);
	RELEASE_ARRAY(normals_faces);
	RELEASE_ARRAY(normals_faces_vector);
	RELEASE_ARRAY(normals_vector);


	App->renderer3D->DeleteBuffer(id_tex_coords);
	RELEASE_ARRAY(tex_coords);
}

bool ResourceMesh::LoadInMemory()
{
	return false;
}

void ResourceMesh::Save(json & config) const
{
}

void ResourceMesh::Load(const json & config)
{
}

void ResourceMesh::LoadVertices(const aiMesh* loaded_mesh)
{
	// Copy VERTICES data (vertex)
	num_vertex = loaded_mesh->mNumVertices;
	vertex = new float3[num_vertex];
	memcpy(vertex, loaded_mesh->mVertices, sizeof(float3) * num_vertex);
	LOG("New mesh with %d vertices loaded", num_vertex);
}

void ResourceMesh::LoadIndices(const aiMesh* loaded_mesh)
{
	// Copy INDICES/FACES data (index) (Faces as used in assimp)
	if (loaded_mesh->HasFaces())
	{
		num_polys = loaded_mesh->mNumFaces;
		num_index = num_polys * 3;
		index = new uint[num_index]; // assuming each face is a triangle

		for (uint j = 0; j < loaded_mesh->mNumFaces; ++j)
		{
			if (loaded_mesh->mFaces[j].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices! Export your 3D file with triangularized faces/polys!");
			}
			else
				memcpy(&index[j * 3], loaded_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
		}
		LOG("Mesh has %d indices loaded & %d polys", num_index, num_index / 3);
	}
}

void ResourceMesh::LoadNormals(const aiMesh * loaded_mesh)
{
	// Copy NORMALS (Vertices)
	if (loaded_mesh->HasNormals())
	{
		normals_vector = new float3[num_vertex];
		num_normals = num_vertex;
		memcpy(normals_vector, loaded_mesh->mNormals, sizeof(float3) * num_vertex);

		//Calculate the positions and vectors of the face Normals
		normals_faces = new float3[num_index];
		normals_faces_vector = new float3[num_index];
		//for (int j = 0; j < num_index; j += 3) //TODO: DIDAC SOLVE THIS CRASH
		//{
		//	// 3 points of the triangle/face
		//	float3 vert1 = vertex[index[j]];
		//	float3 vert2 = vertex[index[j + 1]];
		//	float3 vert3 = vertex[index[j + 2]];

		//	//Calculate starting point of the normal
		//	normals_faces[j] = (vert1 + vert2 + vert3) / 3;

		//	//Calculate Cross product of 2 edges of the triangle to obtain Normal vector
		//	float3 edge_a = vert2 - vert1;
		//	float3 edge_b = vert3 - vert1;

		//	float3 normal;
		//	normal = Cross(edge_a, edge_b);
		//	normal.Normalize();

		//	normals_faces_vector[j] = normal;
		//}

	}
}

void ResourceMesh::LoadTexCoords(const aiMesh * loaded_mesh)
{
	if (loaded_mesh->HasTextureCoords(0)) // Check only the fisrt texture tex_coords
	{
		num_tex_coords = num_vertex;
		tex_coords = new float[num_tex_coords * 2]; // Since each vertex will have a U and a V value we use double the size

		int k = 0;
		for (int j = 0; j < num_tex_coords; ++j)
		{
			k = j * 2;
			tex_coords[k] = loaded_mesh->mTextureCoords[0][j].x;
			tex_coords[k + 1] = loaded_mesh->mTextureCoords[0][j].y;
		}
	}
}

void ResourceMesh::ChangeNormalsLength(float new_length)
{
	if (new_length > 0.01f)
	{
		for (int i = 0; i < num_vertex; ++i)
		{
			normals_vector[i] /= normals_last_length;
			normals_vector[i] *= new_length;
		}
		for (int i = 0; i < num_index; ++i)
		{
			normals_faces_vector[i] /= normals_last_length;
			normals_faces_vector[i] *= new_length;
		}
		normals_last_length = new_length;
	}
}

void ResourceMesh::LoadMeshBounds()
{
	if (num_vertex > 0) {
		bounds.SetFrom(vertex, num_vertex);

		// We save to distance between max and min points now so later we just have to access it instead of calculating it
		size = { abs(bounds.MaxX() - bounds.MinX()), abs(bounds.MaxY() - bounds.MinY()), abs(bounds.MaxZ() - bounds.MinZ()) };
	}
}
