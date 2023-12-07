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

// MaterialColour is an extra base class for colours used in material that we may want to change based on coordinates.

#pragma once

#include "colour.h"
#include "ray.h"
#include "hit.h"

// The base class implements a simple constant colour.
class MaterialColour {

	float r,g,b,a;
public:

	MaterialColour(float red, float green, float blue, float alpha)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}

	// compute_once is called once per intersection
	Colour lookup_colour(Hit &hit)
	{
		Colour result;
		result.r = r;
		result.g = g;
		result.b = b;
		result.a = a;
		return result;
	}

	MaterialColour operator= (MaterialColour other)
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		return *this;
	}
};
