#include <iostream>
#include <sstream>
#include <string>
#include <math.h>

#include <gl/glew.h>
#include <gl/glut.h>

#include "ObjModel.h"
#include "TrafficLight.h"
#include "utility.h"

#define PI 3.141593

using namespace std;

// White dash lane.
int numDash;
// Length of dash line.
float dashLength = 1.2;
// Counter for the traffic signal and string event.
float counter = 0.0, numI = 0.0, numS;
// 20 miliseconds for update.
int updateInterval = 20;
// 3D models.
ObjModel car, surveillanceCamera, tree, mountain, house, house2;
// Define traffice light NS and WE directions.
TrafficLight trafficLightNS, trafficLightWE;
// The display list IDs for car, surveillanceCamera, and terrain.
int carID, surveillanceCameraID, terrainID, treeID, mountainID, houseID, house2ID;
// Signals for traffic.
Signal Green_Signal = Green, Red_Signal = Red, Yellow_Signal = Yellow;
// Car position and inital position.
Vector3 carPosition = { 0, 0, 40 }, carInitialPos = { 0, 0, 40 };
// Car speed and world car moving distance.
Vector3 localCarSpeed = { 0, 0, 0 }, worldCarSpeed;
// Setup accelerate speed and decelerate speed.
float accelerationSpeed = 0.1, decelerationSpeed = -0.05;
// Car event and keyboard event.
bool leftUp, rightUp, upUp, downUp, farMirror, backMirror, showCarEvent, stringChange, edgeReach;
// Setup various view values.
float farValue, showSteerValue, mirrorEyeValue = 2.0;
// Initalize content strings.
string viewString = "Normal", cameraString = "First Person", carSpeedString = "0", carHeadingString = "North", mirrorString;
string welcomeString = "WELCOM TO ZELIN'S OPENGL PROGRAM!!!";
string gameScreenString = "For more information, please right click on the screen.";
string gameOriginString = "Please right click on the screen if you want to watch it again.";
string gameIntroString_0 = "UP/DOWN controls moving forward and backward.";
string gameIntroString_1 = "While LEFT/RIGHT controls moving left and right.";
string gameIntroString_2 = "A/D are used to control the view angle of side mirrors.";
string gameIntroString_3 = "F1, F2, and F3 provide users various camera views.";
string notificationString = "Introduction is playing!";
string edgeString = "Warning: please turn back!!!";
// Declare friction value.
float f = 0.005;
// Car direction and heading
float carHeading = 180, cameraHeading = 180;
// Define steering angle.
float steeringAngle = 1.5;
// Define spinning angle.
float showingAngle = 2;
// Third person camera offset in the car's local frame and world frame.
Vector3	localCameraOffset = { 0, 0, -6.5 }, worldCameraOffset, farWorldOffset, showWorldOffset;
// Width and height for the window and five small viewports.
double winWidth, winHeight, sWidth, sHeight, slot, mWidth, mHeight;
// Update the small viewports' size automatically.
void reshape(int w, int h)
{
	winWidth = w, winHeight = h;
	// Update sWidth and sHeight here.
	sWidth = w * 4 / 13, sHeight = h / 4;
	// Update slot.
	slot = w * 25 / 1300;
	// Update mWidth and mHeight
	mWidth = w * 2 / 13, mHeight = h / 8;
}
// Compute direction
void computeDirection() {
	// Compute the car heading.
	if (carHeading == 180)
		carHeadingString = "North";
	else if (carHeading == 0)
		carHeadingString = "South";
	else if (carHeading == 270)
		carHeadingString = "West";
	else if (carHeading == 90)
		carHeadingString = "East";
	else if (carHeading > 0 && carHeading < 90)
		carHeadingString = "SouthEast";
	else if (carHeading > 90 && carHeading < 180)
		carHeadingString = "NorthEast";
	else if (carHeading > 180 && carHeading < 270)
		carHeadingString = "NorthWest";
	else
		carHeadingString = "SouthWest";
}
// Compute world car move distance.
void computeWorldCarSpeed() {
	worldCarSpeed.x = localCarSpeed.z * sin(carHeading * PI / 180);
	worldCarSpeed.y = 0;
	worldCarSpeed.z = localCarSpeed.z * cos(carHeading * PI / 180);
}
// Compute third person world camera offset.
void computerWorldCameraOffset() {
	// Update the third person camera offset in the world frame.
	worldCameraOffset.x = localCameraOffset.z * sin(carHeading * PI / 180);
	worldCameraOffset.z = localCameraOffset.z * cos(carHeading * PI / 180);
	farWorldOffset.x = farValue * sin(carHeading * PI / 180);
	farWorldOffset.z = farValue * cos(carHeading * PI / 180);
}
// Compute show your car event camera offset.
void computeShowCameraOffset() {
	showWorldOffset.x = showSteerValue * sin(cameraHeading * PI / 180);
	showWorldOffset.z = showSteerValue * cos(cameraHeading * PI / 180);
}
// Special key control
void specialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		leftUp = false;
		// Smooth moving.
		f = 0.001;
		// Handle the right turns.
		if (carHeading == 360)
			carHeading = 0;
		else
			carHeading += steeringAngle;		
		// Show Direction
		computeDirection();
	}
	if (key == GLUT_KEY_RIGHT)
	{
		rightUp = false;
		// Smooth moving.
		f = 0.001;
		// Handle the right turns.
		if (carHeading == 0)
			carHeading = 360;
		else
			carHeading -= steeringAngle;	
		// Show Direction.
		computeDirection();
	}
	if (key == GLUT_KEY_UP)
	{
		upUp = false;
		// Acceleration.
		localCarSpeed.z += accelerationSpeed;
		// Largest speed is 0.8.
		if (localCarSpeed.z > 0.8) {
			localCarSpeed.z = 0.8;
		}
	}
	if (key == GLUT_KEY_DOWN)
	{
		downUp = false;
		// Deceleration.
		localCarSpeed.z += decelerationSpeed;
		// Largest speed is -0.5.
		if (localCarSpeed.z < -0.5) {
			localCarSpeed.z = -0.5;
		}
	}
	if (key == GLUT_KEY_F1) {
		if (!showCarEvent) {
			// Update event.
			backMirror = true;
			viewString = "Back Mirror";
		}
	}
	if (key == GLUT_KEY_F3) {
		if (!showCarEvent) {
			// Update event
			farMirror = !farMirror;
		}
		// Update string.
		if (farMirror) {
			cameraString = "Third Person";
		}
		else {
			cameraString = "First Person";
		}
	}
	if (key == GLUT_KEY_F2) {
		if (!farMirror && !backMirror) {
			// Update event.
			showCarEvent = !showCarEvent;
		}
		if (showCarEvent) {
			showWorldOffset.y = 3;
		}
		else {
			showWorldOffset.y = 0;
		}
	}
}
// Special key is Up
void keyBoardUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		upUp = true;
		break;
	case GLUT_KEY_DOWN:
		downUp = true;
		break;
	case GLUT_KEY_RIGHT:
		rightUp = true;
		f = 0.005;
		break;
	case GLUT_KEY_LEFT:
		leftUp = true;
		f = 0.005;
		break;
	case GLUT_KEY_F1:
		backMirror = false;
		viewString = "Normal";
		break;
	}
}
// Draw scene
void drawScene()
{
	// Draw terrain
	glCallList(terrainID);
	// Enable lighting effect.
	glEnable(GL_LIGHTING);

	// Draw car.
	glPushMatrix();
	// Set position.
	glTranslatef(carPosition.x, carPosition.y, carPosition.z);
	// Set rotation.
	glRotatef(carHeading, 0, 1, 0);
	// Set scale.
	glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);
	// Draw by calling ID.
	glCallList(carID);
	// Pop matrix.
	glPopMatrix();

	// Draw house.
	glPushMatrix();
	// Set position.
	glTranslatef(23, 0, 35);
	// Set scale.
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(houseID);
	// Pop matrix.
	glPopMatrix();

	// Draw another house.
	glPushMatrix();
	// Set position.
	glTranslatef(45, 0, -35);
	// Set rotation.
	glRotatef(180, 0, 1, 0);
	// Set scale.
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(houseID);
	// Pop matrix.
	glPopMatrix();

	// Draw house2.
	glPushMatrix();
	// Set position.
	glTranslatef(-25, 0, -15);
	// Set rotation.
	glRotatef(90, 0, 1, 0);
	// Set scale.
	glScalef(8, 8, 8);
	// Draw by calling ID.
	glCallList(house2ID);
	// Pop matrix.
	glPopMatrix();

	// Draw another house2.
	glPushMatrix();
	// Set position.
	glTranslatef(-35, 0, 15);
	// Set rotation.
	glRotatef(-90, 0, 1, 0);
	// Set scale.
	glScalef(8, 8, 8);
	// Draw by calling ID.
	glCallList(house2ID);
	// Pop matrix.
	glPopMatrix();

	// Draw front mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(0, -20, -1000);
	// Set rotation.
	glRotatef(0, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front left mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(-500, -20, -1000);
	// Set rotation.
	glRotatef(0, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front right mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(500, -20, -1000);
	// Set rotation.
	glRotatef(0, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();

	// Draw back mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(0, -20, 1000);
	// Set rotation.
	glRotatef(180, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front left mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(-500, -20, 1000);
	// Set rotation.
	glRotatef(180, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front right mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(500, -20, 1000);
	// Set rotation.
	glRotatef(180, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();

	// Draw east mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(1000, -20, 0);
	// Set rotation.
	glRotatef(-90, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front left mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(1000, -20, -500);
	// Set rotation.
	glRotatef(-90, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front right mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(1000, -20, 500);
	// Set rotation.
	glRotatef(-90, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();

	// Draw west mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(-1000, -20, 0);
	// Set rotation.
	glRotatef(90, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front left mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(-1000, -20, -500);
	// Set rotation.
	glRotatef(90, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Draw front right mountain.
	glPushMatrix();
	// Set position.
	glTranslatef(-1000, -20, 500);
	// Set rotation.
	glRotatef(90, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();

	// Four corners' mountains: first corner.
	glPushMatrix();
	// Set position.
	glTranslatef(-900, -20, -900);
	// Set rotation.
	glRotatef(45, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Second corner.
	glPushMatrix();
	// Set position.
	glTranslatef(900, -20, -900);
	// Set rotation.
	glRotatef(-45, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Third corner.
	glPushMatrix();
	// Set position.
	glTranslatef(-900, -20, 900);
	// Set rotation.
	glRotatef(-225, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();
	// Forth corner.
	glPushMatrix();
	// Set position.
	glTranslatef(900, -20, 900);
	// Set rotation.
	glRotatef(225, 0, 1, 0);
	// Set scale.
	glScalef(30, 30, 30);
	// Draw by calling ID.
	glCallList(mountainID);
	// Pop matrix.
	glPopMatrix();

	// Draw first four trees.
	glPushMatrix();
	// Set position.
	glTranslatef(20, 2.5, 20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(20, 2.5, 15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(15, 2.5, 15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(15, 2.5, 20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();

	// Draw second four trees.
	glPushMatrix();
	// Set position.
	glTranslatef(-20, 2.5, -20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(-20, 2.5, -15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(-15, 2.5, -15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(-15, 2.5, -20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();

	// Draw third four trees.
	glPushMatrix();
	// Set position.
	glTranslatef(-20, 2.5, 20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(-20, 2.5, 15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(-15, 2.5, 15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(-15, 2.5, 20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();

	// Draw forth four trees.
	glPushMatrix();
	// Set position.
	glTranslatef(20, 2.5, -20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(20, 2.5, -15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(15, 2.5, -15);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();
	// Draw tree.
	glPushMatrix();
	// Set position.
	glTranslatef(15, 2.5, -20);
	// Set scale
	glScalef(1.5, 1.5, 1.5);
	// Draw by calling ID.
	glCallList(treeID);
	// Pop matrix.
	glPopMatrix();

	// North-East (Green_Signal).
	glPushMatrix();
	// Set position.
	glTranslatef(10, 0, -10.5);
	// Set scale.
	glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
	// Draw traffic light.
	trafficLightNS.Draw();
	// Pop matrix.
	glPopMatrix();
	// Draw surveillance camera.
	glPushMatrix();
	glTranslatef(10, 0, -10);
	glRotatef(-45, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();

	// South-West (Green_Signal).
	glPushMatrix();
	// Set position.
	glTranslatef(-10, 0, 10.5);
	// Rotate.
	glRotatef(180, 0, 1, 0);
	// Set scale.
	glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);
	// Draw traffic light.
	trafficLightNS.Draw();
	// Pop matrix.
	glPopMatrix();
	// Draw surveillance camera.
	glPushMatrix();
	glTranslatef(-10, 0, 10);
	glRotatef(135, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();

	// South-East (Red_Signal).
	glPushMatrix();
	// Set position.
	glTranslatef(10, 0, 10.5);
	// Rotate.
	glRotatef(-90, 0, 1, 0);
	// Set scale.
	glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);
	// Draw traffic light.
	trafficLightWE.Draw();
	// Pop matrix.
	glPopMatrix();
	// Draw surveillance camera.
	glPushMatrix();
	glTranslatef(10, 0, 10);
	glRotatef(-135, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();

	// North-West (Red_Signal).
	glPushMatrix();
	// Set position.
	glTranslatef(-10, 0, -10.5);
	// Rotate.
	glRotatef(90, 0, 1, 0);
	// Set scale.
	glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);

	// Draw traffic light.
	trafficLightWE.Draw();
	// Pop matrix.
	glPopMatrix();
	// Draw surveillance camera.
	glPushMatrix();
	glTranslatef(-10, 0, -10);
	glRotatef(45, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();
}
// Compute dash;
void computeDash() {
	numDash = 1000 / dashLength;
}
// Initialization.
void init()
{
	glClearColor(0.5, 0.5, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Note that the light is defined in the eye or camera frame.
	GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0};
	// Initalize shading and lighting.
	GLfloat ambient[] = {0.3, 0.3, 0.3, 1};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);	
	glEnable(GL_LIGHT0);

	// Generate display list for the surveillance camera.
	surveillanceCameraID = glGenLists(1);
	glNewList(surveillanceCameraID, GL_COMPILE);
	surveillanceCamera.Draw();
	glEndList();
	// Generate display list for the car.
	carID = glGenLists(2);
	glNewList(carID, GL_COMPILE);
	car.Draw();
	glEndList();
	// Generate display list for the tree.
	treeID = glGenLists(3);
	glNewList(treeID, GL_COMPILE);
	tree.Draw();
	glEndList();
	// Generate display list for the tree.
	mountainID = glGenLists(4);
	glNewList(mountainID, GL_COMPILE);
	mountain.Draw();
	glEndList();
	// Generate display list for the house.
	houseID = glGenLists(5);
	glNewList(houseID, GL_COMPILE);
	house.Draw();
	glEndList();
	// Generate display list for the house2.
	house2ID = glGenLists(6);
	glNewList(house2ID, GL_COMPILE);
	house2.Draw();
	glEndList();
	// Generate the display list for terrain, including road and grass.
	terrainID = glGenLists(7);
	glNewList(terrainID, GL_COMPILE);
	glDisable(GL_LIGHTING);
	// Grass
	glColor3f(0, 0.6, 0);
	glBegin(GL_QUADS);
		glVertex3f(-1000, 0, 1000);
		glVertex3f(-10, 0, 1000);
		glVertex3f(-10, 0, 10);
		glVertex3f(-1000, 0, 10);

		glVertex3f(10, 0, 1000);
		glVertex3f(1000, 0, 1000);
		glVertex3f(1000, 0, 10);
		glVertex3f(10, 0, 10);

		glVertex3f(10, 0, -10);
		glVertex3f(1000, 0, -10);
		glVertex3f(1000, 0, -1000);
		glVertex3f(10, 0, -1000);

		glVertex3f(-1000, 0, -10);
		glVertex3f(-10, 0, -10);
		glVertex3f(-10, 0, -1000);
		glVertex3f(-1000, 0, -1000);
	glEnd();
	// Roads
	glBegin(GL_QUADS);
		glColor3f(0.2, 0.2, 0.2);

		glVertex3f(-10, 0, 1000);
		glVertex3f(10, 0, 1000);
		glVertex3f(10, 0, -1000);
		glVertex3f(-10, 0, -1000);

		glVertex3f(-1000, 0, 10);
		glVertex3f(1000, 0, 10);
		glVertex3f(1000, 0, -10);
		glVertex3f(-1000, 0, -10);
	glEnd();
	// South yellow line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 0);
		glVertex3f(-0.1, 0.05, 10);
		glVertex3f(0.1, 0.05, 10);
		glVertex3f(0.1, 0.05, 1000);
		glVertex3f(-0.1, 0.05, 1000);
	glEnd();
	// North yellow line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 0);
		glVertex3f(-0.1, 0.05, -10);
		glVertex3f(0.1, 0.05, -10);
		glVertex3f(0.1, 0.05, -1000);
		glVertex3f(-0.1, 0.05, -1000);
	glEnd();
	// West yellow line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 0);
		glVertex3f(-10, 0.05, 0.1);
		glVertex3f(-10, 0.05, -0.1);
		glVertex3f(-1000, 0.05, 0.1);
		glVertex3f(-1000, 0.05, 0.1);
	glEnd();
	// East yellow line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 0);
		glVertex3f(10, 0.05, -0.1);
		glVertex3f(10, 0.05, 0.1);
		glVertex3f(1000, 0.05, 0.1);
		glVertex3f(1000, 0.05, -0.1);
	glEnd();
	// South right white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(9.7, 0.05, 10);
		glVertex3f(9.9, 0.05, 10);
		glVertex3f(9.9, 0.05, 1000);
		glVertex3f(9.7, 0.05, 1000);
	glEnd();
	// South left white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(-9.7, 0.05, 10);
		glVertex3f(-9.9, 0.05, 10);
		glVertex3f(-9.9, 0.05, 1000);
		glVertex3f(-9.7, 0.05, 1000);
	glEnd();
	// North left white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(-9.7, 0.05, -10);
		glVertex3f(-9.9, 0.05, -10);
		glVertex3f(-9.9, 0.05, -1000);
		glVertex3f(-9.7, 0.05, -1000);
	glEnd();
	// North right white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(9.7, 0.05, -10);
		glVertex3f(9.9, 0.05, -10);
		glVertex3f(9.9, 0.05, -1000);
		glVertex3f(9.7, 0.05, -1000);
	glEnd();
	// West left white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(-10, 0.05, -9.7);
		glVertex3f(-10, 0.05, -9.9);
		glVertex3f(-1000, 0.05, -9.9);
		glVertex3f(-1000, 0.05, -9.7);
	glEnd();
	// West right white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(-10, 0.05, 9.7);
		glVertex3f(-10, 0.05, 9.9);
		glVertex3f(-1000, 0.05, 9.9);
		glVertex3f(-1000, 0.05, 9.7);
	glEnd();
	// East left white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(10, 0.05, 9.7);
		glVertex3f(10, 0.05, 9.9);
		glVertex3f(1000, 0.05, 9.9);
		glVertex3f(1000, 0.05, 9.7);
	glEnd();
	// East right white line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 1);
		glVertex3f(10, 0.05, -9.7);
		glVertex3f(10, 0.05, -9.9);
		glVertex3f(1000, 0.05, -9.9);
		glVertex3f(1000, 0.05, -9.7);
	glEnd();
	// Calculate number of dash lines
	computeDash();
	// White dash lanes
	for (int i = 0; i <= numDash; i++) {
		if (i % 2 == 0) {
			glColor3f(1, 1, 1);
			// South part
			glBegin(GL_POLYGON);
				glVertex3f(4.8, 0.05, 10 + (i * dashLength));
				glVertex3f(5.0, 0.05, 10 + (i * dashLength));
				glVertex3f(5.0, 0.05, 10 + dashLength * (i + 1));
				glVertex3f(4.8, 0.05, 10 + dashLength * (i + 1));
			glEnd();
			glBegin(GL_POLYGON);
				glVertex3f(-4.8, 0.05, 10 + (i * dashLength));
				glVertex3f(-5.0, 0.05, 10 + (i * dashLength));
				glVertex3f(-5.0, 0.05, 10 + dashLength * (i + 1));
				glVertex3f(-4.8, 0.05, 10 + dashLength * (i + 1));
			glEnd();
			// North part
			glBegin(GL_POLYGON);
				glVertex3f(-4.8, 0.05, -10 - (i * dashLength));
				glVertex3f(-5.0, 0.05, -10 - (i * dashLength));
				glVertex3f(-5.0, 0.05, -10 - dashLength * (i + 1));
				glVertex3f(-4.8, 0.05, -10 - dashLength * (i + 1));
			glEnd();
			glBegin(GL_POLYGON);
				glVertex3f(4.8, 0.05, -10 - (i * dashLength));
				glVertex3f(5.0, 0.05, -10 - (i * dashLength));
				glVertex3f(5.0, 0.05, -10 - dashLength * (i + 1));
				glVertex3f(4.8, 0.05, -10 - dashLength * (i + 1));
			glEnd();
			// West part
			glBegin(GL_POLYGON);
				glVertex3f(-10 - (i * dashLength), 0.05, 4.8);
				glVertex3f(-10 - (i * dashLength), 0.05, 5.0);
				glVertex3f(-10 - dashLength * (i + 1), 0.05, 5.0);
				glVertex3f(-10 - dashLength * (i + 1), 0.05, 4.8);
			glEnd();
			glBegin(GL_POLYGON);
				glVertex3f(-10 - (i * dashLength), 0.05, -4.8);
				glVertex3f(-10 - (i * dashLength), 0.05, -5.0);
				glVertex3f(-10 - dashLength * (i + 1), 0.05, -5.0);
				glVertex3f(-10 - dashLength * (i + 1), 0.05, -4.8);
			glEnd();
			// East part
			glBegin(GL_POLYGON);
				glVertex3f(10 + (i * dashLength), 0.05, 4.8);
				glVertex3f(10 + (i * dashLength), 0.05, 5.0);
				glVertex3f(10 + dashLength * (i + 1), 0.05, 5.0);
				glVertex3f(10 + dashLength * (i + 1), 0.05, 4.8);
			glEnd();
			glBegin(GL_POLYGON);
				glVertex3f(10 + (i * dashLength), 0.05, -4.8);
				glVertex3f(10 + (i * dashLength), 0.05, -5.0);
				glVertex3f(10 + dashLength * (i + 1), 0.05, -5.0);
				glVertex3f(10 + dashLength * (i + 1), 0.05, -4.8);
			glEnd();
		}
	}
	glEndList();
}
// Display function
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Setup viewport, projection, and camera for the main view.
	glViewport(0, 0, winWidth, winHeight - sHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float)winWidth / (winHeight - sHeight), 1, 1500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Back mirror event
	if (backMirror) {
		glRotatef(180, 0, 1, 0);
		viewString = "Back Mirror";
	}
	// Update event
	if (farMirror == false) {
		farValue = 0;
	}
	else {
		farValue = -6.5;
	}
	if (showCarEvent == false) {
		showSteerValue = 0;
	}
	else {
		showSteerValue = -12;
	}
	// Update the code here for the correct third person camera that moves with the car.
	gluLookAt(carPosition.x + worldCameraOffset.x + farWorldOffset.x + showWorldOffset.x, carPosition.y + 2 + showWorldOffset.y, carPosition.z + worldCameraOffset.z + farWorldOffset.z + showWorldOffset.z, carPosition.x, carPosition.y + 1.5,
		carPosition.z, 0, 1, 0);
	drawScene();

	// Setup the viewport, projection, camera for the South-East and draw the scene again.
	glViewport(slot, winHeight - sHeight, sWidth, sHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float)sWidth / sHeight, 1, 1500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Camera height is 3 meters, looking at (0, 3, -5) point
	gluLookAt(10, 4, 10, 0, 3,
		-5, 0, 1, 0);
	drawScene();

	// Setup viewport, projection, and camera for the top view camera and draw the scene again.
	glViewport(2 * slot + sWidth, winHeight - sHeight, sWidth, sHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)sWidth / sHeight, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 45, 0, 0, 0,
		0, 0, 0, 1);
	drawScene();

	// Setup viewport, projection, camera for the South-West camera and draw the scene again.
	glViewport(3 * slot + 2 * sWidth, winHeight - sHeight, sWidth, sHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float)sWidth / sHeight, 1, 1500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-10, 4, 10, 5, 3,
		0, 0, 1, 0);
	drawScene();

	// Setup left mirror, and draw the scene again.
	glViewport(slot, winHeight - sHeight - mHeight - 20, mWidth, mHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float)mWidth / mHeight, 1, 1500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180, 0, 1, 0);
	gluLookAt(carPosition.x + 2 + worldCameraOffset.x, carPosition.y + 0.6, carPosition.z + worldCameraOffset.z, carPosition.x + mirrorEyeValue, carPosition.y + 0.6,
		carPosition.z, 0, 1, 0);
	drawScene();

	// Setup right mirror, and draw the scene again.
	glViewport(3 * slot + 3 * sWidth - mWidth, winHeight - sHeight - mHeight - 20, mWidth, mHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float)mWidth / mHeight, 1, 1500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180, 0, 1, 0);
	gluLookAt(carPosition.x - 2 + worldCameraOffset.x, carPosition.y + 0.6, carPosition.z + worldCameraOffset.z, carPosition.x - mirrorEyeValue, carPosition.y + 0.6,
		carPosition.z, 0, 1, 0);
	drawScene();

	// Display text info
	glColor3f(1.0, 1.0, 1.0);
	glWindowPos2f(winWidth / 2 - sWidth / 2, winHeight - sHeight - 20);
	printString("Direction: " + carHeadingString + "    " + "Speed: " + carSpeedString);
	glWindowPos2f(winWidth / 2 - sWidth / 2, winHeight - sHeight - 40);
	printString("View Angle: " + viewString + "    " + "Camera View: " + cameraString);
	if (stringChange) {
		glColor3f(1.0, 0.0, 0.0);
		glWindowPos2f(winWidth / 2 - sWidth / 4, winHeight - sHeight - 120);
		printString(notificationString);
	}
	else {
		glColor3f(1.0, 1.0, 0.0);
	}
	// Setup strings.
	glWindowPos2f(winWidth / 2 - sWidth / 2, winHeight - sHeight - 80);
	printString(gameScreenString);
	glColor3f(0.0, 0.0, 1.0);
	glWindowPos2f(slot, winHeight - sHeight - mHeight - 40);
	printString("Left Side Mirror: "  + mirrorString);
	glWindowPos2f(3 * slot + 3 * sWidth - mWidth, winHeight - sHeight - mHeight - 40);
	printString("Right Side Mirror: " + mirrorString);
	if (edgeReach) {
		glColor3f(1.0, 0.0, 0.0);
		glWindowPos2f(winWidth / 2 - sWidth / 4, winHeight - sHeight - 140);
		printLargeString(edgeString);
	}

	glutSwapBuffers();
	glFlush();
}
// Keyboard function
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		// Add code for reset
		carPosition.x = carInitialPos.x;
		carPosition.y = carInitialPos.y;
		carPosition.z = carInitialPos.z;
		carHeading = 180;
		carHeadingString = "N";
		localCarSpeed.z = 0;
		worldCarSpeed.x = 0;
		worldCarSpeed.z = 0;
		localCameraOffset.x = 0;
		localCameraOffset.y = 0;
		localCameraOffset.z = -6.5;
		worldCameraOffset.x = localCameraOffset.z * sin(carHeading * PI / 180);
		worldCameraOffset.z = localCameraOffset.z * cos(carHeading * PI / 180);
		counter = 0;
		numI = 0;
		break;
	case 'b':
		// Add code for breaking.
		localCarSpeed.z = 0;
		worldCarSpeed.x = worldCarSpeed.z = 0;
		break;
	case 27:
		exit(0);
		break;
	case 'a':
		// Move mirror angle with -0.05 vlaue
		mirrorEyeValue -= 0.05;
		if (mirrorEyeValue <= 0) {
			mirrorEyeValue = 0;
		}
		break;
		break;
	case 'd':
		// Move mirror angle with 0.05 vlaue
		mirrorEyeValue += 0.05;
		if (mirrorEyeValue >= 4) {
			mirrorEyeValue = 4;
		}
	}

	// Re-render
	glutPostRedisplay();
}
// Edge detective
void edgeDetect() {
	if (carPosition.x >= 850) {
		carPosition.x = 850;
		edgeReach = true;
	}
	else if (carPosition.x <= -850) {
		carPosition.x = -850;
		edgeReach = true;
	}
	else if (carPosition.z >= 850) {
		carPosition.z = 850;
		edgeReach = true;
	}
	else if (carPosition.z <= -850) {
		carPosition.z = -850;
		edgeReach = true;
	}
	else {
		edgeReach = false;
	}
}
// Update function
void update()
{
	// Update car position.
	carPosition.x += worldCarSpeed.x;
	carPosition.z += worldCarSpeed.z;
	// Detect edge.
	edgeDetect();
	// Update car speed.
	if (localCarSpeed.z > 0) {
		localCarSpeed.z -= f;
	}
	else if (localCarSpeed.z < 0) {
		localCarSpeed.z += f;
	}
	// Compute car speed.
	computeWorldCarSpeed();
	// Compute camera offset.
	computerWorldCameraOffset();
	if (abs(int(localCarSpeed.z * 100)) == 0) {
		worldCarSpeed.x = worldCarSpeed.z = 0;
	}
	// If it is show car event.
	if (showCarEvent) {
		cameraHeading += showingAngle;
		if (cameraHeading >= 360) {
			cameraHeading = 0;
		}
	}
	else {
		cameraHeading = 180;
	}
	// Compute camera offset.
	computeShowCameraOffset();
	// Stream used for the conversion
	ostringstream convert;
	// Insert the textual representation of 'Number' in the characters in the stream
	convert << abs(int(localCarSpeed.z * 100));
	carSpeedString = convert.str();
	convert.str("");
	convert.clear();
	convert << mirrorEyeValue;
	mirrorString = convert.str();
	convert.str("");
	convert.clear();
}
// Update event.
void update_event() {
	// State machine for the traffic signals using three variables: Green_Signal, Red_Signal, and counter.
	counter += 0.02;
	numI += 0.02;
	// If it is string change event.
	if (stringChange) {
		numS += 0.02;
		if (numS <= 1) {
			gameScreenString = gameIntroString_0;
		}
		else if (numS > 1 && numS <= 2) {
			gameScreenString = gameIntroString_1;
		}
		else if (numS > 2 && numS <= 3) {
			gameScreenString = gameIntroString_2;
		}
		else if (numS > 3 && numS <= 4) {
			gameScreenString = gameIntroString_3;
		}
		else if (numS > 4) {
			gameScreenString = gameOriginString;
			numS = 0;
			stringChange = false;
		}
	}
	// Count numbers to update signal materials.
	if (counter <= 2.5) {
		trafficLightNS.setSignal(Green_Signal);
	}
	else if (counter > 2.5 && counter <= 3) {
		trafficLightNS.setSignal(Yellow_Signal);
	}
	else if (counter > 3 && counter <= 6) {
		trafficLightNS.setSignal(Red_Signal);
	}
	else if (counter > 6) {
		counter = 0;
	}
	if (numI <= 3.0) {
		trafficLightWE.setSignal(Red_Signal);
	}
	else if (numI > 3.0 && numI <= 5.5) {
		trafficLightWE.setSignal(Green_Signal);
	}
	else if (numI > 5.5 && numI <= 6) {
		trafficLightWE.setSignal(Yellow_Signal);
	}
	else if (numI > 6) {
		numI = 0;
	}
}
// Set a timer
void timer(int miliseconds)
{
	// Update function
	update();
	// Update textures
	update_event();
	// Set a timer
	glutTimerFunc(updateInterval, timer, updateInterval);
	// Redisplay
	glutPostRedisplay();
}
// create menu function
void introMenuFunc(int id) {
	switch (id)
	{
	case 1:
		glClearColor(0.5, 0.5, 1.0, 1);
		break;
	case 2:
		glClearColor(0.5, 0.5, 0.6, 1);
		break;
	case 3:
		glClearColor(0, 0, 0, 1);
		break;
	case 4:
		stringChange = true;
		break;
	}
	glutPostRedisplay();
}
// create menu
void createMenu()
{
	// Initalize menu items.
	int introMenu;
	// Define function.
	introMenu = glutCreateMenu(introMenuFunc);
	// Add key items.
	glutAddMenuEntry("Day Time", 1);
	glutAddMenuEntry("Cloudy Weather", 2);
	glutAddMenuEntry("Night Time", 3);
	glutAddMenuEntry("About", 4);
	// Attach menu.
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
// Main function
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// Screen position.
	glutInitWindowPosition(100, 100);
	// Initialize windows' windth and height.
	winWidth = 1300, winHeight = 800;
	// Initialize the parameters of small screens. 
	sWidth = 400, sHeight = 200;
	// Initialize the parameters of mirror screens.
	mWidth = 200, mHeight = 100;
	// Set slot's size.
	slot = 25;
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Zelin's Traffic Simulation");
	glewInit();
	// Load the 3D models.
	trafficLightNS.ReadFile("Models/TrafficLight.obj");
	trafficLightWE.ReadFile("Models/TrafficLight.obj");
	car.ReadFile("Models/Honda_S2000_inch.obj");
	house.ReadFile("Models/house.obj");
	house2.ReadFile("Models/house2.obj");
	tree.ReadFile("Models/lowpolytree.obj");
	mountain.ReadFile("Models/lowpolymountains.obj");
	surveillanceCamera.ReadFile("Models/camera.obj");
	// Initalize.
	init();
	cout << welcomeString << endl;
	// Create menu.
	createMenu();
	// Display function.
	glutDisplayFunc(display);
	// Reshape function.
	glutReshapeFunc(reshape);
	// Keyboard function (Normal).
	glutKeyboardFunc(keyboard);
	// Keyboard function (Special).
	glutSpecialFunc(specialKey);
	glutSpecialUpFunc(keyBoardUp);
	// Update time.
	glutTimerFunc(0, timer, updateInterval);
	// Main loop.
	glutMainLoop();

	system("pause");
}