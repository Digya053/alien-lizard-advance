/*CS 445/545 Prog 4 for Digya Acharya*/
/***********************************************************************************************
alien_lizard_advance.cpp
EXTRA_CREDIT: All four extra credit features have been implemented, i.e, placing two food pieces randomly
in the channel, determining their number and position at random for each spawned lizard (uses globals random_x
and random_y for positioning food at random places), growing lizard by 10 units in length each time it ingests a
food piece (globals food_x, food_y are used for making foods disapear after lizard reaches it). The lizard 
grows 25 units in length on entering a new channel. Blaster needs two hit to destroy the lizard (uses global 
hit_step for determining the number of times lizard has been hit). Score increases by an extra 50 if the lizard 
is in the top channel or the next-to-top channel.

Software Architecture Statement: This program utilizes three callback functions for achieving animation: 
display_func(), the display callback handler,  timer_func(), the timer callback handler and keyboard_func(), 
the glut keyboard callback handler. The display function draws all the objects and text in the scene, and
conditionally draws laser beam, reposition the laser (using globals x_pos_laser and y_pos_laser) in response 
to the keyboard events. The timer event is added as soon as the first frame is displayed, and helps in 
achieving motion in lizard (uses globals move_lizard_x and move_lizard_y for lizard motion).

Other major global variables: lizard_size helps in changing the lizard size on ingesting food and entering 
new channel, x_pos_human is used to determine the end of game, channel_count to determine if it's a clockwise
channel or not, isAnimate and fire_signal are the flags for animation and if laser beam is fired respectively, 
y_laser_hit helps in drawing laser beam only upto the position of lizard if it is hit by the laser, and upto
the end of the screen otherwise, x_tip_head and x_tip_tail is used for determining end of channel and score
is used for keeping track of score.
************************************************************************************************/

#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h> 
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

static float z;				//position all objects in z = -150 plane
static float lizard_size;	//Used for resizing lizard

static float move_lizard_x;	//Keeps track of lizard's current x-position
static float move_lizard_y; //Keeps track of lizard's current y-position

static float x_pos_human;   //Keeps track of human's x-position
static float x_pos_laser;	//Keeps track of current x-position of laser
static float y_pos_laser;   //Keeps track of current y-position of laser

static int channel_count;	//Keeps track of the channel number

static int isAnimate;		//For animating and pausing movement
static int fire_signal;		//Determine if the laser beam has been fired
static int end_game;

static int hit_step;		//Count the number of times lizard has been hit
static float y_laser_hit;	//The y-coordinate upto which the laser beam should reach

static float random_x;		//Random x-coordinate at which foods should be placed
static float random_y;		//Random y-coordinate at which foods should be placed

float x_tip_head;			//Coordinate of the tip of the lizard head
float x_tip_tail;			//Coordinate of the tip of the lizard tail

static bool first_food = true;	//Flag to keep track of whether the food should be drawn or already ingested
static bool second_food = true;

static float matShine[] = { 100.0 };	//Shininess value used throughout the program

static int score;			//Flag to keep track of the score
char canvas_Name[] = "Alien Lizard Advance"; // Name at the top of canvas
char game_over[] = "GAME OVER"; // Name at the top of canvas


// Sets width and height of canvas to 600 by 60s0.
#define canvas_Width 600
#define canvas_Height 600

//Function prototypes
void timer_func(int val);
void keyboard_func(unsigned char key, int x, int y);
void calculate_random_coordinates();

void init(void) {
	/* This function sets the background color, initializes all the global variables and calculate random 
	coordinates to place the food.*/
	glClearColor(0.0, 0.0, 0.0, 1.0);

	z = -150.0;
	lizard_size = 15;	//Initial lizard size is set to 15

	move_lizard_x = -288.0;
	move_lizard_y = 255; //(300-75/2-15/2). Also subtracted with 15/2 to place it in center because triangle is
						// drawn from the bottom in the program

	x_pos_human = -250.0;
	x_pos_laser = 0;
	y_pos_laser = -290;

	channel_count = 0;

	isAnimate = 1;	// Animation is enabled at the start
	fire_signal = 0;
	end_game = 0;

	hit_step = 0;
	y_laser_hit = 0;

	score = 0;
	random_x = 0;
	random_y = 0;
	calculate_random_coordinates();	
}

