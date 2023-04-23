/* 
	Lab 1 Example P2T

	"Almost a ray-tracer"

*/

#include <stdio.h>
#include <math.h>

#define N 30
#define M 30
#define MAXS 4
#define MAX_REFLECTS 8

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
	new_ray_mag = fast_sqrt(new_ray_mag);

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

// a function which fully traces out a ray's path among spheres, and returns the number of hits
int trace_path(struct ray R, int maxs, const struct sphere SS[maxs], int max_reflect, int reflected) {
	//printf("checking for hit %d\n", reflected);
	if (reflected >= max_reflect)
		return reflected;

	struct intersect ii = check_spheres(R, maxs, SS);

	if (ii.S)
	{
		reflect(ii, &R);
		return trace_path(R, maxs, SS, max_reflect, reflected+1);
	}
	else
		return reflected;
}

int main(int argc, char * argv[])
{
	int horizontalRayCount=N, verticalRayCount=M, sphereCount=MAXS, maxReflections=MAX_REFLECTS;

	if (argc == 5)
	{
		sscanf(argv[1], "%d", &horizontalRayCount);
		sscanf(argv[2], "%d", &verticalRayCount);
		sscanf(argv[3], "%d", &sphereCount);
		sscanf(argv[4], "%d", &maxReflections);
	}
	else if (argc == 4)
	{
		sscanf(argv[1], "%d", &horizontalRayCount);
		sscanf(argv[2], "%d", &verticalRayCount);
		sscanf(argv[3], "%d", &sphereCount);
	}
	else if (argc == 3)
	{
		sscanf(argv[1], "%d", &horizontalRayCount);
		sscanf(argv[2], "%d", &verticalRayCount);
	}

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
			struct ray R =
			{
				.u =
				{
					(2 * i/((double)horizontalRayCount)) - 1,
					(2 * j/((double)verticalRayCount)) - 1,
					0
				},
				.v =
				{
					0,
					0,
					1
				}
			};

			int hitCount = trace_path(R, sphereCount, SS, maxReflections, 0);

			printf(hitCount > 0 ? "%d" : " ", hitCount);
		}
		printf("\n");
	}


	return 0;
}

