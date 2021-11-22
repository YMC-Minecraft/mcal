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
		fprintf(stderr, "Usage: %s minX,minY,minZ,maxX,maxY,maxZ (Bounding box of the target entity)\n"
				"\n"
				"Use this tool to simulate the rays to cast for exposure rate calculation.\n"
				"Whenever an entity is to be affected by an explosion, Minecraft will cast lots of rays from the explosion center to the target entity to see how many of them are blocked. The percentage of successful casts will be used for acceleration and damage calculation (see expvc(1) -x).\n"
				"\n"
				"Output: Destination coords for ray casting. The source coord is the explosion center.\n"
				, argv[0]);
		return 64;
	}
	if (val_parse(argv[1], (struct val *)&bbox)) goto usage;
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

