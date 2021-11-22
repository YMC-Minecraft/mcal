#ifndef _COMMON_H
#define _COMMON_H

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct vec3d {
	double x;
	double y;
	double z;
};

static int vec3d_parse(char *in, struct vec3d *out)
{
	int i = 0;
	char *p = strtok(in, ",");
	while (p != NULL)
	{
		double *ptr;
		switch (i ++)
		{
			case 0:
				ptr = &out->x;
				break;
			case 1:
				ptr = &out->y;
				break;
			case 2:
				ptr = &out->z;
				break;
			default:
				assert(0);
				break;
		}
		sscanf(p, "%lf", ptr);
		p = strtok(NULL, ",");
	}
	if (i != 3)
	{
		fprintf(stderr, "Invalid Vec3D: %s. Example: -1,1,1.\n", in);
		return 64;
	}
	return 0;
}

#endif /* _COMMON_H */
