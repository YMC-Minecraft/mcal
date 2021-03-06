/*
 * Explosion entity velocity and damage calculator.
 *
 * @1.17.1
 *
 */
#include "common.h"

int main(int argc, char **argv)
{
	struct vec3d tnt = { VEC3D, 0.0, 0.0, 0.0 };
	struct vec3d entity = { VEC3D, 0.0, 0.0, 0.0 };
	struct vec3d velocity = { VEC3D, 0.0, 0.0, 0.0 };
	double entity_eye_y = 0.0;
	double entity_eye_height = 0.0;
	int is_tnt = 1;
	double power = 4.0;
	double exposure = 1.0;
	int blast_prot = 0;
	int number = 1;
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
				"\t-ey vec3d\tTarget entity eye Y (= Y pos + eye height) [Optional if the target is a TNT] [-ey = -eh]\n"
				"\t-eh double\tInstead of using -ey, specify eye height and calculate eye Y [Optional if the target is a TNT] [-ey = -eh]\n"
				"\t-v  vec3d\tTarget entity initial velocity [Optional, defaults to 0,0,0]\n"
				"\t-x  double\tExposure of explosion. Percentage. 1.0 if no blocks blocking the ray. [Optional, defaults to 1.0]\n"
				"\t-b  int\t\tTarget entity blast protection level [Optional]\n"
				"\t-n  int\t\tSimulate multiple explosions (at the same pos) in the same tick, and output the final velocity to the target entity [Optional, defaults to 1]\n"
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
		if (!strcmp("-t", arg)) { if ((r = val_parse(val, (struct val *)&tnt))) return r; }
		else if (!strcmp("-e", arg)) { if ((r = val_parse(val, (struct val *)&entity))) return r; }
		else if (!strcmp("-v", arg)) { if ((r = val_parse(val, (struct val *)&velocity))) return r; }
		else if (!strcmp("-ey", arg))
		{
			if (!is_tnt)
			{
				fprintf(stderr, "-ey cannot be used in conjunction with -eh, or specified multiple times.\n");
				goto usage;
			}
			sscanf(val, "%lf", &entity_eye_y);
			is_tnt = 0;
		}
		else if (!strcmp("-eh", arg))
		{
			if (!is_tnt)
			{
				fprintf(stderr, "-eh cannot be used in conjunction with -ey, or specified multiple times.\n");
				goto usage;
			}
			sscanf(val, "%lf", &entity_eye_height);
			if (entity_eye_height <= 0.0)
			{
				fprintf(stderr, "-ey only accepts > 0.0 numbers.\n");
				goto usage;
			}
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
		else if (!strcmp("-b", arg)) sscanf(val, "%d", &blast_prot);
		else if (!strcmp("-n", arg))
		{
			sscanf(val, "%d", &number);
			if (number < 1)
			{
				fprintf(stderr, "Invalid number of explosions: %d. Must be >= 1.\n",
						number);
				goto usage;
			}
		}
		else
		{
			fprintf(stderr, "Unknown argument: %s\n", arg);
			goto usage;
		}
	}
	if (entity_eye_height > 0.0)
	{
		entity_eye_y = entity.y + entity_eye_height;
		fprintf(stderr, "Calculated eye Y: %.5f + %.5f = %.5f\n", entity.y, entity_eye_height, entity_eye_y);
	}
	fprintf(stderr, "(%.5f, %.5f, %.5f) -> %d x %.2f -> %c(%.5f, %.5f, %.5f)\n",
			tnt.x,
			tnt.y,
			tnt.z,
			number,
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
	fprintf(stderr, "Delta distance normalized: %.5f, %.5f, %.5f\n", xx, yy, zz);
	double accel = (1.0 - (dist / power2)) * exposure;
	if (blast_prot > 0)
	{
		fprintf(stderr, "Applying blact protection level of %d.\n", blast_prot);
		accel -= floor(accel * (double)((float)blast_prot * 0.15f));
	}
	printf("ACCELERATION=%.5f\n", accel);
	int damage = number * ((int) ((accel * accel + accel) / 2.0 * 7.0 * power2 + 1.0));
	printf("DAMAGE=%d\n", damage);
	struct vec3d delta_v = {
		VEC3D,
		number * xx * accel,
		number * yy * accel,
		number * zz * accel
	};
	printf("DELTA_VELOCITY=\"%.5f,%.5f,%.5f\"\n", delta_v.x, delta_v.y, delta_v.z);
	velocity.x += delta_v.x;
	velocity.y += delta_v.y;
	velocity.z += delta_v.z;
	printf("FINAL_VELOCITY=\"%.5f,%.5f,%.5f\"\n", velocity.x, velocity.y, velocity.z);
	return 0;
}
