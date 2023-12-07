/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Phong is a child class of Material and implement the simple Phong
// surface illumination model.

#pragma once

#include "../Core/material.h"
#include "../Core/material_colour.h"

class Phong : public Material {
public:
//BEGIN_STAGE_ONE
	MaterialColour *ambient;
	MaterialColour *diffuse;
	MaterialColour *specular;
	float  power;

	Phong(MaterialColour *p_ambient, MaterialColour *p_diffuse, MaterialColour *p_specular, float p_power);
//END_STAGE_ONE
	Phong()
	{

	}

	Colour compute_once(Ray& viewer, Hit& hit, int recurse);
	Colour compute_per_light(Vector& viewer, Hit& hit, Vector& ldir);

};
