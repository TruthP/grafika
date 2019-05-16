#ifndef CAMERA_H
#define CAMERA_H

#include "model.h"

struct Camera
{
	struct Vertex position;
	struct Vertex pose;
	
	float min_x;
	float max_x;
	float min_y;
	float max_y;
};

/**
 * Initialize the camera position and direction.
 */
void init_camera(struct Camera* camera);

/**
 * Transform the models into the view point of the camera.
 */
void set_view_point(const struct Camera* camera);

/**
 * Rotate the camera horizontally and vertically.
 */
void rotate_camera(struct Camera* camera, double horizontal, double vertical);

/**
 * Move the camera forward.
 */
void move_camera_forward(struct Camera* camera, double distance);

/**
 * Move the camera backward.
 */
void move_camera_backward(struct Camera* camera, double distance);

/**
 * Step the camera left.
 */
void step_camera_left(struct Camera* camera, double distance);

/**
 * Step the camera right.
 */
void step_camera_right(struct Camera* camera, double distance);

void move_camera_up(struct Camera* camera, double distance);

void move_camera_down(struct Camera* camera, double distance);

float getPositionX(struct Camera* camera);

float getPositionY(struct Camera* camera);

#endif // CAMERA_H

