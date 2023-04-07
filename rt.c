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

	printf("The discriminant is %06.2f for the case of:\n", disc);
	printf("Ray\torigin: (%06.2f, %06.2f, %06.2f)\n", ux, uy, uz);
	printf("\tdirection: (%06.2f, %06.2f, %06.2f)\n", vx, vy, vz);
	printf("Sphere\tcentre: (%06.2f, %06.2f, %06.2f)\n", cx, cy, cz);
	printf("\tradius squared: %06.2f\n", r2);

	return 0;
}

