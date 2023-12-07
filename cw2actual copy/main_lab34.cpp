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

/* This is the top level for the program you need to create for lab three and four.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// these are core raytracing classes
#include "Core/framebuffer.h"
#include "Core/scene.h"
#include "Core/material_colour.h"

// classes that contain our objects to be rendered, all derived from Object
#include "Objects/polymesh_object.h"
#include "Objects/sphere_object.h"
#include "Objects/plane_object.h"
#include "Objects/quadratic_object.h"

// classes that contain our lights, all derived from Light
#include "Lights/directional_light.h"

// classes that contain the materials applied to an object, all derived from Material
#include "Materials/phong_material.h"
#include "Materials/falsecolour_material.h"
#include "Materials/global_material.h"
#include "Materials/compound_material.h"

//classes that contain cameras, all derived from Camera
#include "Cameras/simple_camera.h"
#include "Cameras/full_camera.h"

using namespace std;

// you will find it useful during development/debugging to create multiple functions that fill out the scene.
void build_scene(Scene& scene)
{
	// The following transform allows 4D homogeneous coordinates to be transformed.It moves the supplied teapot model to somewhere visible.
	Transform * transform = new Transform(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, -60.0f,
			0.0f, 1.0f, 0.0f, 20.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	
	//  Read in the teapot model.
	PolyMesh* pm = new PolyMesh((char *)"teapot-low.obj", true);
	pm->apply_transform(*transform);
	
	//Add quadratic object
	Quadratic* quadobj = new Quadratic(1.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	Phong* quadMaterial = new Phong(new MaterialColour(0.1f, 0.1f, 0.1f, 1.0f), 
	                                new MaterialColour(0.5f, 0.0f, 0.0f, 1.0f), 
									new MaterialColour(0.4f, 0.4f, 0.4f, 1.0f), 40.0f);   
	quadobj->set_material(quadMaterial);
	scene.add_object(quadobj);


	//Instantiate sphere 1
	Sphere* sphere = new Sphere(Vertex(1.5f, 1.0f, 4.0f), 1.0f);
	//Instantiate sphere 2
	Sphere* sphere2 = new Sphere(Vertex(-1.5f, 1.0f, 4.0f), 1.0f);
	//Instantiate directional lighting element
	DirectionalLight* dl = new DirectionalLight(Vector(1.01f, -1.0f, 1.0f), Colour(1.0f, 1.0f, 1.0f, 0.0f));
	//Add lighting to the scene
	scene.add_light(dl);

	Phong* bp1 = new Phong(new MaterialColour(0.0f, 0.1f, 0.0f,1.0f), new MaterialColour(0.0f, 0.4f, 0.0f,1.0f), new MaterialColour(0.4f, 0.4f, 0.4f,1.0f), 40.f);
	Phong* bp2 = new Phong(new MaterialColour(0.1f, 0.1f, 0.1f,1.0f), new MaterialColour(0.5f, 0.0f, 0.0f,1.0f), new MaterialColour(0.4f, 0.4f, 0.4f,1.0f), 40.f);
	GlobalMaterial *gm1 = new GlobalMaterial(&scene, Colour(0.1f, 0.1f, 0.1f),Colour(0.1f, 0.1f, 0.1f) , 1.53f);

	CompoundMaterial*cm = new CompoundMaterial(2);
	cm->include_material(bp1);
	cm->include_material(gm1);
	//Set the material of the polymesh object to compound material
	pm->set_material(cm);

	//scene.add_object(pm);

	sphere->set_material(bp2);
	sphere2->set_material(cm);

	scene.add_object(sphere);
	scene.add_object(sphere2);
}


// This is the entry point function to the program.
int main(int argc, char *argv[])
{
	int width = 512;
	int height = 512;
	// Create a framebuffer
	FrameBuffer* fb = new FrameBuffer(width, height);
	
	// Create a scene
	Scene scene;
	
	// Setup the scene
	build_scene(scene);
	
	// Declare a camera
	Camera *camera = new SimpleCamera(0.5f);
	Vertex a=Vertex(-30.0f, 0.0f, 0.0f);
	Vector b=Vector(1.0f, 0.0f, 1.0f);
	Vector c=Vector(0.0f, 1.0f, 0.0f);
	//Camera* camera = new FullCamera(0.5f, a, b, c);
	
	// Camera generates rays for each pixel in the framebuffer and records colour + depth.
	camera->render(scene,*fb);
	
	// Output the framebuffer colour and depth as two images
	fb->writeRGBFile((char *)"test.ppm");
	fb->writeDepthFile((char *)"depth.ppm");
	
	cerr << "\nDone.\n" << flush;
	
	// a debug check to make sure we don't leak hits.
	cerr << "Hit Pool " << Hit::pool_size << " Allocated " << Hit::allocated << "\n" << flush;
	return 0;
}
