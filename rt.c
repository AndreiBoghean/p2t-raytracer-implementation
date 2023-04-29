/* 
	Lab 1 Example P2T

	"Almost a ray-tracer"

*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard library, includes stuff for random numbers
#include "trace.h"
#include "structs.h"
#include "lodepng/lodepng.h"


#define N 30
#define M 30
#define MAXS 4
#define MAX_REFLECTS 8

int parse_file(int maxs, struct sphere SS[maxs], char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (NULL == fp) return -1; // file open failed, giving up and returning -1.

	for (int i=0 ; i<maxs ; i++)
	{
		char buffer[7] = {0};
		if (NULL == fgets(buffer, 7, fp)) return -1; // file read failed, giving up and returning 1.
		if (0 != strcmp("SPHERE", buffer)) continue;

		fscanf(fp, " %lf %lf %lf %lf %f %f %f\n", &SS[i].c[0], &SS[i].c[1], &SS[i].c[2], &SS[i].r, &SS[i].colour[0], &SS[i].colour[1], &SS[i].colour[2]);

		SS[i].r2 = SS[i].r*SS[i].r; // we store r^2 but the user gave us r
		for (int k=0 ; k<3 ; k++)
			if (SS[i].colour[k] > 1.0) SS[i].colour[k] = 1.0;
			else if (SS[i].colour[k] < 0.0) SS[i].colour[k] = 0.0;
	}

	fclose(fp);
	fp = NULL;
	return 0;
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

	if (0 != parse_file(sphereCount, SS, "scene")) return 1;

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

			float colour[3] = {0.0, 0.0, 0.0};

			int subray_i;
			for (subray_i=0 ; subray_i<100 ; subray_i++)
			{
				struct ray nR = R;
				double dx, dy;

				do
				{
					double dx = ( 2.0 * rand() / (double) RAND_MAX ) - 1.0;
					double dy = ( 2.0 * rand() / (double) RAND_MAX ) - 1.0;
				}
				while (dx*dx + dy*dy > 1.0);

				// scale down to 3*pixelWidth
				// (3* in order to simulate being slightly out of focus)
				nR.u[0] += 3*dx/horizontalRayCount;
				nR.u[1] += 3*dy/verticalRayCount;

				float _colour[3] = {1.0, 1.0, 1.0};
				trace_path(nR, sphereCount, SS, maxReflections, 0, _colour);
				for (int k=0 ; k<3 ; k++) colour[k] += _colour[k];
			}

			for (int k=0 ; k<3 ; k++) colour[k] /= (subray_i+1);

			// this line is only useful if we switch from multiplicative to an average of colours.
			// since that will mean we no longer haveto multiply by 255
			//*image[verticalRayCount][horizontalRayCount] = *colours;
			for (int k=0 ; k<3 ; k++)
				image[j][i][k] = 255 * colour[k];
		}
	}

	lodepng_encode24_file("render.png", (const unsigned char *) image, horizontalRayCount, verticalRayCount);
	return 0;
}

