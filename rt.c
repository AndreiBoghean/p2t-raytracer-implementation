/* 
	Lab 1 Example P2T

	"Almost a ray-tracer"

*/

#include <stdio.h>
#include <math.h>
#include "trace.h"
#include "structs.h"
#include "lodepng/lodepng.h"


#define N 30
#define M 30
#define MAXS 4
#define MAX_REFLECTS 8



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
		printf("please enter the R, G, and B value for sphere %d in format R,G,B in the range [0,0: 1.0]\n", i);
		scanf("%f,%f,%f", &SS[i].colour[0], &SS[i].colour[1], &SS[i].colour[2]);
		for (int k=0 ; k<3 ; k++)
			if (SS[i].colour[k] > 1.0) SS[i].colour[k] = 1.0;
			else if (SS[i].colour[k] < 0.0) SS[i].colour[k] = 0.0;
	}

	unsigned char image[verticalRayCount][horizontalRayCount][3];

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

			float colour[3] = {1.0, 1.0, 1.0};
			int hitCount = trace_path(R, sphereCount, SS, maxReflections, 0, colour);

			// this line is only useful if we switch from multiplicative to an average of colours.
			// since that will mean we no longer haveto multiply by 255
			//*image[verticalRayCount][horizontalRayCount] = *colours;
			for (int k=0 ; k<3 ; k++)
				image[j][i][k] = 255 * colour[k];
			//printf(hitCount > 0 ? "%d" : " ", hitCount);
		}
		//printf("\n");
	}

	lodepng_encode24_file("render.png", (const unsigned char *) image, horizontalRayCount, verticalRayCount);
	return 0;
}

