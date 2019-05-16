struct Light
{
	float light_x;
	float light_y;
	float light_z;
	float shininess;
	
	GLfloat light_position[] = {light.light_x, light.light_y, light.light_z, 0};
}

void set_light_position(struct Light* light, float x, float y, float z);

void init_light(struct Light* light);