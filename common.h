#ifndef _COMMON_H
#define _COMMON_H

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

enum val_type { VEC3D, BOX };

struct val {
	enum val_type type;
};

struct vec3d {
	enum val_type type;
	double x;
	double y;
	double z;
};

struct box {
	enum val_type type;
	double min_x;
	double min_y;
	double min_z;
	double max_x;
	double max_y;
	double max_z;
};

static inline double *val_get_index(struct val *val, int index)
{
	switch (val->type)
	{
		case VEC3D:
			struct vec3d *d_v3d = (struct vec3d *)val;
			switch (index)
			{
				case 0: return &d_v3d->x;
				case 1: return &d_v3d->y;
				case 2: return &d_v3d->z;
				default: assert(0);
			}
		case BOX:
			struct box *v_box = (struct box *)val;
			switch (index)
			{
				case 0: return &v_box->min_x;
				case 1: return &v_box->min_y;
				case 2: return &v_box->min_z;
				case 3: return &v_box->max_x;
				case 4: return &v_box->max_y;
				case 5: return &v_box->max_z;
				default: assert(0);
			}
		default:
			assert(0);
	}
}

static int val_parse(char *in, struct val *o)
{
	int i = 0;
	char *p = strtok(in, ",");
	int required_vars = 0;
	switch (o->type)
	{
		case VEC3D:
			required_vars = 3;
			break;
		case BOX:
			required_vars = 6;
			break;
	}
	while (p != NULL)
	{
		sscanf(p, "%lf", val_get_index(o, i++));
		p = strtok(NULL, ",");
	}
	if (i != required_vars)
	{
		fprintf(stderr, "Invalid value: %s.\n", in);
		return 64;
	}
	return 0;
}

#endif /* _COMMON_H */
