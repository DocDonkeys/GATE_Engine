#include "Mesh.h"
#include "Application.h"
#include "libs/Assimp/include/mesh.h"
#include "libs/Assimp/include/scene.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

Mesh::Mesh()
{
}

Mesh::~Mesh()
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

void Mesh::LoadVertices(const aiMesh* loaded_mesh)
{
	// Copy VERTICES data (vertex)
	num_vertex = loaded_mesh->mNumVertices;
	vertex = new float3[num_vertex];
	memcpy(vertex, loaded_mesh->mVertices, sizeof(float3) * num_vertex);
	LOG("New mesh with %d vertices loaded", num_vertex);
}

void Mesh::LoadIndices(const aiMesh* loaded_mesh)
{
	// Copy INDICES/FACES data (index) (Faces as used in assimp)
	if (loaded_mesh->HasFaces())
	{
		num_index = loaded_mesh->mNumFaces * 3;
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

void Mesh::LoadNormals(const aiMesh * loaded_mesh)
{
	// Copy NORMALS (Vertices)
	if (loaded_mesh->HasNormals())
	{
		normals_vector = new float3[num_vertex];
		memcpy(normals_vector, loaded_mesh->mNormals, sizeof(float3) * num_vertex);

		//Calculate the positions and vectors of the face Normals
		num_faces = loaded_mesh->mNumFaces;
		normals_faces = new float3[num_index];
		normals_faces_vector = new float3[num_index];
		for (int j = 0; j < num_index; j+=3)
		{
			// 3 points of the triangle/face
			float3 vert1 = vertex[index[j]];
			float3 vert2 = vertex[index[j + 1]];
			float3 vert3 = vertex[index[j + 2]];

			//Calculate starting point of the normal
			normals_faces[j] = (vert1 + vert2 + vert3) / 3;

			//Calculate Cross product of 2 edges of the triangle to obtain Normal vector
			float3 edge_a = vert2 - vert1;
			float3 edge_b = vert3 - vert1;

			float3 normal;
			normal = Cross(edge_a, edge_b);
			normal.Normalize();

			normals_faces_vector[j] = normal * 0.25f;
		}

	}
}

void Mesh::LoadTexCoords(const aiMesh * loaded_mesh)
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

void Mesh::LoadMeshSizeData()
{
	for (int i = 0; i < num_vertex; i++)
	{
		if (vertex[i].x > size_x.max) {
			size_x.max = vertex[i].x;
		}
		else if (vertex[i].x < size_x.min) {
			size_x.min = vertex[i].x;
		}

		if (vertex[i].y > size_y.max) {
			size_y.max = vertex[i].y;
		}
		else if (vertex[i].y < size_y.min) {
			size_y.min = vertex[i].y;
		}

		if (vertex[i].z > size_z.max) {
			size_z.max = vertex[i].z;
		}
		else if (vertex[i].z < size_z.min) {
			size_z.min = vertex[i].z;
		}
	}

	size_x.width = abs(size_x.max - size_x.min);
	size_y.width = abs(size_y.max - size_y.min);
	size_z.width = abs(size_z.max - size_z.min);
}