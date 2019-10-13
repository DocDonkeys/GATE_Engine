#include "Primitive.h"
//#include "libs/par/par_shapes.h"
#include "libs/glew/include/GL/glew.h"
#include "libs/SDL/include/SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

Primitive::Primitive()
{
}

Primitive::~Primitive()
{
}

void Primitive::CreateSphere(int slices, int stacks)
{
	//mesh = par_shapes_create_parametric_sphere(slices, stacks);
}

bool Primitive::SendToVRAM()
{
	////If the mesh doesn't contain information we avoid crashing the applicattion and return false
	//if (mesh == nullptr)
	//	return false;

	//glGenBuffers(1, (GLuint*) &(vertex_id));
	//glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->npoints *3 , mesh->points, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glGenBuffers(1, (GLuint*) &(indices_id));
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * mesh->ntriangles * 3, mesh->triangles, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//info_in_VRAM = true;

	return true;
}

bool Primitive::Draw()
{
	////If the mesh values are not in the VRAM don't draw & avoid crashing
	//if (!info_in_VRAM)
	//	return false;

	//glEnableClientState(GL_VERTEX_ARRAY);

	//glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	//glDrawElements(GL_TRIANGLES, mesh->ntriangles * 3, GL_UNSIGNED_SHORT, nullptr);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glDisableClientState(GL_VERTEX_ARRAY);
	return true;
}