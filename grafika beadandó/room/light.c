#include "light.h"
#include "utils.h"

#include <GL/glut.h>
#include <math.h>

void set_light_position(struct Light* light, float x, float y, float z)
{
	light->light_x= x;
	light->light_y= y;
	light->light_z= z;
	
	GLfloat light_position[] = {light.light_x, light.light_y, light.light_z, 0};
	light->shininess = 50;
}

void init_light(struct Light* light)
{
	GLfloat light_position[] = {light.light_x, light.light_y, light.light_z, 0};
}