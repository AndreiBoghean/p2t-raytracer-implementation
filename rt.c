/* 
	Lab 1 Example P2T

	"Almost a ray-tracer"

*/

#include <stdio.h>

#define N 30
#define M 30

int main(void) {
	int horizontalRayCount=N, verticalRayCount=M;

	double cx, cy, cz, r2; //x,y,z components of sphere centre, radius squared
	printf("please enter the x, y, and z value of the sphere in format x,y,z\n");
	scanf("%lf,%lf,%lf", &cx, &cy, &cz);
	printf("please enter radius of the sphere\n");
	scanf("%lf", &r2);
	r2 = r2*r2; // we store r^2 but the user gave us r


	double ux, uy, uz, vx, vy, vz; //origin of line, unit vector direction

	uz = 0; // plane from which the orthographic projection starts from does not vary in z,
			// only in x and y, so we can keep z constantly set to 0 from the begining.

	for (int j=0; j<verticalRayCount ; j++ )
	{
		uy = (2 * j/((double)verticalRayCount)) - 1;
		// ^ scale any number of rays down to a range from -1 to 1 (exclusive (since we want the side of our "pixels" to be touching the side of the viewport, rather than the pixels being centered on the side of the viewport.))

		for (int i=0 ; i<horizontalRayCount ; i++ )
		{
			ux = (2 * i/((double)horizontalRayCount)) - 1;
			// ^ scale any number of rays down to a range from -1 to 1 (exclusive (since we want the side of our "pixels" to be touching the side of the viewport, rather than the pixels being centered on the side of the viewport.))

			vx = vy = 0; vz = 1.0; //a ray along the z direction

			double Dx = ux - cx, Dy = uy - cy, Dz = uz - cz;
			double D2 = Dx*Dx + Dy*Dy + Dz*Dz;
			double vD = vx*Dx + vy*Dy + vz*Dz ;
			double disc = vD*vD - D2 + r2;


			if (disc < 0) //discriminant is negative so no intersection
				{ printf(" "); }
			else //grazing or full intersection
				{ printf("X"); }
		}
		printf("\n");
	}


	return 0;
}

