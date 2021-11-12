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
Apart from these, after the game ends, a "GAME_OVER" message has been added and the screen closes itself after 
2 secs.

Software Architecture Statement: This program utilizes three callback functions for achieving animation: 
display_func(), the display callback handler,  timer_func(), the timer callback handler and keyboard_func(), 
the glut keyboard callback handler. The display function draws all the objects and text in the scene, and
conditionally draws laser beam, reposition the laser (using globals x_pos_laser and y_pos_laser) in response 
to the keyboard events. The timer event is added as soon as the first frame is displayed, and helps in 
achieving motion in lizard (uses globals move_lizard_x and move_lizard_y for lizard motion).

Other major global variables: lizard_size helps in changing the lizard size on ingesting food and entering 
new channel, animation period is the interval between each lizard movement, x_pos_human is used to determine 
the end of game, channel_count to determine if it's a clockwise channel or not, isAnimate and fire_signal are
the flags for animation and if laser beam is fired respectively, y_laser_hit helps in drawing laser beam only
upto the position of lizard if it is hit by the laser, and upto the end of the screen otherwise, x_tip_head and
x_tip_tail is used for determining end of channel and score is used for keeping track of score.
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

static int animationPeriod;	//Time interval between each movement of a lizard

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


// Sets width and height of canvas to 600 by 600.
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

	animationPeriod = 50; // Set to change 20 frames in 1 sec (1000/20 = 50)

	hit_step = 0;
	y_laser_hit = 0;

	score = 0;
	random_x = 0;
	random_y = 0;
	calculate_random_coordinates();	
}

void setup_light_source(void) {
	/* This function turns on OpenGL lighting and keeps light at the world origin.*/
	
	// Set light property vectors
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
	/*
	This function draws a head of a stickman using glutSolidSphere.
	Parameters:
	----------
		x: float
			The x-position where solid sphere should be drawn
		y: float
			The y-position where solid sphere should be drawn
		z: float
			The z-plane where solid sphere should be drawn
	*/
	glPushMatrix();
	glTranslatef(x, y, z);
	glutSolidSphere(7, 10, 10);
	glPopMatrix();
}

void draw_human_torso(float x, float  y, float z) {
	/*
	This function draws torso of stick man.
	Parameters:
	----------
		x: float
			The x-position from where line should begin with
		y: float
			The y-position from where line should begin with
		z: float
			The z-plane where line should be drawn
	*/
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y - 15, z);
	glEnd();
}

void draw_human_hands(float x, float y, float z) {
	/*
	This function draws hands of a stick man.
	Parameters:
	----------
		x: float
			The x-position from where line strip for hands should begin with
		y: float
			The y-position from where line strip for hands should begin with
		z: float
			The z-plane where line strip for hands should be drawn
	*/
	glBegin(GL_LINE_STRIP);
		glVertex3f(x - 5, y - 5, z);
		glVertex3f(x, y, z);
		glVertex3f(x + 5, y - 5, z);
	glEnd();
}

void draw_human_limbs(float x, float y, float z) {
	/*
	This function draws limbs of a stick man.
	Parameters:
	----------
		x: float
			The x-position from where line strip for limbs should begin with
		y: float
			The y-position from where line strip for limbs should begin with
		z: float
			The z-plane where line strip for limbs should be drawn
	*/
	glBegin(GL_LINE_STRIP);
		glVertex3f(x - 5, y - 8, z);
		glVertex3f(x, y, z);
		glVertex3f(x + 5, y - 8, z);
	glEnd();
}

void draw_human(float x, float y, float z) {
	/*
	This function attaches the components for human and places human in proper position.
	Parameters:
	----------
		x: float
			The x-position where human should be placed
		y: float
			The y-position where human should be placed
		z: float
			The z-plane where human should be placed
	*/
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
	/*
	This function draws two triangles for the odd squares starting from the first square.
	Parameters:
	----------
		x: float
			The x-position of the left bottom part of odd squares.
		y: float
			The y-position of the left bottom part of odd squares.
		z: float
			The z-position of the left bottom part of odd squares.
	*/
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(x + lizard_size, y, z);
		glVertex3f(x, y, z);
		glVertex3f(x + lizard_size, y + 15, z);
		glVertex3f(x, y + 15, z);
	glEnd();
}

void draw_body_even_square(float x, float y, float z) {
	/*
	This function draws two triangles for the even squares starting from the second square.
	Parameters:
	----------
		x: float
			The x-position of the left bottom part of even squares.
		y: float
			The y-position of the left bottom part of even squares.
		z: float
			The z-position of the left bottom part of even squares.
	*/
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(x, y, z);
		glVertex3f(x, y + 15, z);
		glVertex3f(x + lizard_size, y, z);
		glVertex3f(x + lizard_size, y + 15, z);
	glEnd();
}

