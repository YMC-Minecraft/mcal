/*
 * Explosion exposure rays simulator.
 *
 * @1.17.1
 */

#include "common.h"

static inline double lerp(double delta, double start, double end) { return start + delta * (end - start); }

int main(int argc, char **argv)
{
	struct box bbox = { BOX, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	if (argc <= 1)
	{
		goto usage;
usage:
		fprintf(stderr, "Usage: %s minX,minY,minZ,maxX,maxY,maxZ\n"
				"\n"
				"Options:\n"
				"\t-b bounding box\tEntity bounding box. Position dependent. [Mandatory, or -d]"
				"\t-d dimension\tUse entity dimension instead of bounding box. Note: output is also position independent.\n"
				"\t-p vec3d\tOptional argument used with -d. Specify the position of the entity so expexc will show the final position dependent destinations. Same effect with -b alone. [Optional, defaults to 0,0,0]\n"
				"\n"
				"Bounding box format: minX,minY,minZ,maxX,maxY,maxZ. No spaces or brackets allowed.\n"
				"Dimension format: width,height. No spaces or brackets allowed.\n"
				"Use this tool to simulate the rays to cast for exposure rate calculation.\n"
				"Whenever an entity is to be affected by an explosion, Minecraft will cast lots of rays from the explosion center to the target entity to see how many of them are blocked. The percentage of successful casts will be used for acceleration and damage calculation (see expvc(1) -x).\n"
				"\n"
				"Output: Destination coords for ray casting. The source coord is the explosion center.\n"
				, argv[0]);
		return 64;
	}
	struct vec3d position = { VEC3D, 0.0, 0.0, 0.0 };
	struct entity_dimen dimen = { ENTITY_DIMEN, 0.0, 0.0 };
	int arg_supplied = 0;
	int use_bbox = 1;
	for (int i = 1; i < argc; i ++)
	{
		const char *arg = argv[i];
		if (!strcmp("-h", arg)) goto usage;
		if (i == argc - 1)
		{
			fprintf(stderr, "%s requires an argument.\n", arg);
			goto usage;
		}
		char *val = argv[++i];
		if (!strcmp("-b", arg))
		{
			if (arg_supplied ++)
			{
				fprintf(stderr, "-d is already supplied. It cannot be used in conjunction with -b.\n");
				goto usage;
			}
			if (val_parse(val, (struct val *)&bbox)) goto usage;
		}
		else if (!strcmp("-d", arg))
		{
			if (arg_supplied ++)
			{
				fprintf(stderr, "-b is already supplied. It cannot be used in conjunction with -d.\n");
				goto usage;
			}
			if (val_parse(val, (struct val *)&dimen)) goto usage;
			use_bbox = 0;
		}
		else if (!strcmp("-p", arg)) { if (val_parse(val, (struct val *)&position)) goto usage; }
		else
		{
			fprintf(stderr, "Unknown option %s.\n", arg);
			goto usage;
		}
	}
	if (!arg_supplied)
	{
		fprintf(stderr, "You must supply -b or -d.\n");
		goto usage;
	}
	if (!use_bbox)
	{
		double half_width = dimen.width / 2.0;
		bbox.min_x = position.x -half_width;
		bbox.min_y = position.y;
		bbox.min_z = position.z -half_width;
		bbox.max_x = position.x + half_width;
		bbox.max_y = position.y + dimen.height;
		bbox.max_z = position.z + half_width;
	}
	fprintf(stderr, "Box(%.2f, %.2f, %.2f -> %.2f, %.2f, %.2f)\n",
			bbox.min_x,
			bbox.min_y,
			bbox.min_z,
			bbox.max_x,
			bbox.max_y,
			bbox.max_z);
	double x_step = 1.0 / ((bbox.max_x - bbox.min_x) * 2.0 + 1.0);
	double y_step = 1.0 / ((bbox.max_y - bbox.min_y) * 2.0 + 1.0);
	double z_step = 1.0 / ((bbox.max_z - bbox.min_z) * 2.0 + 1.0);
	double x_half_remain = (1.0 - floor(1.0 / x_step) * x_step) / 2.0;
	double z_half_remain = (1.0 - floor(1.0 / z_step) * z_step) / 2.0;
	if (x_step < 0.0 || y_step < 0.0 || z_step < 0.0) {
		fprintf(stderr, "Iteration step length is less than zero. This means that the max dimensions are smaller than the min dimensions. No ray is possible.\n");
        	return 0;
        }
	for (float x_samp_pos_delta_percentage = 0.0f; x_samp_pos_delta_percentage <= 1.0f; x_samp_pos_delta_percentage += x_step)
	{
        	for (float y_samp_pos_delta_percentage = 0.0f; y_samp_pos_delta_percentage <= 1.0f; y_samp_pos_delta_percentage += y_step)
		{
			for (float z_samp_pos_delta_percentage = 0.0f; z_samp_pos_delta_percentage <= 1.0f; z_samp_pos_delta_percentage += z_step)
			{
				double x_samp_pos = lerp(x_samp_pos_delta_percentage, bbox.min_x, bbox.max_x);
				double y_samp_pos = lerp(y_samp_pos_delta_percentage, bbox.min_y, bbox.max_y);
				double z_samp_pos = lerp(z_samp_pos_delta_percentage, bbox.min_z, bbox.max_z);
				printf("%.5f,%.5f,%.5f\n",
						x_samp_pos + x_half_remain,
						y_samp_pos,
						z_samp_pos + z_half_remain);
			}
		}
	}
	return 0;
}

