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

#include "global_material.h"
// #include "../Core/vector.h"
#include <math.h>
#include <cmath>
#include "../Core/scene.h"



GlobalMaterial::GlobalMaterial(Environment* p_env, Colour p_reflect_weight, Colour p_refract_weight, float p_ior)
{
    this->environment = p_env;
    this->reflect_weight = p_reflect_weight;
    this->refract_weight = p_refract_weight;
    this->ior = p_ior;
}
// reflection and recursion computation goes here
Colour GlobalMaterial::compute_once(Ray& viewer, Hit& hit, int recurse) {
	Colour resultray;
    Colour reflect_result;
    Colour refract_result;

    if (recurse == 0)
    {
        return (resultray);
    }

    // compute reflection ray
    Vector r;
    hit.normal.reflection(viewer.direction, r);
    r.normalise();

    Ray reflection_ray;

    reflection_ray.position = hit.position + (0.001f * r);
    reflection_ray.direction = r;

    float reflection_depth;
    this->environment->raytrace(reflection_ray, recurse - 1, reflect_result, reflection_depth);


    // compute refraction ray
    float reflectance = 1.0, transmittance = 0.0;

    float cos_i, cos_t, values;
    cos_i = hit.normal.dot(-viewer.direction);
    values = (1.0f - 1.0f / (this->ior * this->ior) * (1.0f - cos_i * cos_i));


    if(values >= 0.0f) // total internal reflection if values < 0.0f
    {
        cos_t = std::sqrt(values);

        if (!hit.entering)
        {
            this->ior = 1.0 / this->ior;
        }

        Ray refraction_ray;
        r = (1.0f / this->ior) * viewer.direction - (cos_t - 1.0 / this->ior * cos_i) * hit.normal;
        r.normalise();

        refraction_ray.position = hit.position + (0.001f * r);
        refraction_ray.direction = r;

        float depth_refraction;
        this->environment->raytrace(refraction_ray, recurse - 1, refract_result, depth_refraction);

        float parallel_reflectance, perpendicular_reflectance;
        parallel_reflectance = (this->ior * cos_i - cos_t) / (this->ior * cos_i + cos_t);
        perpendicular_reflectance = (cos_i - this->ior * cos_t) / (cos_i + this->ior * cos_t);

        reflectance = 0.5f * (parallel_reflectance * parallel_reflectance +  perpendicular_reflectance * perpendicular_reflectance);
        transmittance = 1.0f - reflectance;
    }

    resultray = reflect_result * reflectance + refract_result * transmittance;

    return resultray;
}
 
 

Colour GlobalMaterial::compute_per_light(Vector& viewer, Hit& hit, Vector& ldir)
{
	Colour result;


	return result;
}

