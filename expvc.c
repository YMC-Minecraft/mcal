/*
 * Explosion entity velocity and damage calculator.
 *
 * @1.17.1
 *
 */
#include "common.h"

int main(int argc, char **argv)
{
	struct vec3d tnt = { 0.0, 0.0, 0.0 };
	struct vec3d entity = { 0.0, 0.0, 0.0 };
	struct vec3d velocity = { 0.0, 0.0, 0.0 };
	double entity_eye_y = 0.0;
	int is_tnt = 1;
	double power = 4.0;
	double exposure = 1.0;
	if (argc <= 1)
	{
		goto usage;
usage:
		fprintf(stderr, "Usage: %s -t 0,0,0 -e 0,0,0 [-v 0,0,0]\n"
				"\n"
				"Options:\n"
				"\t-t  vec3d\tTNT entity coords [Mandatory]\n"
				"\t-e  vec3d\tTarget entity coords [Mandatory]\n"
				"\t-p  double\tExplosion power [Optional, defaults to 4.0 (TNT)]\n"
				"\t-ey vec3d\tTarget entity eye Y [Optional if the target is a TNT]\n"
				"\t-v  vec3d\tTarget entity initial velocity [Optional, defaults to 0,0,0]\n"
				"\t-x  double\tExposure of explosion. Percentage. 1.0 if no blocks blocking the ray. [Optional, defaults to 1.0]\n"
				"\t-h\t\tPrint usage to stderr\n"
				"\n"
				"Vec3D Format: x,y,z. No spaces or brackets allowed.\n", argv[0]);
		return 64;
	}
	int r = 0;
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
		if (!strcmp("-t", arg)) { if ((r = vec3d_parse(val, &tnt))) return r; }
		else if (!strcmp("-e", arg)) { if ((r = vec3d_parse(val, &entity))) return r; }
		else if (!strcmp("-v", arg)) { if ((r = vec3d_parse(val, &velocity))) return r; }
		else if (!strcmp("-ey", arg))
		{
			sscanf(val, "%lf", &entity_eye_y);
			is_tnt = 0;
		}
		else if (!strcmp("-p", arg)) sscanf(val, "%lf", &power);
		else if (!strcmp("-x", arg))
		{
			sscanf(val, "%lf", &exposure);
			if (exposure < 0.0 || exposure > 1.0)
			{
				fprintf(stderr, "Invalid exposure: [0.0, 1.0]\n");
				goto usage;
			}
		}
		else
		{
			fprintf(stderr, "Unknown argument: %s\n", arg);
			goto usage;
		}
	}
	fprintf(stderr, "(%.5f, %.5f, %.5f) -> %.2f -> %c(%.5f, %.5f, %.5f)\n",
			tnt.x,
			tnt.y,
			tnt.z,
			power,
			is_tnt ? 'T' : 'E',
			entity.x,
			entity.y,
			entity.z);
	fprintf(stderr, "WARNING: Explosion range bounding box is ignored. In reality, if the target entity is too far from the explosion center, it will not be affected. We will support that later.\n");
	double power2 = 2.0 * power;
	double xx = entity.x - tnt.x;
	double zz = entity.z - tnt.z;
	double yy = (is_tnt ? entity.y : entity_eye_y) - tnt.y;
	double dist = sqrt(xx * xx + (entity.y - tnt.y) * (entity.y - tnt.y) + zz * zz);
	fprintf(stderr, "Square distance: %.5f\n", dist);
	fprintf(stderr, "WARNING: Explosion immunity is ignored. In reality, if the target entity is immune to explosion, it will not be affected at all.\n");
	if (dist / power2 > 1)
	{
		printf("Not affected, to too far. (Square distance = %.5f).\n", dist);
		return 1;
	}
	fprintf(stderr, "Delta distance: %.5f, %.5f, %.5f\n", xx, yy, zz);
	double dist_eye_y = sqrt(xx * xx + yy * yy + zz * zz);
	if (dist_eye_y == 0)
	{
		printf("Not affected due to dist_eye_y == 0.\n");
		return 1;
	}
	xx /= dist_eye_y;
	yy /= dist_eye_y;
	zz /= dist_eye_y;
	fprintf(stderr, "Delta distance final: %.5f, %.5f, %.5f\n", xx, yy, zz);
	double accel = (1.0 - (dist / power2)) * exposure;
	printf("ACCELERATION=%.5f\n", accel);
	int damage = (int) ((accel * accel + accel) / 2.0 * 7.0 * power2 + 1.0);
	printf("DAMAGE=%d\n", damage);
	fprintf(stderr, "WARNING: Explosion protection is ignored. In reality, if the entity has that protection, the final acceleration may be altered.\n");
	struct vec3d delta_v = {
		xx * accel,
		yy * accel,
		zz * accel
	};
	printf("DELTA_VELOCITY=\"%.5f,%.5f,%.5f\"\n", delta_v.x, delta_v.y, delta_v.z);
	velocity.x += delta_v.x;
	velocity.y += delta_v.y;
	velocity.z += delta_v.z;
	printf("FINAL_VELOCITY=\"%.5f,%.5f,%.5f\"\n", velocity.x, velocity.y, velocity.z);
	return 0;
}
