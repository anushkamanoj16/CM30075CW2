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

/* CSG is an object that is built by Constructive Solid Geometry from two sub-objects.*/

using namespace std;

#include "csg_object.h"

CSG::CSG(CSG::Mode p_mode, Object* p_left, Object* p_right)
{
//BEGIN_STAGE_ONE
	mode = p_mode;
	left = p_left;
	right = p_right;
//END_STAGE_ONE
	next = (Object*)0;
}

Hit* CSG::intersection(Ray ray)
{
	Hit* result = 0;

	const CSG::Action actions[3][8] = { {CSG::CSG_ADROP, CSG_BDROP,CSG_AEXIT,CSG_BDROP,CSG_ADROP,CSG_BEXIT,CSG_AENTER,CSG_BENTER},
		{CSG::CSG_AEXIT, CSG_BEXIT,CSG_ADROP,CSG_BENTER,CSG_AENTER,CSG_BDROP,CSG_ADROP,CSG_BDROP},
	{CSG::CSG_ADROP, CSG_BENTER,CSG_AEXIT,CSG_BEXIT,CSG_ADROP,CSG_BDROP,CSG_AENTER,CSG_BDROP} };

	Hit* left_hits = left->intersection(ray);
	Hit* right_hits = right->intersection(ray);

	Hit* current = 0;

	Hit* temp;
	while ((left_hits != 0) && (right_hits != 0))
	{
		int state = 0;
		if (left_hits->entering)
		{
			state += 4;
		}
		if (right_hits->entering)
		{
			state += 2;
		}
		if (left_hits->t > right_hits->t)
		{
			state += 1;
		}
		
		switch (actions[mode][state])
		{
		case CSG::CSG_AENTER:
			if (result == 0)
			{
				result = left_hits;
			}
			else
			{
				current->next = left_hits;
			}
			current = left_hits;
			left_hits = left_hits->next;
			current->next = 0;
			current->entering = true;
			break;
		case CSG::CSG_AEXIT:
			if (result == 0)
			{
				result = left_hits;
			}
			else
			{
				current->next = left_hits;
			}
			current = left_hits;
			left_hits = left_hits->next;
			current->next = 0;
			current->entering = false;
			break;
		case CSG::CSG_ADROP:
			temp = left_hits;
			left_hits = left_hits->next;
			delete temp;
			break;
		case CSG::CSG_BENTER:
			if (result == 0)
			{
				result = right_hits;
			}
			else
			{
				current->next = right_hits;
			}
			current = right_hits;
			right_hits = right_hits->next;
			current->next = 0;
			current->entering = true;
			break;
		case CSG::CSG_BEXIT:
			if (result == 0)
			{
				result = right_hits;
			}
			else
			{
				current->next = right_hits;
			}
			current = right_hits;
			right_hits = right_hits->next;
			current->next = 0;
			current->entering = false;
			break;
		case CSG::CSG_BDROP:
			temp = right_hits;
			right_hits = right_hits->next;
			delete temp;
			break;
		};
	}

	switch (mode)
	{
		case CSG::CSG_DIFF:
			if (left_hits != 0)
			{
				if(result == 0)
				{
					result = current = left_hits;
				}
				else
				{
					current->next = left_hits;
				}
			}
			else
			{
				while (right_hits != 0)
				{
					temp = right_hits;
					right_hits = right_hits->next;
					delete temp;
				}
			}
			break;
		case CSG::CSG_UNION:
			if (left_hits == 0)
			{
				if (result == 0)
				{
					result = current = right_hits;
				}
				else
				{
					current->next = right_hits;
				}
			}
			else
			{
				if (result == 0)
				{
					result = current = left_hits;
				}
				else
				{
					current->next = left_hits;
				}
			}
			break;
		case CSG::CSG_INTER:
			if (left_hits != 0)
			{
				while (left_hits != 0)
				{
					temp = left_hits;
					left_hits = left_hits->next;
					delete temp;
				}
			}
			else
			{
				while (right_hits != 0)
				{
					temp = right_hits;
					right_hits = right_hits->next;
					delete temp;
				}
			}
			break;
	};

	return result;
}

void CSG::apply_transform(Transform& transform)
{
//BEGIN_STAGE_TWO
	if (left != 0) left->apply_transform(transform);
	if(right != 0) right->apply_transform(transform);
//END_STAGE_TWO
}