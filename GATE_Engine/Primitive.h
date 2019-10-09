#ifndef __MODULERENDERER3D_H__
#define __MODULERENDERER3D_H__

#include "Globals.h"


struct par_shapes_mesh_s;

class Primitive
{
public:
	Primitive();
	~Primitive();

public: //Methods

	void CreateSphere(int slices, int stacks);

public: //Vars
	par_shapes_mesh_s* mesh = nullptr;

	uint vertex_id = 0;
	uint indices_id = 0;

	bool info_in_VRAM = false;

	bool SendToVRAM();

	bool Draw();

};

#endif // !__MODULERENDERER3D_H__