void setup_light_source(void) {
	/* This function turns on OpenGL lighting and keeps light at the world origin.*/
	
	// Set ight property vectors
	float lightAmb[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightDif[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightSpec[] = { 1.0, 1.0, 1.0, 1.0 };

	// Set light position at the world origin
	float lightPos[] = { 0.0, 0.0, 0.0, 1.0 }; 

	// Properties of GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//Enable lighting and light source GL_LIGHT0
	glEnable(GL_LIGHTING); 
	glEnable(GL_LIGHT0);
}

void draw_human_head(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glutSolidSphere(7, 10, 10);
	glPopMatrix();
}

void draw_human_torso(float x, float  y, float z) {
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y - 15, z);
	glEnd();
}

void draw_human_hands(float x, float y, float z) {
	glBegin(GL_LINE_STRIP);
		glVertex3f(x - 5, y - 5, z);
		glVertex3f(x, y, z);
		glVertex3f(x + 5, y - 5, z);
	glEnd();
}

void draw_human_limbs(float x, float y, float z) {
	glBegin(GL_LINE_STRIP);
		glVertex3f(x - 5, y - 8, z);
		glVertex3f(x, y, z);
		glVertex3f(x + 5, y - 8, z);
	glEnd();
}

void draw_human(float x, float y, float z) {
	// Material properties of the human.
	float matAmb[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float matDif[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float matSpec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	draw_human_head(x, y, z);
	draw_human_torso(x, y - 7, z);
	draw_human_hands(x, y - 9, z);
	draw_human_limbs(x, y - 22, z);
}

void draw_body_odd_square(float x, float y, float z) {
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(x + lizard_size, y, z);
		glVertex3f(x, y, z);
		glVertex3f(x + lizard_size, y + 15, z);
		glVertex3f(x, y + 15, z);
	glEnd();
}

void draw_body_even_square(float x, float y, float z) {
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(x, y, z);
		glVertex3f(x, y + 15, z);
		glVertex3f(x+lizard_size, y, z);
		glVertex3f(x + lizard_size, y + 15, z);
	glEnd();
}

void draw_lizard_body(float x, float y, float z) {
	// Material properties of the lizard body.
	float matAmb[] = { 0.0f, 0.5f, 0.0f, 1.0f };
	float matDif[] = { 0.0f, 0.5f, 0.0f, 1.0f };
	float matSpec[] = {0.0f, 0.5f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	draw_body_odd_square(x, y, z);
	draw_body_even_square(x + lizard_size, y, z);
	draw_body_odd_square(x + 2 * lizard_size, y, z);
	draw_body_even_square(x + 3 * lizard_size, y, z);
	draw_body_odd_square(x + 4 * lizard_size, y, z);
}

void draw_lizard_head(float x, float y, float z) {
	// Material properties of the lizard head.
	float matAmb[] = { 0.6f, 0.6f, 0.0f, 1.0f };
	float matDif[] = { 0.6f, 0.6f, 0.0f, 1.0f };
	float matSpec[] = { 0.6f, 0.6f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	x_tip_head = x - 12;
	if (x_tip_head < -288) {
		x_tip_head = -288;
	}
	glBegin(GL_TRIANGLES);
	glVertex3f(x, y, z);
	glVertex3f(x, y + 15, z);
	glVertex3f(x - 12, y + 15/2, z);
	glEnd();
}

void draw_lizard_tail(float x, float y, float z) {
	float matAmb[] = { 0.6f, 0.6f, 0.0f, 1.0f };
	float matDif[] = { 0.6f, 0.6f, 0.0f, 1.0f };
	float matSpec[] = { 0.6f, 0.6f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	x_tip_tail = x + 12;
	glBegin(GL_TRIANGLES);
		glVertex3f(x, y, z);
		glVertex3f(x, y  + 15 , z);
		glVertex3f(x + 12, y + 15/2, z);
	glEnd();
}


void draw_lizard_limbs(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(45, 0, 0, -1);
	glTranslatef(-x, -y, -z);
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y - 6, z);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(45, 0, 0, 1);
	glTranslatef(-x, -y, -z);
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y - 6, z);
	glEnd();
	glPopMatrix();
}

void draw_all_limbs(float x, float y, float z) {
	// Material properties of the lizard limbs.
	float matAmb[] = { 0.0f, 0.4f, 0.0f, 1.0f };
	float matDif[] = { 0.0f, 0.4f, 0.0f, 1.0f };
	float matSpec[] = { 0.0f, 0.4f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	draw_lizard_limbs(x + lizard_size/2, y, z);
	draw_lizard_limbs(x + (5*lizard_size) - (lizard_size/2), y, z);
	
}

void draw_lizard(float x, float y, float z) {
	glPushMatrix();
	draw_lizard_body(x, y, z);
	draw_lizard_head(x, y, z);
	draw_lizard_tail(x + lizard_size * 5, y, z);
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
	float matAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float matDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float matSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(90, 1, 0, 0);
	gluCylinder(qobj, 2.5, 2.5, 10, 20, 20);
	glPopMatrix();
}

void handle_lizard_hit() {
	if (x_pos_laser <= move_lizard_x + lizard_size * 5 + 24 and x_pos_laser >= move_lizard_x - 12) {
		hit_step += 1;
		y_laser_hit = move_lizard_y;
		if (hit_step == 1) {
			glutTimerFunc(50, timer_func, 3);
			fire_signal = 0;
		}
		else if (hit_step >= 2) {
			glutTimerFunc(1000, timer_func, 4);
		}
	}
	else {
		y_laser_hit = 300;
	}
}

void draw_laser_beam() {
	float matAmb[] = { 1.0f, 0.99f, 0.81f, 1.0f };
	float matDif[] = { 1.0f, 0.99f, 0.81f, 1.0f };
	float matSpec[] = { 1.0f, 0.99f, 0.81f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	handle_lizard_hit();
	glLineWidth(2);
	glBegin(GL_LINES);
		glVertex3f(x_pos_laser, y_pos_laser, z);	
		glVertex3f(x_pos_laser, y_laser_hit, z);
	glEnd();
}

void calculate_random_coordinates() {
	int max = 10;
	srand((unsigned int)time(NULL));
	random_x = (float)(rand() % max) * 25;
	if (random_x == x_pos_laser or random_x == x_pos_human) {
		random_x = random_x + 50;
	}
	const float array_num[3] = { 180, 105, 30 };
	int rand_index = rand() % 3;
	random_y = (float)array_num[rand_index];
}

void handle_lizard_ingest_clockwise_move() {
	if (move_lizard_y == random_y and (move_lizard_x + ((lizard_size * 5) + 12) >= random_x or x_tip_tail >= random_x-23)) {
		if (first_food) {
			lizard_size += 2;
		}
		first_food = false;
	}
	if (move_lizard_y == -random_y-15 and (move_lizard_x + ((lizard_size * 5) + 12) >= -random_x or x_tip_tail >= -random_x+23)) {
		if (second_food) {
			lizard_size += 2;
		}
		second_food = false;
	}

}

void handle_lizard_ingest_anticlockwise_move() {
	if (move_lizard_y == random_y and (move_lizard_x + ((lizard_size * 5) + 12) <= random_x or x_tip_head <= random_x-23)) {
		if (first_food) {
			lizard_size += 2;
		}
		first_food = false;
	}
	if (move_lizard_y == -random_y-15 and (move_lizard_x + ((lizard_size * 5) + 12) <= -random_x or x_tip_head <= -random_x+23)) {
		if (second_food) {
			lizard_size += 2;
		}
		second_food = false;
	}
}

void display_food(float x, float y, float z) {
	float matAmb[] = { 0.5f, 0.35f, 0.05f, 1.0f };
	float matDif[] = { 0.5f, 0.35f, 0.05f, 1.0f };
	float matSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	glPushMatrix();
	glTranslatef(x, y, z);
	glutSolidTorus(8, 23, 30, 30);
	glPopMatrix();
	if (channel_count % 2 == 0) {
		handle_lizard_ingest_clockwise_move();
	}
	else {
		handle_lizard_ingest_anticlockwise_move();
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
	float matAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float matDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float matSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	char scoreText[20] = "SCORE: ";
	char scoreNum[6];
	sprintf_s(scoreNum, 6, "%d", score);
	strcat_s(scoreText, 20, scoreNum);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, scoreText);
	glPopMatrix();
}

void display_game_over(float x, float y, float z) {
	float matAmb[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float matDif[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float matSpec[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, game_over);
	glPopMatrix();
}

void display_func(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	draw_human(x_pos_human, -270.0, z);	
	draw_laser(x_pos_laser, y_pos_laser, z);
	if (fire_signal) {
		draw_laser_beam();
		glutTimerFunc(70, timer_func, 2);
	}
	draw_lizard(move_lizard_x, move_lizard_y, z);
	display_score(-20, 285, z);
	if (first_food) {
		display_food(random_x, random_y, z);
	}
	if (second_food) {
		display_food(-random_x, -random_y-15, z);
	}	
	if (end_game) {
		glutKeyboardFunc(NULL);
		display_game_over(-50, 0, -150);
	} 
	glutSwapBuffers();
	glFlush();

}

void move_lizard(void) {
	if (channel_count % 2 == 0) {
		move_lizard_x += 4.5;
	}
	else {
		move_lizard_x -= 4.5;
	}
	if (move_lizard_y < -270) {	
		move_lizard_y = -270;
	}
}

void calculate_anticlockwise_start_position() {
	move_lizard_x = 300 - lizard_size * 5 - 24;
	move_lizard_y -= 75;
	channel_count += 1;
	lizard_size += 5;
}

void calculate_clockwise_start_position() {
	move_lizard_x = -300;
	move_lizard_y -= 75;
	channel_count += 1;
	lizard_size += 5;
}

void check_end_position() {
	if (move_lizard_x <= x_pos_human and move_lizard_y <= -270) {
		isAnimate = 0;
		end_game = 1;
		glutTimerFunc(2000, timer_func, 5);

	}
}

void set_lizard_spawn_values() {
	move_lizard_x = -288.0;
	move_lizard_y = 255;
	first_food = true;
	second_food = true;
	lizard_size = 15;
	hit_step = 0;
	fire_signal = 0;
	calculate_random_coordinates();
	if (channel_count == 0 or channel_count == 1) {
		score += 50;
	}
	channel_count = 0;
	score += 50;
}

void timer_func(int val) {
	switch (val) {
	case 1:
		if ((channel_count % 2 == 0) and x_tip_tail >= 288) {
			calculate_anticlockwise_start_position();
		}
		if ((channel_count % 2 != 0) and (x_tip_head <= -288)) {
			calculate_clockwise_start_position();
		}
		glutPostRedisplay();
		move_lizard();
		check_end_position();
		if (isAnimate) {
			glutTimerFunc(50, timer_func, 1);
		}
		break;
	case 2:
		fire_signal = 0;
		glutPostRedisplay();
		break;
	case 3:
		lizard_size = lizard_size / 2;
		break;
	case 4:
		set_lizard_spawn_values();
		glutKeyboardFunc(keyboard_func);
		glutPostRedisplay();
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
		x_pos_laser -= 5;
		glutPostRedisplay();
		break;
	case 'K': case'k':
		x_pos_laser += 5;
		glutPostRedisplay();
		break;
	case ' ':
		glutKeyboardFunc(NULL);
		fire_signal = 1;
		glutKeyboardFunc(keyboard_func);
		break;
}
}

int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	glutDisplayFunc(display_func);
	setup_light_source();
	init();
	glutTimerFunc(50, timer_func, 1);
	glutKeyboardFunc(keyboard_func);
	glutMainLoop();
	return 0;
}
