#ifndef structs_flag
#define structs_flag

struct sphere
{
	double c[3]; // x,y,z components of sphere centre
	double r; // radius of the sphere
	double r2; // radius of the sphere, squared
	float colour[3]; // R G B [0.0: 1.0]
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

#endif