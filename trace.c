
#include "trace.h"

double trace(struct sphere s, struct ray R)
{
	double D[3], D2=0, vD=0, disc=s.r2;
	for (int k=0 ; k<3 ; k++)
	{
		D[k] = R.u[k] - s.c[k];
		D2 += D[k]*D[k];
		vD += R.v[k]*D[k];
	}
	disc += vD*vD - D2;

	if (disc < 0) //discriminant is negative so no intersection
		return INFINITY;
	else if (disc == 0) //grazing intersection
	{
		// dot product of ray's direction vector and vector from sphere to ray's origin in negative,
		// that means the sphere is in the direction of the ray.
		// (since v points in direction of sphere, and D points from the sphere to ray.)
		if (vD < 0)
			return -vD;
		else
			return INFINITY; // positive dot product means the ball is not in front so pretend we didnt see it.
	}
	else // full intersection (disc > 0)
	{
		disc = sqrt(disc); // note: the root of the discriminant (sqrt(b^2-4ac)) gives the distance between 2 factors of a quadratic

		if (vD < 0) // if center of sphere is in the direction of the ray...
		{
			// note:
			// the root of the discriminant (sqrt(b^2-4ac)) is the distance between 2 roots of a quadratic.
			if (-vD > disc) // sphere is entierly in front of the ray's origin.
				return -vD-disc; // return distance to the point of intersection.
			else
				return disc - vD;
		}
		else // if center of sphere is at least party behind the origin of the ray
		{
			if (vD > disc) // vD is greater than distance between the 2 intersections, meaning it's not possible that the sphere is at all in front of us
				return INFINITY; // no points are in front.
			else // one of the points is in front...
				return disc - vD; // return the point which is in front.
		}
	}
}

struct intersect check_spheres(struct ray R, int maxs, const struct sphere SS[maxs])
{
	struct intersect ii = {.dist = INFINITY, .S = NULL};

	for (int si=0 ; si<maxs ; si++)
	{
		double ds = trace(SS[si], R);
		if (ds < ii.dist)
		{
			ii.S = &SS[si];
			ii.dist = ds;
		}
	}

	return ii;
}

void reflect(struct intersect ii, struct ray *Rp) {
	// note: best to start with the abstract-est level of understanding by reading from the bottom up
	/*
	intersection_point = Rp.origin + ii.distance * Rp.direction
	old_ray = intersection_point - ray_origin
	normal_vect = ii.sphere.origin - intersection_point
	normal_unit_vect = normal_vect / |normal_vect|
	component_parallel_to_normal = dot_prod(old_ray, normal_unit_vect) * normal_unit_vect
	new_ray = old_ray - 2 * component_parallel_to_normal
	*/

	// intersection_point = Rp.origin + ii.distance * Rp.direction
	double intersection_point[3];
	for (int k=0 ; k<3 ; k++)
		intersection_point[k] = Rp->u[k] + ii.dist * Rp->v[k];

	// old_ray = intersection_point - ray_origin
	double old_ray[3];
	for (int k=0 ; k<3 ; k++)
		old_ray[k] = intersection_point[k] - Rp->u[k];

	// normal_vect = ii.sphere.origin - intersection_point
	double normal_vect[3];
	for (int k=0 ; k<3 ; k++)
		normal_vect[k] = intersection_point[k] - ii.S->c[k];

	// normal_unit_vect = normal_vect / |normal_vect|
	// note that beacuse we obtain normal_vect by intersection_point minus sphere_center,
	// normal_vect's magnitude is simply the radius of the sphere.

	double normal_unit_vect[3];
	for (int k=0 ; k<3 ; k++)
		normal_unit_vect[k] = normal_vect[k] / ii.S->r;

	// component_parallel_to_normal = dot_prod(old_ray, normal_unit_vect) * normal_unit_vect
	double dot_prod = 0;
	for (int k=0 ; k<3 ; k++)
		dot_prod += old_ray[k]*normal_unit_vect[k];

	double component_parallel_to_normal[3];
	for (int k=0 ; k<3 ; k++)
		component_parallel_to_normal[k] = dot_prod * normal_unit_vect[k];

	// new_ray = old_ray - 2 * component_parallel_to_normal
	double new_ray[3];
	for (int k=0 ; k<3 ; k++)
		new_ray[k] = old_ray[k] - 2 * component_parallel_to_normal[k];

	// now we need to turn this new ray into the format of u + d*v
	// we know u = intersection_point
	// we can get v by normalising new_ray

	double new_ray_mag = 0;
	for (int k=0 ; k<3 ; k++)
		new_ray_mag += new_ray[k]*new_ray[k];
	new_ray_mag = sqrt(new_ray_mag);

	for (int k=0 ; k<3 ; k++)
		Rp->v[k] = new_ray[k]/new_ray_mag;

	// update the origin, but also move it *away* from the true point of intersection,
	// towards the new reflected direction
	// (in order to not have it intersect with the sphere it just collided with)
	// (note to do this with the *new* direction vector)

	// note: for some reason,
	// doing *Rp->u = *intersection_pont ahead of time
	// is not the same as what we currently do
	for (int k=0 ; k<3 ; k++)
		Rp->u[k] = intersection_point[k] + 1e-6 * Rp->v[k];
}
