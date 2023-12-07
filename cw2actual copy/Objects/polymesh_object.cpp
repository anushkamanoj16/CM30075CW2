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

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "polymesh_object.h"

using namespace std;

int PolyMesh::strtoint(string s)
{
    int r=0;
    for(int i = 0;i < s.length();i+=1)
    {
        if ((s[i] == ' ') || (s[i] == '/'))
        {
            return r;
        } else
{
            r = r*10 + (s[i]-'0');
        }
    }
    return r;
}


// When the file is read, only the vertices and the faces are used. The normals and texture coordinates are ignored.
// Normals are re-constructed from the faces.
PolyMesh::PolyMesh(char* file, bool smooth)
{
//BEGIN_STAGE_ONE
    int count;
    int xpos,ypos,zpos;
    ifstream meshfile(file);

    // I don't recommend hard-coding the storage size, but I had limited time.
    vertex = new Vertex[100000];
    triangle = new TriangleIndex[100000];

    vertex_count = 0;
    triangle_count = 0;

    smoothing = smooth;

    cerr << "Opening meshfile: " << file << endl;

    if (!meshfile.is_open())
    {
        cerr << "Problem reading meshfile (not found)." << endl;
        meshfile.close();
        exit(-1);
    }

    string line;

    while(!meshfile.eof())
    {
        try {
            getline(meshfile, line);
        }
        catch (ifstream::failure e)
        {
            cerr << "Problem reading meshfile (getline failed)." << endl;
        }

	if (line.size() == 0)
        {
            // Empty line
            continue;
        }
        switch(line[0])
        {
          case '#': // comment, do nothing
            break;
          case 'f': // face
            if (line[1] != ' ')
            {
                cerr << "Malformed face line:" << line << endl;
                break;
            }
            // try to parse faces.
            int values[8];
            int c;
            int np,lp;
            lp = line.find(" ") + 1;
            for (c = 0;;)
            {
                if ((line[lp] < '0') || (line[lp] >'9')) // not a valid number, so stop.
		{
			break;
		}
                values[c] = strtoint(line.substr(lp)); // convert to int
                c += 1;

                np = line.find(" ", lp); // look for the next space.
                if (np == std::string::npos) // end of string
                {
                    break;
                }
                lp = np + 1;
            }

            triangle[triangle_count][0] = values[0]-1;
            triangle[triangle_count][1] = values[1]-1;
            triangle[triangle_count][2] = values[2]-1;
            triangle_count += 1;

            if (c > 3)
            {
                triangle[triangle_count][0] = values[2]-1;
                triangle[triangle_count][1] = values[3]-1;
                triangle[triangle_count][2] = values[0]-1;
                triangle_count += 1;
            }

            break;
          case 'v': // vertex property
            switch(line[1])
            {
              case ' ': // a vertex
                xpos = line.find(" ")+1;
                ypos = line.find(" ",xpos)+1;
                zpos = line.find(" ",ypos)+1;

                vertex[vertex_count].x = stof(line.substr(xpos,ypos-xpos-1));

                vertex[vertex_count].y = stof(line.substr(ypos,zpos-ypos-1));

               vertex[vertex_count].z = stof(line.substr(zpos)) + 50.0f;
                vertex[vertex_count].w = 1.0;
                vertex_count += 1;

                break;
              case 'n': // a normal, ignore it.
                break;
              case 't': // texture coordinates, ignore them.
                break;
              default:
                cerr << "Malformed vertex related line in meshfile:" << line << endl;
                break;
            };
            break;
          default: // don't know how to process, so skip
            cerr << "Unhandled line in meshfile:" << line << endl;
            break;
        };
    }

    cerr << "Found " << vertex_count << " vertices." << endl;
    cerr << "Found " << triangle_count << " triangles." << endl;
    meshfile.close();
    cerr << "Meshfile read." << endl;

    // generate face normals

    face_normal = new Vector[triangle_count];

    for (int i = 0; i < triangle_count; i += 1)
    {
        compute_face_normal(i, face_normal[i]);
    }

    // if required compute vertex normals, as we ignored any supplied in the file.

    if (smooth)
    {
        vertex_normal = new Vector[vertex_count];
        compute_vertex_normals();
    }
    else
    {
        vertex_normal = 0;
    }

    cerr << "Meshfile processed." << endl;
//END_STAGE_ONE
    next = 0;
}

//BEGIN_STAGE_ONE
// Moller-Trumbore approach used.
bool PolyMesh::rayTriangleIntersect(const Ray& ray, Vector v0, Vector v1, Vector v2, float& t, float& u, float& v)
{
    Vector p;
    Vector d;
    Vector e1, e2, h, s, q;
    float a, f;

    p.x = ray.position.x;
    p.y = ray.position.y;
    p.z = ray.position.z;
    d = ray.direction;

    e1 = v1 - v0;
    e2 = v2 - v0;

    d.cross(e2, h);
    a = e1.dot(h);

    if (a > -0.00001f && a < 0.00001f)
    {
        return false;
    }

    f = 1 / a;
    s = p - v0;
    u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }

    s.cross(e1, q);
    v = f * d.dot(q);

    if ((v < 0.0f) || ((u + v) > 1.0f))
    {
        return false;
    }

    // compute t

    t = f * e2.dot(q);

    return true;
}

