#include "camera.h"

#include "utils.h"

#include <GL/glut.h>
#include <math.h>

void init_camera(struct Camera* camera)
{
    camera->position.x = 15.0;
    camera->position.y = -28.0;
    camera->position.z = 206.5;
	
	camera->min_x= 0.5;
	camera->max_x= 1.5;
	camera->min_y= -2.5;
	camera->max_y= -1.5;

    camera->pose.x = 0;
    camera->pose.y = 0;
    camera->pose.z = 90;
}

void set_view_point(const struct Camera* camera)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-(camera->pose.x + 90), 1.0, 0, 0);
	glRotatef(-(camera->pose.z - 90), 0, 0, 1.0);
	glTranslatef(-camera->position.x, -camera->position.y, -camera->position.z);
}

void rotate_camera(struct Camera* camera, double horizontal, double vertical)
{
	camera->pose.z += horizontal;
	camera->pose.x += vertical;

	if (camera->pose.z < 0) {
		camera->pose.z += 360.0;
	}

	if (camera->pose.z > 360.0) {
		camera->pose.z -= 360.0;
	}

	if (camera->pose.x < 0) {
		camera->pose.x += 360.0;
	}

	if (camera->pose.x > 360.0) {
		camera->pose.x -= 360.0;
	}
}

void move_camera_forward(struct Camera* camera, double distance)
{
	double angle = degree_to_radian(camera->pose.z);

	camera->position.x += cos(angle) * distance;
	camera->position.y += sin(angle) * distance;
	
	camera->min_x += cos(angle) * distance;
	camera->min_y += sin(angle) * distance;	
	camera->max_x += cos(angle) * distance;
	camera->max_y += sin(angle) * distance;
	}

void move_camera_backward(struct Camera* camera, double distance)
{
	double angle = degree_to_radian(camera->pose.z);

	camera->position.x -= cos(angle) * distance;
	camera->position.y -= sin(angle) * distance;
	
	camera->min_x -= cos(angle) * distance;
	camera->min_y -= sin(angle) * distance;	
	camera->max_x -= cos(angle) * distance;
	camera->max_y -= sin(angle) * distance;
}

void step_camera_left(struct Camera* camera, double distance)
{
	double angle = degree_to_radian(camera->pose.z + 90.0);

	camera->position.x += cos(angle) * distance;
	camera->position.y += sin(angle) * distance;
	
	camera->min_x += cos(angle) * distance;
	camera->min_y += sin(angle) * distance;	
	camera->max_x += cos(angle) * distance;
	camera->max_y += sin(angle) * distance;
}

void step_camera_right(struct Camera* camera, double distance)
{
	double angle = degree_to_radian(camera->pose.z - 90.0);

	camera->position.x += cos(angle) * distance;
	camera->position.y += sin(angle) * distance;
	
	camera->min_x -= cos(angle) * distance;
	camera->min_y -= sin(angle) * distance;	
	camera->max_x -= cos(angle) * distance;
	camera->max_y -= sin(angle) * distance;
}

void move_camera_up(struct Camera* camera, double distance)
{
	double angle = degree_to_radian(camera->pose.z);

	camera->position.z +=  distance;
	
}

void move_camera_down(struct Camera* camera, double distance)
{
	double angle = degree_to_radian(camera->pose.z);

	camera->position.z -=  distance;
	
}

float getPositionX(struct Camera* camera){
	float a = camera->position.x;
	return a;
}

float getPositionY(struct Camera* camera){
	float a = camera->position.y;
	return a;
}
