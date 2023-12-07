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

#include "quadratic_object.h"
#include <math.h>
#include <cmath>

using namespace std;

Quadratic::Quadratic(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j)
{
  // Output the coefficients for debugging purposes
  cout << "Quadratic object created with coefficients: "
         << a << ", " << b << ", " << c << ", " << d << ", " << e << ", "
         << f << ", " << g << ", " << h << ", " << i << ", " << j << endl;

  // Initialize the coefficients of the quadratic equation
  this->A = a;
  this->B = b;
  this->C = c;
  this->D = d;
  this->E = e;
  this->F = f;
  this->G = g;
  this->H = h;
  this->I = i;
  this->J = j;
}

// Method to compute the intersection of a ray with the quadratic surface
Hit *Quadratic::intersection(Ray ray)
{
    // Extract the position and direction components of the ray
    float Px = ray.position.x;
    float Py = ray.position.y;
    float Pz = ray.position.z;

    float Dx = ray.direction.x;
    float Dy = ray.direction.y;
    float Dz = ray.direction.z;

    // Calculate the coefficients of the quadratic equation for ray intersection
    float Aq = A * Dx * Dx + 2 * B * Dx * Dy + 2 * C * Dx * Dz + E * Dy * Dy + 2 * F * Dy * Dz + H * Dz * Dz;
    float Bq = 2 * (A * Px * Dx + B * (Px * Dy + Dx * Py) + C * (Px * Dz + Dx * Pz) + D * Dx + E * Py * Dy + F * (Py * Dz + Dy * Pz) + G * Dy + H * Pz * Dz + I * Dz);
    float Cq = A * Px * Px + 2 * B * Px * Py + 2 * C * Px * Pz + 2 * D * Px + E * Py * Py + 2 * F * Py * Pz + 2 * G * Py + H * Pz * Pz + 2 * I * Pz + J;

    // Calculate the discriminant to check for real solutions
    float discriminant = Bq * Bq - 4 * Aq * Cq;

    // Check for no real solutions (no intersection)
    if (discriminant < 0.0f)
    {
        return 0; // No intersection
    }

    // Handle the linear case (Aq is very small)
    if (std::fabs(Aq) < 1e-8f)
    {
        float t = -Cq / Bq;
        if(t <= 0)   // No intersection for linear case
        {
            return 0;
        }

        // Compute the intersection point and normal for the linear case
        Vertex hit_position = ray.position + t * ray.direction;
        Vector normal = this->compute_normal(hit_position);

        // Create a new Hit object to store intersection data
        Hit* hit0 = new Hit();
        hit0->what = this;
        hit0->t = t;
        hit0->entering = true;
        hit0->position = hit_position;
        hit0->normal = normal;
        hit0->next = 0;

        // Adjust the normal direction if necessary
        if (hit0->normal.dot(ray.direction) > 0.0)
        {
            hit0->normal.negate();
        }

        return hit0;
    }

    // Handle the standard quadratic case
    float sqrtDiscriminant = sqrt(discriminant);
    float t0 = (-Bq - sqrtDiscriminant) / (2 * Aq);
    float t1 = (-Bq + sqrtDiscriminant) / (2 * Aq);

    // Check for no intersection in the quadratic case
    if(t1 <= 0.0f)
    {
        return 0;
    }

    // Handle the case with one intersection point
    if (t0 < 0.0f & t1 > 0.0f)
    {
        Vertex hit_position0 = ray.position + t1 * ray.direction;
        Vector normal0 = this->compute_normal(hit_position0);

        // Create a new Hit object for the intersection
        Hit* hit0 = new Hit();
        hit0->what = this;
        hit0->t = t1;
        hit0->entering = true;
        hit0->position = hit_position0;
        hit0->normal = normal0;
        hit0->next = 0;

        // Adjust the normal direction if necessary
        if (hit0->normal.dot(ray.direction) > 0.0)
        {
            hit0->normal.negate();
        }

        return hit0;
    }

    // Handle the case with two intersection points
    if (t0 > 0)
    {
        // First intersection
        Vertex hit_position0 = ray.position + t0 * ray.direction;
        Vector normal0 = this->compute_normal(hit_position0);

        // Create a new Hit object for the first intersection
        Hit* hit0 = new Hit();
        hit0->what = this;
        hit0->t = t0;
        hit0->entering = true;

        hit0->position = hit_position0;
        hit0->normal = normal0;
        hit0->normal.normalise();

        // Check if the normal vector is pointing in the same direction as the ray
        if (hit0->normal.dot(ray.direction) > 0.0)
        {
            // If the normal is in the same direction as the ray, invert it
            // This is necessary to ensure the normal always points 'outwards' from the surface
            hit0->normal.negate();
        }

        // Calculate the position of the second intersection point
        Vertex hit_position1 = ray.position + t1 * ray.direction;
        // Compute the normal at the second intersection point
        Vector normal1 = this->compute_normal(hit_position1);

        // Create a new Hit object for the second intersection
        Hit* hit1 = new Hit();
        hit1->what = this; // Reference to the object being intersected
        hit1->t = t1; // The 't' value (distance along the ray) to the intersection
        hit1->entering = false; // Indicates whether the ray is entering or exiting the object

        hit1->position = hit_position1; // Position of the intersection
        hit1->normal = normal1; // Normal at the intersection

        if (hit1->normal.dot(ray.direction) > 0.0)
        {
            // If the normal is in the same direction as the ray, invert it
            hit1->normal.negate();
        }

        // Link the two Hit objects (for cases where there are two intersections)
        hit0->next = hit1; // The next intersection after hit0 is hit1
        hit1->next = 0; // There are no further intersections after hit1

        // Return the first Hit object, which is linked to the second one
        return hit0;
    }
}


// Compute the normal vector at a given point on the quadratic surface
Vector Quadratic::compute_normal(Vertex& point)
{
    // Calculate the gradient of the quadratic surface at the given point
    // The gradient vector is perpendicular to the surface and represents the normal
    Vector normal(
        2 * (A * point.x + B * point.y + C * point.z + D), // Partial derivative with respect to x
        2 * (B * point.x + E * point.y + F * point.z + G), // Partial derivative with respect to y
        2 * (C * point.x + F * point.y + H * point.z + I)  // Partial derivative with respect to z
    );

    // Normalize the normal vector to ensure it has a unit length
    normal.normalise();

    // Return the normalized normal vector
    return normal;
}

// Apply a transformation to the quadratic surface
void Quadratic::apply_transform(Transform& trans)
{
    // Create a transformation matrix from the current coefficients of the quadratic equation
    Transform coef = Transform(
            A, B, C, D,
            B, E, F, G,
            C, F, H, I,
            D, G, I, J
        );

    // Apply the transformation to the coefficients
    // This involves multiplying the transpose of the transformation matrix with the coefficient matrix
    // and then multiplying the result with the transformation matrix
    Transform new_coef = trans.transpose() * coef * trans;

    // Update the coefficients of the quadratic equation with the transformed values
    this->A = new_coef.matrix[0][0];
    this->B = new_coef.matrix[0][1];
    this->C = new_coef.matrix[0][2];
    this->D = new_coef.matrix[0][3];
    this->E = new_coef.matrix[1][1];
    this->F = new_coef.matrix[1][2];
    this->G = new_coef.matrix[1][3];
    this->H = new_coef.matrix[2][2];
    this->I = new_coef.matrix[2][3];
    this->J = new_coef.matrix[3][3];
}