// compute the vertex normals from the face normals.
void PolyMesh::compute_vertex_normals(void)
{

    // The vertex_normal array is already zeroed.

    // accumulate all the face normals for triangles that share a vertex.
    for (int i = 0; i < triangle_count; i += 1)
    {
        for (int j = 0; j < 3; j += 1)
        {
            vertex_normal[triangle[i][j]] = vertex_normal[triangle[i][j]] + face_normal[i];
        }
    }

    // normalise the result.
    for (int i = 0; i < vertex_count; i += 1)
    {
        vertex_normal[i].normalise();
    }
}

// compute face normals for a triangle.
void PolyMesh::compute_face_normal(int which_triangle, Vector& normal)
{
    Vector v0v1, v0v2;
    v0v1.x = vertex[triangle[which_triangle][1]].x - vertex[triangle[which_triangle][0]].x;
    v0v1.y = vertex[triangle[which_triangle][1]].y - vertex[triangle[which_triangle][0]].y;
    v0v1.z = vertex[triangle[which_triangle][1]].z - vertex[triangle[which_triangle][0]].z;

    v0v1.normalise();

    v0v2.x = vertex[triangle[which_triangle][2]].x - vertex[triangle[which_triangle][0]].x;
    v0v2.y = vertex[triangle[which_triangle][2]].y - vertex[triangle[which_triangle][0]].y;
    v0v2.z = vertex[triangle[which_triangle][2]].z - vertex[triangle[which_triangle][0]].z;

    v0v2.normalise();

    v0v1.cross(v0v2, normal);
    normal.normalise();
}

Hit* PolyMesh::triangle_intersection(Ray ray, int which_triangle)
{
    float ndotdir = face_normal[which_triangle].dot(ray.direction);

    if (fabs(ndotdir) < 0.000000001f)
    {
        // ray is parallel to triangle so does not intersect
        return 0;
    }

    float t, u, v;

    bool flag = rayTriangleIntersect(ray, vertex[triangle[which_triangle][0]], vertex[triangle[which_triangle][1]], vertex[triangle[which_triangle][2]], t, u, v);

    if (flag == false) return 0;

    Hit* hit = new Hit();

    hit->t = t;
    hit->what = this;
    hit->position = ray.position + t * ray.direction;

    if (smoothing)
    {
        hit->normal = u * vertex_normal[triangle[which_triangle][1]]
            + v * vertex_normal[triangle[which_triangle][2]]
            + (1 - u - v) * vertex_normal[triangle[which_triangle][0]];
    }
    else
    {
        hit->normal = face_normal[which_triangle];
    }

    hit->normal.normalise();

    if (hit->normal.dot(ray.direction) > 0.0)
    {
        hit->normal.negate();
    }

    return hit;
}
//END_STAGE_ONE

Hit* PolyMesh::intersection(Ray ray)
{
    Hit* hits = 0;
//BEGIN_STAGE_ONE
    int i;

    // Check each triangle, add any hits to sorted list.

    for (i = 0; i < triangle_count; i += 1)
    {
        Hit* hit = triangle_intersection(ray, i);

        if (hit != 0)
        {
            if (hits != 0)
            {
                Hit* step = hits;
                Hit* prev = 0;
                while (step != 0)
                {
                    if (hit->t < step->t)
                    {
                        // if the new hit is in front of the current step, it inserts before it.
                        hit->next = step;
                        if (prev != 0)
                        {
                            prev->next = hit;
                        }
                        else
                        {
                            hits = hit;
                        }
                        break;
                    }

                    prev = step;
                    step = step->next;
                }

                if (step == 0)
                {
                    // hit if bigger than step, insert it afterwards
                    prev->next = hit;
                    hit->next = 0;
                }
            }
            else
            {
                hit->next = 0;
                hits = hit;
            }
        }
    }

    Hit* temp = hits;
    bool entering = true;

    while (temp != 0)
    {
        temp->entering = entering;
        entering = !entering;
        temp = temp->next;
    }
//END_STAGE_ONE
    return hits;
}

void PolyMesh::apply_transform(Transform& trans)
{
//BEGIN_STAGE_ONE
    for (int i = 0; i < vertex_count; i += 1)
    {
        trans.apply(vertex[i]);
    }

    Transform ti = trans.inverse().transpose();

    if (smoothing)
    {

        for (int i = 0; i < vertex_count; i += 1)
        {
            ti.apply(vertex_normal[i]);
        }
    }

    for (int i = 0; i < triangle_count; i += 1)
    {
        ti.apply(face_normal[i]);
    }
    return;
// END_STAGE_ONE
}