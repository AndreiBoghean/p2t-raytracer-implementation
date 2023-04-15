/* 
	Lab 1 Example P2T

	"Almost a ray-tracer"

*/

#include <stdio.h>
#include <math.h>

#define N 30
#define M 30
#define MAXS 4

struct sphere
{
	double c[3]; // x,y,z components of sphere centre
	double r; // radius of the sphere
	double r2; // radius of the sphere, squared
};

struct ray // ray = u + d*v
{
	double u[3]; // origin of ray
	double v[3]; // normalised direction vector of the ray
};

struct intersect
{
	double dist; // distance along ray of intersection.
	const struct sphere *S; // pointer to the sphere object that was hit.
};

double fast_sqrt(double x)
{
	asm("sqrtsd %0, %0" : "=v" (x) : "v" (x) );
	return x;
}


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
		return 1;
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
		disc = fast_sqrt(disc); // note: the root of the discriminant (sqrt(b^2-4ac)) gives the distance between 2 factors of a quadratic

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

void reflect(struct intersect ii, struct ray *Rp) {
	// note: best to start with the abstract-est level of understanding by reading from the bottom up
	/*
	intersection_point = Rp.origin + ii.distance * Rp.direction
	normal_vect = ii.sphere.origin - intersection_point
	normal_unit_vect = normal_vect / |normal_vect|
	component_parallel_to_normal = dot_prod(old_ray, normal_vect) * normal_unit_vect
	new_ray = old_ray - 2 * component_parallel_to_normal
	*/

	// intersection_point = Rp.origin + ii.distance * Rp.direction
	double intersection_point[3] = {0, 0, 0};
	for (int k=0 ; k<3 ; k++)
		intersection_point[k] += Rp->u[k] + ii.dist * Rp->v[k];

	// normal_vect = ii.sphere.origin - intersection_point
	double normal_vect[3] = {0, 0, 0};
	for (int k=0 ; k<3 ; k++)
		normal_vect[k] += ii.S->c[k] - intersection_point[k];

	// normal_unit_vect = normal_vect / |normal_vect|

	//double normal_vect_mag = 0;
	//for (int k=0 ; k<3 ; k++)
	//	normal_vect_mag += normal_vect[k]*normal_vect[k];
	//normal_vect_mag = fast_sqrt(normal_vect_mag);

	double normal_unit_vect[3] = {0, 0, 0};
	for (int k=0 ; k<3 ; k++)
		normal_unit_vect[k] += normal_vect[k] / ii.S->r;

	// component_parallel_to_normal = dot_prod(old_ray, normal_unit_vect) * normal_unit_vect
	double dot_prod = 0;
	for (int k=0 ; k<3 ; k++)
		dot_prod += intersection_point[k]*normal_unit_vect[k];

	double component_parallel_to_normal[3] = {0, 0, 0};
	for (int k=0 ; k<3 ; k++)
		component_parallel_to_normal[k] += dot_prod * normal_unit_vect[k];

	// new_ray = old_ray - 2 * component_parallel_to_normal
	double new_ray[3];
	*new_ray = *intersection_point;

	for (int k=0 ; k<3 ; k++)
		new_ray[k] -= 2 * component_parallel_to_normal[k];

	// now we need to turn this new ray into the format of u + d*v
	// we know u = intersection_point
	// we can get v by normalising new_ray

	*Rp->u = *intersection_point;

	double new_ray_mag = 0;
	for (int k=0 ; k<3 ; k++)
		new_ray_mag += new_ray[k]*new_ray[k];
	new_ray_mag = fast_sqrt(new_ray_mag);

	for (int k=0 ; k<3 ; k++)
		Rp->v[k] = new_ray[k]/new_ray_mag;
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

int main(void)
{
	int horizontalRayCount=N, verticalRayCount=M, sphereCount=MAXS;

	struct sphere SS[sphereCount];

	for (int i=0 ; i<sphereCount ; i++)
	{
		printf("please enter the x, y, and z value for sphere %d in format x,y,z\n", i);
		scanf("%lf,%lf,%lf", &SS[i].c[0], &SS[i].c[1], &SS[i].c[2]);
		printf("please enter radius of the sphere\n");
		scanf("%lf", &SS[i].r);
		SS[i].r2 = SS[i].r*SS[i].r; // we store r^2 but the user gave us r
	}


	//struct ray R = { .u = {0, 0, 0}, .v = {0, 0, 1} };
	// ^ note that the plane from which the orthographic projection starts from
	// does not vary in z only in x and y. z will not change from the inital value of 0.

	for (int j=0; j<verticalRayCount ; j++ )
	{
		//R.u[1] = (2 * j/((double)verticalRayCount)) - 1;
		// ^ scale any number of rays down to a range from -1 to 1 (exclusive (since we want the side of our "pixels" to be touching the side of the viewport, rather than the pixels being centered on the side of the viewport.))

		for (int i=0 ; i<horizontalRayCount ; i++ )
		{
			//R.u[0] = (2 * i/((double)horizontalRayCount)) - 1;
			// ^ scale any number of rays down to a range from -1 to 1 (exclusive (since we want the side of our "pixels" to be touching the side of the viewport, rather than the pixels being centered on the side of the viewport.))

			// alternate code to generate a whole new ray object each loop.
			// (this was needed because we altered the code such that reflections modify the original ray)
			// and thusly reusing the same ray which hash now been modified will not work.
			// (also, conceptually it does not make sense to essentially take the same photon and reuse it)
			struct ray R = {
				.u = {
						(2 * i/((double)horizontalRayCount)) - 1,
						(2 * j/((double)verticalRayCount)) - 1,
						0
				},
				.v = {
					0,
					0,
					1
				}
			};

			double temp[3] = {0, 0, 1};
			*R.v = *temp; // a ray along the z direction


			struct intersect ii = check_spheres(R, sphereCount, SS);
			double d = ii.dist;

			if (d == INFINITY)
			{
				printf(" ");
			}
			else
			{
				printf("X");
				reflect(ii, &R);
			}

		}
		printf("\n");
	}


	return 0;
}

