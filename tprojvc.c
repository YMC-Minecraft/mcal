/*
 * Thrown projectile velocity calculator.
 *
 * @1.17.1
 *
 */
#include "common.h"

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		goto usage;
usage:
		fprintf(stderr, "Usage: %s -p 0,0,0 -v 0,0,0 -t 10 [-w] [-ng]\n"
				"\n"
				"Options:\n"
				"\t-p vec3d\tInitial position [Mandatory]\n"
				"\t-v vec3d\tInitial velocity [Mandatory]\n"
				"\t-t int\tTicks to simulate [Mandatory]\n"
				"\t-w\tIn water [Optional]\n"
				"\t-ng\tNo gravity [Optional]\n"
				"\t-h\tShow this help\n"
				"\n"
				"Vec3D Format: x,y,z. No spaces or brackets allowed.\n"
				"Numbers represent post-tick values.\n",
				argv[0]
		       );
		return 64;
	}
	struct vec3d v = { VEC3D, 0, 0, 0 };
	struct vec3d pos = { VEC3D, 0, 0, 0 };
	int ticks = 0;
	int water = 0;
	int no_gravity = 0;

	int r = 0;
	for (int i = 1; i < argc; i ++)
	{
		const char *arg = argv[i];
                if (!strcmp("-h", arg)) { goto usage; }
		else if (!strcmp("-w", arg)) { water = 1; continue; }
		else if (!strcmp("-ng", arg)) { no_gravity = 1; continue; }
                if (i == argc - 1)
                {
                        fprintf(stderr, "%s requires an argument.\n", arg);
                        goto usage;
                }
                char *val = argv[++i];
		if (!strcmp("-p", arg)) { if ((r = val_parse(val, (struct val *)&pos))) return r; }
		else if (!strcmp("-v", arg)) { if ((r = val_parse(val, (struct val *)&v))) return r; }
		else if (!strcmp("-t", arg))
		{
			sscanf(val, "%d", &ticks);
			if (ticks <= 0)
			{
				fprintf(stderr, "-t requires a natural number.\n");
				goto usage;
			}
		}
		else
                {
                        fprintf(stderr, "Unknown argument: %s\n", arg);
                        goto usage;
                }
	}

	fprintf(stderr, "t,x,y,z,vx,vy,vz\n");
	/* Initial value. Print to stderr (classified as verbose msg) */
	fprintf(stderr, "%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f\n",
			0,
			pos.x,
			pos.y,
			pos.z,
			v.x,
			v.y,
			v.z);
	const float deaccel = water ? 0.8f : 0.99f;
	for (int i = 1; i <= ticks; i ++)
	{
		pos.x += v.x;
		pos.y += v.y;
		pos.z += v.z;
		v.x *= deaccel;
		v.y *= deaccel;
		v.z *= deaccel;
		v.y -= no_gravity ? 0.0f : 0.03f;
		printf("%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f\n",
				i,
				pos.x,
				pos.y,
				pos.z,
				v.x,
				v.y,
				v.z);
	}
	return 0;
}

