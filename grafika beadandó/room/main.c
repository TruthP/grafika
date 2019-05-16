#include "camera.h"
#include "model.h"
#include "draw.h"

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <SOIL/SOIL.h>

#include <stdio.h>
#include <math.h>

#define VIEWPORT_RATIO (4.0 / 3.0)
#define VIEWPORT_ASPECT 50.0

#define CAMERA_SPEED 10.0

int mouse_x, mouse_y;

struct Model model;
struct Model model1;
struct Model model2;

struct Camera camera;

int nw = 640;
int nh = 480;
int help = 0;

struct Action
{
    int move_forward;
    int move_backward;
    int step_left;
    int step_right;
	int move_up;
	int move_down;
};

struct Action action;
int time;
int sky_texture_id;
float shine = 3.0;
float lightx = 0.0;
float lighty = 0.0;
float lightz = 0.0;
float lightw = 1.0;

float ajtoZ = 15.0;
int stop = 1;

typedef GLubyte Pixel[3]; /*represents red green blue*/

Pixel* images[3];
GLuint texture_names[7];
//Textúra betöltése fájlból. Visszatér a textúra nevével.
GLuint load_texture(char* filename, Pixel* image)
{
    GLuint texture_name;
    glGenTextures(1, &texture_name);

    int width;
    int height;

    image = (Pixel*)SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (Pixel*)image);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //legközelebbi színt hasznája a kirajzoláshoz, gyorsabb, mint a LINEAR
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texture_name;
}

void initialize_texture()
{
    unsigned int i;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    char texture_filenames[][32] = {
        "textures/blue.png", //0
        "textures/plafon.png",
        "textures/space.png",
		"textures/padlo.png",
		"",
		"textures/ocean.jpg", //5
		"textures/sky.jpg",
		"textures/help.png",
		"textures/pFal.png",
		"textures/ajto.png",
		"textures/tiger.png",  //10
		"textures/asztal.png"
    };

    for (i = 0; i < 13; ++i) {
        texture_names[i] = load_texture(texture_filenames[i], images[i]);
    }
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
}


void update_camera_position(struct Camera* camera, double elapsed_time)
{
    double distance;

    distance = elapsed_time * CAMERA_SPEED;

    if (action.move_forward == TRUE) {
		move_camera_forward(camera, distance);
    }

    if (action.move_backward == TRUE) {
		move_camera_backward(camera, distance);
    }

    if (action.step_left == TRUE) {
	    step_camera_left(camera, distance);
    }

    if (action.step_right == TRUE) {
		step_camera_right(camera, distance);
    }
	if (action.move_up == TRUE) {
		move_camera_up(camera, distance);
	}
	if (action.move_down == TRUE) {
		move_camera_down(camera, distance);
	}
}

double calc_elapsed_time()
{
    int current_time;
    double elapsed_time;

    current_time = glutGet(GLUT_ELAPSED_TIME);
    elapsed_time = (double)(current_time - time) /1000.0;
    time = current_time;

    return elapsed_time;
}

void draw_skybox()
{
    double theta, phi1, phi2;
    double x1, y1, z1;
    double x2, y2, z2;
    double u, v1, v2;

    int n_slices, n_stacks;
    double radius;
    int i, k;

    n_slices = 12;
    n_stacks = 6;
    radius = 1000;

    glPushMatrix();

    glScaled(radius, radius, radius);

	glBegin(GL_TRIANGLE_STRIP);

    for (i = 0; i < n_stacks; ++i) {
        v1 = (double)i / n_stacks;
        v2 = (double)(i + 1) / n_stacks;
        phi1 = v1 * M_PI / 2.0;
        phi2 = v2 * M_PI / 2.0;
        for (k = 0; k <= n_slices; ++k) {
            u = (double)k / n_slices;
            theta = u * 2.0 * M_PI;
            x1 = cos(theta) * cos(phi1);
            y1 = sin(theta) * cos(phi1);
            z1 = sin(phi1);
            x2 = cos(theta) * cos(phi2);
            y2 = sin(theta) * cos(phi2);
            z2 = sin(phi2);
            glTexCoord2d(u, 1.0 - v1);
            glVertex3d(x1, y1, z1);
            glTexCoord2d(u, 1.0 - v2);
            glVertex3d(x2, y2, z2);
        }
    }
    glEnd();

    glPopMatrix();
}