void draw_lizard_body(float x, float y, float z) {
	/*
	This function places the odd and even squares alternatively in proper position. x, y and z
	are the position for first triangular square. Position for other squares are calculated from
	x, y, z position.
	Parameters:
	----------
		x: float
			The x-position where first square should be drawn.
		y: float
			The y-position where first square should be drawn.
		z: float
			The z-position where first square should be drawn.
	*/
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
	/*
	This function draws the lizard head using GL_TRIANGLES and places the lizard head in proper position.
	Parameters:
	----------
		x: float
			The x-position where the head should be placed.
		y: float
			The y-position where the head should be placed.
		z: float
			The z-position where the head should be placed.
	*/
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
	/*
	This function draws the lizard tail using GL_TRIANGLES and places the lizard tail in proper position.
	Parameters:
	----------
		x: float
			The x-position where the tail should be placed.
		y: float
			The y-position where the tail should be placed.
		z: float
			The z-position where the tail should be placed.
	*/
	// Material properties of the lizard tail.
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
	/*
	This function draws the lizard limbs using GL_LINES, rotate the line as as to make the two lines
	placed at an angle of 45 degree. 
	Parameters:
	----------
		x: float
			The x-position where the limb should be placed.
		y: float
			The y-position where the limb should be placed.
		z: float
			The z-position where the limb should be placed.
	*/
	glPushMatrix();
	// Translate to origin
	glTranslatef(x, y, z);
	glRotatef(45, 0, 0, -1);
	// Translate back to original position
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
	/*
	This function draws both the lizard limbs and places them at correct position.
	----------
		x: float
			The x-position where the first limb should be placed.
		y: float
			The y-position where the first limb should be placed.
		z: float
			The z-position where the first limb should be placed.
	*/
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
	/*
	This function draws all the parts of the lizard and places them at correct position.
	----------
		x: float
			The x-position where the left bottom of the first triangular square should be placed. 
		y: float
			The y-position where the left bottom of the first triangular square should be placed.
		z: float
			The z-position where the left bottom of the first triangular square should be placed.
	*/
	glPushMatrix();
	draw_lizard_body(x, y, z);
	draw_lizard_head(x, y, z);
	draw_lizard_tail(x + lizard_size * 5, y, z);
	draw_all_limbs(x, y, z);
	glPopMatrix();
}


