/* 
	Lab 1 Example P2T

	"Almost a ray-tracer"

*/

#include <stdio.h>

int main(void) {
	double cx, cy, cz, r2; //x,y,z components of sphere centre, radius squared
	cx = 1.0 ; cy = 1.0; cz = 2.0; r2 = 1.5; //change here to move sphere around
	double ux, uy, uz, vx, vy, vz; //origin of line, unit vector direction 
	ux = uy = uz = 0; //shoot ray from the actual origin to start with
	vx = vy = 0; vz = 1.0; //a ray along the z direction

	double Dx = ux - cx, Dy = uy - cy, Dz = uz - cz;
	double D2 = Dx*Dx + Dy*Dy + Dz*Dz;
	double vD = vx*Dx + vy*Dy + vz*Dz ;
	double disc = vD*vD - D2 + r2;


	if (disc < 0) //discriminant is negative so no intersection
		{ printf(" "); }
	else //grazing or full intersection
		{ printf("X"); }

	return 0;
}