void draw_folyoso(){

	glPushMatrix();

	GLfloat light_color[] = {1.000, 1.000, 0.878, 0};
	GLfloat material_ambient[] = {0.05, 0.05, 0.05, 1};
    GLfloat material_diffuse[] = {0.5, 0.5, 0.5, 1};
    GLfloat material_specular[] = {0.7, 0.7, 0.7, 1};

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);

    GLfloat material_shininess[] = {shine};
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_color);

	//1es alap
	glBindTexture(GL_TEXTURE_2D, texture_names[3]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,0.0,1.5); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,-30.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30.0,-30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30.0,0.0,1.5);  //jf

	glEnd();
	//1es bal fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,0.0,15); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,0.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,-30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,-30.0,15);  //jf

	glEnd();

	//1es jobb fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(15.0,0.0,0.0);
        glVertex3d(30.0,0.0,15); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,0.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,-30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,-30.0,15);  //jf

	glEnd();

	//1es hátsó fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(30.0,-30.0,15); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(30.0,-30.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(0.0,-30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(0.0,-30.0,15);  //jf

	glEnd();

	//1es felső fal
	glBindTexture(GL_TEXTURE_2D, texture_names[1]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);      //textúrázási koordináta beállítása
		glNormal3f(0.0,0.0,-1.0);  //normálvektor beállítása
        glVertex3d(0.0,0.0,15.0); //bf megad egy csúcsot

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(30.0,0.0,15.0); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(30.0,-30.0,15.0); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(0.0,-30.0,15);  //jf

	glEnd();

	//az alap második része
	glBindTexture(GL_TEXTURE_2D, texture_names[3]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,30.0,1.5); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,0.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30.0,0.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30.0,30.0,1.5);  //jf

	glEnd();

	//2es bal fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,30.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,30.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,0.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,0.0,15.0);  //jf

	glEnd();

	//2es jobb fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,30.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,30.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,0.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,0.0,15);  //jf

	glEnd();

	//2es felső fal
	glBindTexture(GL_TEXTURE_2D, texture_names[1]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(0.0,30.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(30.0,30.0,15.0); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(30.0,0.0,15.0); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(0.0,0.0,15);  //jf

	glEnd();

	//3as alap
	glBindTexture(GL_TEXTURE_2D, texture_names[3]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,60.0,1.5); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,30.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30.0,30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30.0,60.0,1.5);  //jf

	glEnd();

	//3as bal, fal felső része
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,60.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,60.0,11.0); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,30.0,11.0); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,30.0,15.0);  //jf

	glEnd();

	//3as bal, fal alsó része
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,60.0,5.5); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,60.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(0.0,30.0,5.5);  //jf

	glEnd();

	//3as jobb, fal felső része
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,60.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,60.0,11.0); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,30.0,11.0); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,30.0,15);  //jf

	glEnd();

	//3as jobb, fal alsó része
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,60.0,5.5); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,60.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(30.0,30.0,5.5);  //jf

	glEnd();

	//3as hátsó fal
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(30.0,60.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(30.0,60.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(0.0,60.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(0.0,60.0,15);  //jf

	glEnd();

	//3as felső fal
	glBindTexture(GL_TEXTURE_2D, texture_names[1]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(0.0,60.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(30.0,60.0,15.0); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(30.0,30.0,15.0); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,-1.0);
        glVertex3d(0.0,30.0,15);  //jf

	glEnd();


	//3as bal fal
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(14.75,60.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(14.75,60.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(14.75,30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(14.75,30.0,15.0);  //jf

	glEnd();

	//3as jobb fal
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(15.25,60.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(15.25,60.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(15.25,30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(15.25,30.0,15.0);  //jf

	glEnd();

	//3as fedő fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(14.75,30.0,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(14.75,30.0,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(15.25,30.0,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(15.25,30.0,15.0);  //jf

	glEnd();


	//3as ajtós 1-es külső fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(0.0,29.75,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(0.0,29.75,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(7.5,29.75,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(7.5,29.75,15);  //jf

	glEnd();

	//3as ajtós 1-es belső fal
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(0.0,30.25,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(0.0,30.25,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(7.5,30.25,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(7.5,30.25,15);  //jf

	glEnd();

	//3as ajtós 1-es takaró fal
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(7.5,29.75,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(7.5,29.75,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(7.5,30.25,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(1.0,0.0,0.0);
        glVertex3d(7.5,30.25,15);  //jf

	glEnd();

	//3as ajtós 3-as külső fal
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(22.5,29.75,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(22.5,29.75,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(30.0,29.75,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(30,29.75,15);  //jf

	glEnd();

	//3as ajtós 3-as belső fal
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(22.5,30.25,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(22.5,30.25,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(30.0,30.25,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(30,30.25,15);  //jf

	glEnd();

    //3as ajtós 3-as takaró fal
	glBindTexture(GL_TEXTURE_2D, texture_names[8]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(22.5,29.75,15.0); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(22.5,29.75,1.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(22.5,30.25,1.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(-1.0,0.0,0.0);
        glVertex3d(22.5,30.25,15);  //jf

	glEnd();


	//AJTÓ
	glBindTexture(GL_TEXTURE_2D, texture_names[9]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(7.5,29.85,ajtoZ); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(7.5,29.85,ajtoZ-13.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(22.50,29.85,ajtoZ-13.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,-1.0,0.0);
        glVertex3d(22.5,29.85,ajtoZ);  //jf

	glEnd();

	//AJTÓ másik oldala
	glBindTexture(GL_TEXTURE_2D, texture_names[9]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(7.5,30.0,ajtoZ); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(7.5,30.0,ajtoZ-13.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(22.50,30,ajtoZ-13.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(22.5,30,ajtoZ);  //jf

	glEnd();

	//asztal lap
	glBindTexture(GL_TEXTURE_2D, texture_names[11]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,60.0,4.5); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(0.0,57.0,4.5); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30.0,57.0,4.5); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(30,60,4.5);  //jf

	glEnd();

	//asztal éle
	glBindTexture(GL_TEXTURE_2D, texture_names[11]);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(0.0,57.0,4.5); //bf

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(0.0,57.0,4.0); //ba

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(30.0,57.0,4.0); //ja

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,1.0,0.0);
        glVertex3d(30.0,57.0,4.5);  //jf

	glEnd();

	glPopMatrix();
}
void kinyit(){

	if(ajtoZ < 30.0){
		ajtoZ = ajtoZ+0.2;
	}
}

void bezar(){

	if(ajtoZ > 15.0){
		ajtoZ = ajtoZ-0.2;
	}
}

void setAjto(){
		if(stop == 0){
			stop = 1;
		} else stop = 0;
}

void glEnable2D()
{
	 int vPort[4];

   glGetIntegerv(GL_VIEWPORT, vPort);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   glOrtho(vPort[0], vPort[0]+vPort[2], vPort[1], vPort[1]+vPort[3], -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glTranslatef(0.375, 0.375, 0.);

   glPushAttrib(GL_DEPTH_BUFFER_BIT);
   glDisable(GL_DEPTH_TEST);

   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
}

void glDisable2D()
{
   glPopAttrib();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
}

void displayHelp(){

    glClear(GL_COLOR_BUFFER_BIT);

	glEnable2D();
	glBindTexture(GL_TEXTURE_2D, texture_names[7]);
		glBegin(GL_QUADS);

			//bf
			glTexCoord2f(0.0,1.0);
			glVertex3f(0, 0, 0.0);
			//ba
			glTexCoord2f(0.0,0.0);
			glVertex3f(0, nh, 0);
			//ja
			glTexCoord2f(1.0,0.0);
			glVertex3f(nw, nh, 0);
			//jf
			glTexCoord2f(1.0,1.0);
			glVertex3f(nw, 0, 0);
		glEnd();

	glDisable2D();

	glutPostRedisplay();
    glutSwapBuffers();

}

void displayM()
{
    double elapsed_time;

	lightx = camera.position.x;
	lighty = camera.position.y;
	lightz = camera.position.z;

	GLfloat light_color[] = {1, 1, 1, 0};
	GLfloat light_position[] = {lightx, lighty, lightz, lightw};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_color);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_names[6]);
	glRotatef(90, 0.0, 0.0, 1.0);
	glTranslatef(0.0,0.0,-10.0);
	draw_skybox();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(6.5,58.7,205.5);
		glRotatef(90, 1.0, 0, 0);
		glBindTexture(GL_TEXTURE_2D, texture_names[10]);
		draw_model(&model);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(23.6,58.7,205.5);
		glRotatef(90, 1.0, 0, 0);
		glBindTexture(GL_TEXTURE_2D, texture_names[10]);
		draw_model(&model2);
	glPopMatrix();

	glPushMatrix();

		glBindTexture(GL_TEXTURE_2D, texture_names[5]);
		glBegin(GL_QUADS);

        glTexCoord2f(0.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(-1000.0,1000.0,0.1);

        glTexCoord2f(0.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(-1000.0,-1000.0,0.1);

        glTexCoord2f(1.0, 1.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(1000.0,-1000.0,0.1);

        glTexCoord2f(1.0,0.0);
		glNormal3f(0.0,0.0,1.0);
        glVertex3d(1000.0,1000.0,0.1);

    glEnd();
	glPopMatrix();

	glTranslatef(0.0,0.0,200.0); //a szoba elhelyezése
	draw_folyoso();

    elapsed_time = calc_elapsed_time();
    update_camera_position(&camera, elapsed_time);
	set_view_point(&camera);

	if(stop==0){
		kinyit();
	}
	if(stop==1){
		bezar();
	}

	glutSwapBuffers();
}

void display(){
	if(help == 0)
		displayM();
	else
		displayHelp();
}

void reshape(GLsizei width, GLsizei height)
{
    int x, y, w, h;
    double ratio;

	nw = width;
	nh = height;

    ratio = (double)width / height;
    if (ratio > VIEWPORT_RATIO) {
        w = (int)((double)height * VIEWPORT_RATIO);
        h = height;
        x = (width - w) / 2;
        y = 0;
    }
    else {
        w = width;
        h = (int)((double)width / VIEWPORT_RATIO);
        x = 0;
        y = (height - h) / 2;
    }

    glViewport (x, y, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)width / (GLdouble)height, 0.01, 10000.0);
	}

void mouse_handler(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void setHelper(){
	if(help == 0){
		help = 1;
	}
	else{
		help = 0;
	}
}

void HelpKey(int key, int x, int y){

	switch(key){
		case GLUT_KEY_F1:
			setHelper();
			break;
	}
}

void motion_handler(int x, int y)
{
	double horizontal, vertical;

	horizontal = mouse_x - x;
	vertical = mouse_y - y;

	rotate_camera(&camera, horizontal, vertical);

	mouse_x = x;
	mouse_y = y;

    glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
        action.move_forward = TRUE;
		break;
	case 's':
        action.move_backward = TRUE;
		break;
	case 'a':
        action.step_left = TRUE;
		break;
	case 'd':
        action.step_right = TRUE;
		break;
	case '-':
		shine += 1;
		break;
	case '+':
		if(shine>=0)
			shine -= 1;
		else break;
		break;
	case 'n':
		setAjto();
		break;
    case 27:
        exit(0);
        break;
	}

	glutPostRedisplay();
}

void key_up_handler(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
        action.move_forward = FALSE;
		break;
	case 's':
        action.move_backward = FALSE;
		break;
	case 'a':
        action.step_left = FALSE;
		break;
	case 'd':
        action.step_right = FALSE;
		break;
	}

	glutPostRedisplay();
}

void idle()
{
    glutPostRedisplay();
}

void set_lightings()
{
    GLfloat light_position[] = { lightx, lighty, lightz, lightw };
    GLfloat light_ambient[] = { 0, 0, 0, 1 };
    GLfloat light_diffuse[] = { 1.000, 1.000, 0.878, 1 };
    GLfloat light_specular[] = { 1, 1, 1, 1 };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void initialize()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

	set_lightings();

    glClearDepth(1.0);

	glMatrixMode(GL_PROJECTION);
    glEnable(GL_TEXTURE_2D);


	initialize_texture();
}

int main(int argc, char* argv[])
{
	load_model("tiger_model.obj", &model);
	scale_model(&model, 0.001, 0.001, 0.001);
    print_model_info(&model);

    load_model("tiger_model.obj", &model2);
	scale_model(&model2, 0.001, 0.001, 0.001);
    print_model_info(&model2);

    glutInit(&argc, argv);

	glutInitWindowSize(1000, 800);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	int window = glutCreateWindow("Room");
	glutSetWindow(window);

    initialize();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_handler);
    glutKeyboardUpFunc(key_up_handler);
    glutMouseFunc(mouse_handler);
    glutMotionFunc(motion_handler);
	glutSpecialFunc(HelpKey);
    glutIdleFunc(idle);

    init_camera(&camera);

    action.move_forward = FALSE;
    action.move_backward = FALSE;
    action.step_left = FALSE;
    action.step_right = FALSE;

    glutMainLoop();

    return 0;
}

