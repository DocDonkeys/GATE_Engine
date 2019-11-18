#ifndef LIGHT_H
#define LIGHT_H

#include "Color.h"

struct Light	// CHANGE/FIX: Review what are we going to do with these and inside which filter should it be
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	float pos_x, pos_y, pos_z;	//IMPROVE: Including float3 here destroys the "include" framework, so we use this instead
	
	int ref;
	bool on;
};

#endif //LIGHT_H