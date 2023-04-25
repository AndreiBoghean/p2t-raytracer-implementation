
#include <stdio.h>
#include <math.h>
#include "structs.h"

double trace(struct sphere s, struct ray R);

struct intersect check_spheres(struct ray R, int maxs, const struct sphere SS[maxs]);

void reflect(struct intersect ii, struct ray *Rp);

int trace_path(struct ray R, int maxs, const struct sphere SS[maxs], int max_reflect, int reflected, float colour[3]);