void draw_laser(float x, float y, float z) {
	/*
	This function draws the cylindrical laser using gluCylinder. Referred from Guha's book.
	----------
		x: float
			The x-position where the cylinder should be placed.
		y: float
			The y-position where the cylinder should be placed.
		z: float
			The z-position where the cylinder should be placed.
	*/
	// Create a pointer to a quadric object.
	GLUquadricObj *qobj; 
	// Initialize new quadric object
	qobj = gluNewQuadric();
	// Draw the quadrics in wireframe
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

void handle_lizard_hit(void) {
	/*
	This function changes the lizard size to half if it has been hit once, spaws a new lizard if it has been hit twice
	and set the y_position of the laser line to the y-position of the lizard if it has been hit.
	*/
	if (x_pos_laser <= move_lizard_x + lizard_size * 5 + 24 and x_pos_laser >= move_lizard_x - 12) {
		hit_step += 1;
		y_laser_hit = move_lizard_y;
		if (hit_step == 1) {
			fire_signal = 1;
			glutTimerFunc(animationPeriod, timer_func, 3);
			fire_signal = 0;
		}
		else if (hit_step >= 2) {
			glutKeyboardFunc(NULL);
			glutTimerFunc(1000, timer_func, 4);
		}
	}
	else {
		y_laser_hit = 300;
	}
}

void draw_laser_beam(void) {
	/*
	This function draws a laser beam of using GL_LINES of width 2. If the lizard is not hit it draws upto the end of the 
	canvas and if it's hit, it draws the beam only upto lizard's y-position.
	*/
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

void calculate_random_coordinates(void) {
	/*
	This function draws food at random places. Only the y-values 180, 105 and 30 are taken randomly as as not to overpaint score, human and laser. 
	For x-value, any random number divisible by 25 is taken.
	*/
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

void handle_lizard_ingest_clockwise_move(void) {
	/*
	This function determines if the lizard is passing over the food in the clockwise direction, set the food flags
	to false if the lizard has passed it and increases lizard's size.
	*/
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

void handle_lizard_ingest_anticlockwise_move(void) {
	/*
	This function determines if the lizard is passing over the food in the anti-clockwise direction, set the food flags
	to false if the lizard has passed it and increases lizard's size.
	*/
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
	/*
	This function draws the chocolate donut at random places using glutSolidTorus.
	----------
		x: float
			The x-position where the food should be placed.
		y: float
			The y-position where the food should be placed.
		z: float
			The z-position where the food should be placed.
	*/
	// Material properties of food
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
	/*
	This function is used for displaying score at the top of the canvas.
	----------
		x: float
			The x-position where the score should be placed.
		y: float
			The y-position where the score should be placed.
		z: float
			The z-position where the score should be placed.
	*/
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
	/*
	This function is used for displaying game over at the end of game.
	----------
		x: float
			The x-position where the game over should be placed.
		y: float
			The y-position where the game over should be placed.
		z: float
			The z-position where the game over should be placed.
	*/
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
	/*
	This is a glut display callback handler which is called whenever a window needs to be displayed or redisplayed. It clears the
	canvas screen and reloads all the objects of the scene when called. This function also clears the food when lizard passes over 
	them, displays the text score and the text game over at the end of the game.
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	draw_human(x_pos_human, -270.0, z);	
	draw_laser(x_pos_laser, y_pos_laser, z);
	if (fire_signal) {
		draw_laser_beam();
		glutTimerFunc(100, timer_func, 2);
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
	/*
	This function moves the lizard. If it's a clockwise motion, it increases each movement by 4.5, else, it increases each
	movement by -4.5. And set the y-position of lizard to -270 if it went below -270 (out of the canvas).
	*/
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

void calculate_anticlockwise_start_position(void) {
	/*
	This function calculates the start position of lizard in the anticlockwise direction (even channels assuming channel count
	starts from 1).
	*/
	move_lizard_x = 300 - lizard_size * 5 - 24;
	move_lizard_y -= 75;
	channel_count += 1;
	lizard_size += 5;
}

void calculate_clockwise_start_position(void) {
	/*
	This function calculates the start position of lizard in the clockwise direction (odd channels assuming channel count
	starts from 1).
	*/
	move_lizard_x = -300;
	move_lizard_y -= 75;
	channel_count += 1;
	lizard_size += 5;
}

void check_end_position(void) {
	/*
	This function determines if the lizard has reached the end position. If it has, it sets end_game flag to 1 and closes the screen after
	2 secs.
	*/
	if (move_lizard_x <= x_pos_human and move_lizard_y <= -270) {
		isAnimate = 0;
		end_game = 1;
		glutTimerFunc(2000, timer_func, 5);
	}
}

void set_lizard_spawn_values(void) {
	/*
	This function updates the score, calculates random coordinates for displaying food and initializes the necessary values
	required for spawing the lizard at the initial position.
	*/
	if (hit_step > 1) {
		if (channel_count == 0 or channel_count == 1) {
			score += 100;
		}
		else {
			score += 50;
		}
		
	}
	move_lizard_x = -288.0;
	move_lizard_y = 255;
	first_food = true;
	second_food = true;
	lizard_size = 15;
	hit_step = 0;
	fire_signal = 0;
	channel_count = 0;
	calculate_random_coordinates();
	
}

void timer_func(int val) {
	/*
	This function is a glut timer callback function which displays 20 new frames in 1 second to enable lizard movement in both anticlockwise and clockwise direction,
	sets the fire_signal to 0 to disable laser beam display after 100 ms, decreases the lizard size by a factor of 2 if it has been hit once, spawns the lizard 
	after 1000ms and exit after 2000ms at the end of game.
	Parameters:
	-----------
		value: Integer
			This is the timerID.
	*/
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
			glutTimerFunc(animationPeriod, timer_func, 1);
		}
		break;
	case 2:
		fire_signal = 0;
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
	/*
	This function is a glut keyboard callback handler. Whenever a key is pressed, it gets stored in a key
	variable. The switch condition matches the corresponding key condition and perform necessary
	computation.
	Parameters:
	-----------
		key: unsigned char
			The key input received
		x: Integer
			Mouse x-coordinate
		y: Integer
			Mouse y-coordinate
	*/
	switch (key)
	{
	case 'J': case 'j':
		// Enable left movement of laser
		x_pos_laser -= 5;
		glutPostRedisplay();
		break;
	case 'K': case'k':
		// Enable right movement of laser
		x_pos_laser += 5;
		glutPostRedisplay();
		break;
	case ' ':
		// Set fire signal to 1 to display laser beam at the next frame display
		glutKeyboardFunc(NULL);
		fire_signal = 1;
		glutKeyboardFunc(keyboard_func);
		break;
}
}

/************** MAIN FUNCTION **************/
int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	glutDisplayFunc(display_func);
	// Set up light source at the world origin. Initialized at main to reduce overhead though small if initialized in the
	// display callback handler
	setup_light_source();
	init();
	glutTimerFunc(animationPeriod, timer_func, 1);
	glutKeyboardFunc(keyboard_func);
	glutMainLoop();
	return 0;
}
