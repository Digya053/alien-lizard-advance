#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

static float z;
static float y_inc;
static float move_lizard_x;
static float move_lizard_y;
static float final_lizard_x;
static int channel_count; 
static float x_pos_human;
static float x_pos_laser;
static float y_pos_laser;
static int isAnimate;
static int fire_signal;
static float y;
static int hit_step;

static float random_x;
static float random_y;

float x_tip_head;
float x_tip_tail;

static bool first_food = true;
static bool second_food = true;

static float matShine[] = { 50.0 };

static int score;
char canvas_Name[] = "Alien Lizard Advance"; // Name at the top of canvas


// Sets width and height of canvas to 600 by 60s0.
#define canvas_Width 600
#define canvas_Height 600

void timer_func(int val);
void keyboard_func(unsigned char key, int x, int y);
void calculate_random_coordinates();

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	z = -150.0;
	y_inc = 15;
	move_lizard_y = 225.0; //300-75
	final_lizard_x = 0;
	channel_count = 0;
	move_lizard_x = -288.0;
	x_pos_human = -250.0;
	x_pos_laser = 0;
	y_pos_laser = -290;
	isAnimate = 1;
	fire_signal = 0;
	y = 0;
	score = 0;
	random_x = 0;
	random_y = 0;
	calculate_random_coordinates();
	hit_step = 0;
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
		glVertex3f(x + y_inc, y, z);
		glVertex3f(x , y, z);
		glVertex3f(x + y_inc, y + y_inc, z);
		glVertex3f(x, y + y_inc, z);
	glEnd();
}

void draw_body_even_square(float x, float y, float z) {
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(x, y, z);
		glVertex3f(x, y + y_inc, z);
		glVertex3f(x + y_inc, y, z);
		glVertex3f(x + y_inc, y + y_inc, z);
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
	draw_body_even_square(x + y_inc, y, z);
	draw_body_odd_square(x + 2 * y_inc, y, z);
	draw_body_even_square(x + 3 * y_inc, y, z);
	draw_body_odd_square(x + 4 * y_inc, y, z);
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
	float y_tip = y_inc / 2;
	x_tip_head = x - 12;
	glBegin(GL_TRIANGLES);
	glVertex3f(x, y, z);
	glVertex3f(x, y + y_inc, z);
	glVertex3f(x - 12, y + y_tip, z);
	glEnd();
}

