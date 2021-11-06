#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

static float z;
static float y_inc;
static GLUquadricObj *qobj; // Create a pointer to a quadric object.
static float matShine[] = { 50.0 };

char canvas_Name[] = "Alien Lizard Advance"; // Name at the top of canvas

// Sets width and height of canvas to 600 by 60s0.
#define canvas_Width 600
#define canvas_Height 600

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	z = -150.0;
	y_inc = 0;
	// Create the new quadric object.

	qobj = gluNewQuadric();



	// Specify that quadrics are drawn in wireframe.

	gluQuadricDrawStyle(qobj, GLU_LINE);

}

void setup_light_source(void) {
	// Turn on OpenGL lighting.
	glEnable(GL_LIGHTING);
	// Light property vectors.
	float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightPos[] = { 0.0, 0.0, -150.0, 1.0 };
	float globAmb[] = { 1.0, 1.0, 1.0, 1.0 };

	// Light properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0); // Enable particular light source.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.
}

void draw_head(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glutSolidSphere(7, 10, 10);
	glPopMatrix();
}

void draw_torso(float x, float  y, float z) {
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y - 15, z);
	glEnd();
}

void draw_hands(float x, float y, float z) {
	glBegin(GL_LINE_STRIP);
		glVertex3f(x - 5, y - 5, z);
		glVertex3f(x, y, z);
		glVertex3f(x + 5, y - 5, z);
	glEnd();
}

void draw_limbs(float x, float y, float z) {
	glBegin(GL_LINE_STRIP);
		glVertex3f(x - 5, y - 8, z);
		glVertex3f(x, y, z);
		glVertex3f(x + 5, y - 8, z);
	glEnd();
}

void draw_human(float x, float y, float z) {
	// Material properties of the human.
	float matAmb[] = { 1.0, 0.0, 0.0, 1.0 };
	float matDif[] = { 1.0, 0.0, 0.0, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);

	draw_head(x, y, z);
	draw_torso(x, y - 7, z);
	draw_hands(x, y - 9, z);
	draw_limbs(x, y - 22, z);
}

void draw_body_odd_square(float x, float y, float z) {
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(x, y + y_inc, z);
		glVertex3f(x, y - 15, z);
		glVertex3f(x + 15, y + y_inc, z);
		glVertex3f(x + 15, y - 15, z);
	glEnd();
}

void draw_body_even_square(float x, float y, float z) {
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(x, y - 15, z);
		glVertex3f(x, y, z);
		glVertex3f(x + 15, y - 15, z);
		glVertex3f(x + 15, y, z);
	glEnd();
}

void draw_lizard_body(float x, float y, float z) {
	// Material properties of the lizard body.
	float matAmb[] = { 0.0, 1.0, 0.0, 1.0 };
	float matDif[] = { 0.0, 1.0, 0.0, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	draw_body_odd_square(x, y, z);
	draw_body_even_square(x + 15, y, z);
	draw_body_odd_square(x + 30, y, z);
	draw_body_even_square(x + 45, y, z);
	draw_body_odd_square(x + 60, y, z);
}

void draw_lizard_head(float x, float y, float z) {
	// Material properties of the lizard head.
	float matAmb[] = { 1.0, 1.0, 0.0, 1.0 };
	float matDif[] = { 1.0, 1.0, 0.0, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	glBegin(GL_TRIANGLES);
	glVertex3f(x, y + y_inc, z);
	glVertex3f(x, y - 15, z);
	glVertex3f(x - 12, y - 8, z);
	glEnd();
}

void draw_lizard_tail(float x, float y, float z) {
	glBegin(GL_TRIANGLES);
	glVertex3f(x, y + y_inc, z);
	glVertex3f(x, y - 15, z);
	glVertex3f(x + 12, y - 8, z);
	glEnd();
}


void draw_lizard_limbs(float x, float y, float z) {
	glBegin(GL_LINE_STRIP);
	glVertex3f(x - 6, y - 6, z);
	glVertex3f(x, y, z);
	glVertex3f(x + 6, y - 6, z);
	glEnd();
}

void draw_all_limbs(float x, float y, float z) {
	// Material properties of the lizard limbs.
	float matAmb[] = { 0.0, 0.5, 0.0, 1.0 };
	float matDif[] = { 0.0, 0.5, 0.0, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	draw_lizard_limbs(x + 8, y, z);
	draw_lizard_limbs(x + 67, y, z);
	
}

void draw_lizard(float x, float y, float z) {
	draw_lizard_body(x, y, z);
	draw_lizard_head(x, y, z);
	draw_lizard_tail(x + 75, y, z);
	draw_all_limbs(x, y - 15, z);
}


void draw_laser(float x, float y, float z) {
	// Material properties of the laser.
	float matAmb[] = { 1.0, 1.0, 1.0, 1.0 };
	float matDif[] = { 1.0, 1.0, 1.0, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(90, 1, 0, 0);
	gluCylinder(qobj, 2.5, 2.5, 10, 20, 20);
	glPopMatrix();
}
void display_func(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	draw_human(-250.0, -270.0, z);
	draw_lizard(-250.0, 270.0, z);
	draw_laser(0, -290, z);
	glutSwapBuffers();
	glFlush();

}

int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	glutDisplayFunc(display_func);
	init();
	setup_light_source();
	glutMainLoop();
	return 0;
}
