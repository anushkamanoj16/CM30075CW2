/****************************************************************************
*
* krt - Ken's Raytracer - Coursework Edition. (C) Copyright 1993-2022.
*
* I've put a lot of time and effort into this code. For the last decade
* it's been used to introduce hundreds of students at multiple universities
* to raytracing. It forms the basis of your coursework but you are free
* to continue using/developing forever more. However, I ask that you don't
* share the code or your derivitive versions publicly. In order to continue
* to be used for coursework and in particular assessment it's important that
* versions containing solutions are not searchable on the web or easy to
* download.
*
* If you want to show off your programming ability, instead of releasing
* the code, consider generating an incredible image and explaining how you
* produced it.
*/

 // The global material generates a reflection/refraction layer
#pragma once

#include "../Core/material.h"
#include "../Core/environment.h"

class GlobalMaterial: public Material {
public:
	Colour reflect_weight;
	Colour refract_weight;
	float ior;
	Environment* environment;

	GlobalMaterial(Environment* p_env, Colour p_reflect_weight, Colour p_refract_weight, float ior);

	Colour compute_once(Ray& viewer, Hit& hit, int recurse);

	Colour compute_per_light(Vector& viewer, Hit& hit, Vector& ldir);

};