void draw_lizard_tail(float x, float y, float z) {
	float matAmb[] = { 1.0, 0.0, 0.0, 1.0 };
	float matDif[] = { 1.0, 0.0, 0.0, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	float y_tip = y_inc / 2;
	x_tip_tail = x + 12;
	glBegin(GL_TRIANGLES);
	glVertex3f(x, y, z);
	glVertex3f(x, y  + y_inc , z);
	glVertex3f(x + 12, y + y_tip, z);
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
	draw_lizard_limbs(x + y_inc/2, y, z);
	draw_lizard_limbs(x + (5*y_inc) - (y_inc/2), y, z);
	
}

void draw_lizard(float x, float y, float z) {
	glPushMatrix();
	draw_lizard_body(x, y, z);
	draw_lizard_head(x, y, z);
	float x_tail = y_inc * 5;
	draw_lizard_tail(x + x_tail, y, z);
	draw_all_limbs(x, y, z);
	glPopMatrix();
}


void draw_laser(float x, float y, float z) {
	GLUquadricObj *qobj; // Create a pointer to a quadric object.
	// Create the new quadric object.
	qobj = gluNewQuadric();

	// Specify that quadrics are drawn in wireframe.
	gluQuadricDrawStyle(qobj, GLU_LINE);
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

void draw_laser_beam() {
	float matAmb[] = { 1.0, 0.99, 0.81, 1.0 };
	float matDif[] = { 1.0, 0.99, 0.81, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	float lizard_size = (y_inc * 5) + 12;
	if (x_pos_laser <= move_lizard_x + lizard_size + 12 and x_pos_laser >= move_lizard_x-12) {
		glutKeyboardFunc(NULL);
		y = move_lizard_y - y_inc;
		if (hit_step == 1) {
			glutTimerFunc(1000, timer_func, 3);
		}
		else if (hit_step >= 2){
			glutTimerFunc(1000, timer_func, 4);
		}
	}
	else {
		y = 300;
	}
	glBegin(GL_LINES);
		glVertex3f(x_pos_laser-2, y_pos_laser, z);	
		glVertex3f(x_pos_laser-2, y, z);
		glVertex3f(x_pos_laser+2, y_pos_laser, z);
		glVertex3f(x_pos_laser+2, y, z);
	glEnd();
}

void increase_lizard_size() {
	y_inc += 10;
	//glutPostRedisplay();
}

void calculate_random_coordinates() {
	int max = 10;
	srand(time(NULL));
	random_x = (rand() % max) * 25;
	const int array_num[4] = {75, 150};
	int rand_index = rand() % 4;
	random_y = array_num[rand_index];
}
void display_food(float x, float y, float z) {
	float matAmb[] = { 0.5, 0.35, 0.05, 1.0 };
	float matDif[] = { 0.5, 0.35, 0.05, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	glPushMatrix();
	glTranslatef(x, y, z);
	glutSolidTorus(8, 23, 30, 30);
	glPopMatrix();
	if (channel_count % 2 == 0) {
		if (move_lizard_y == random_y and (move_lizard_x + ((y_inc * 5) + 12) >= random_x || x_tip_tail >= random_x)) {
			if (first_food) {
				increase_lizard_size();
			}
			first_food = false;
		}
		if (move_lizard_y == -random_y and (move_lizard_x + ((y_inc * 5) + 12) >= -random_x || x_tip_tail >= -random_x)) {
			if (second_food) {
				increase_lizard_size();
			}
			second_food = false;
		}

	}
	else {
		if (move_lizard_y == random_y and (move_lizard_x + ((y_inc * 5) + 12) <= random_x || x_tip_head <= random_x)) {
			if (first_food) {
				increase_lizard_size();
			}
			first_food = false;
		}
		if (move_lizard_y == -random_y and (move_lizard_x + ((y_inc * 5) + 12) <= -random_x || x_tip_head <= -random_x)) {
			if (second_food) {
				increase_lizard_size();
			}
			second_food = false;
		}
	}
	
	
}

void writeBitmapString(void *font, char *string) {
	/*
	This function writes a bitmap text, one character at a time.
	Parameters:
	----------
		font: void pointer
			The font to use to write a text.
		string: char pointer
			The text to write.
	*/
	char *c;
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void display_score(float x, float y, float z) {
	glPushMatrix();
	glRasterPos3f(x, y, z);
	char scoreText[20] = "SCORE: ";
	char scoreNum[6];
	sprintf_s(scoreNum, 6, "%d", score);
	strcat_s(scoreText, 20, scoreNum);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, scoreText);
	glPopMatrix();
}

void display_func(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	draw_human(x_pos_human, -270.0, z);	
	draw_lizard(move_lizard_x, move_lizard_y, z);
	draw_laser(x_pos_laser, y_pos_laser, z);
	if (fire_signal) {
		draw_laser_beam();
		glutTimerFunc(80, timer_func, 2);
	}
	display_score(-20, 280, z);
	if (first_food) {
		display_food(random_x, random_y, z);
	}
	if (second_food) {
		display_food(-random_x, -random_y, z);
	}
		
	glutSwapBuffers();
	glFlush();

}

void move_lizard(void) {
	if (channel_count % 2 == 0) {
		move_lizard_x += 20;
	}
	else {
		move_lizard_x -= 20;
	}
	if (move_lizard_y < -279) {
		move_lizard_y = -279;
	}
}

void timer_func(int val) {
	switch (val) {
	case 1:

		if ((channel_count % 2 == 0) and x_tip_tail >= 279) {
			move_lizard_x = 300 - (y_inc * 5) - ((12 * y_inc) / 2)-20;
			move_lizard_y -= 75;
			channel_count += 1;
			y_inc += 25;
		}
		if ((channel_count % 2 != 0) and (x_tip_head <= -279)) {
			move_lizard_x = -300;
			move_lizard_y -= 75;
			channel_count += 1;
			y_inc += 25;
		}
		glutPostRedisplay();
		move_lizard();
		if (move_lizard_x <= x_pos_human && move_lizard_y <= -279) {
			isAnimate = 0;
			glutTimerFunc(2000, timer_func, 5);
		}
		if (isAnimate) {
			glutTimerFunc(1000, timer_func, 1);
		}
		break;
	case 2:
		fire_signal = 0;
		glutPostRedisplay();
		break;
	case 3:
		//glutKeyboardFunc(NULL);
		y_inc = y_inc / 2;
		//hit_step += 1;
		glutPostRedisplay();
		glutKeyboardFunc(keyboard_func);
		break;
	case 4:
		//glutKeyboardFunc(NULL);
		move_lizard_x = -288.0;
		move_lizard_y = 225.0;
		first_food = true;
		second_food = true;
		y_inc = 15;
		hit_step = 0;
		calculate_random_coordinates();
		if (channel_count == 0 or channel_count == 1) {
			score += 50;
		}
		channel_count = 0;
		score += 50;
		glutPostRedisplay();
		glutKeyboardFunc(keyboard_func);
		break;
	case 5:
		exit(0);
		break;
	}
}

void keyboard_func(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'J': case 'j':
		glutKeyboardFunc(NULL);
		x_pos_laser -= 5;
		glutPostRedisplay();
		glutKeyboardFunc(keyboard_func);
		break;
	case 'K': case'k':
		glutKeyboardFunc(NULL);
		x_pos_laser += 5;
		glutPostRedisplay();
		glutKeyboardFunc(keyboard_func);
		break;
	case ' ':
		glutKeyboardFunc(NULL);
		fire_signal = 1;
		hit_step += 1;
		glutPostRedisplay();
		glutKeyboardFunc(keyboard_func);
		break;
}
}

int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	glutDisplayFunc(display_func);
	init();
	setup_light_source();
	glutTimerFunc(1000, timer_func, 1);
	glutKeyboardFunc(keyboard_func);
	glutMainLoop();
	return 0;
}
