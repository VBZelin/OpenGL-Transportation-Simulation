#include "utility.h"

void printString(string str)
{
	for (int i = 0; i < str.length(); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
}

void printLargeString(string str)
{
	for (int i = 0; i < str.length(); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
}

Vector3 computeRotatedVector(Vector3 input, float angle)
{
	// This code uses GPU to compute the rotation matrix.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, 0, 1, 0);

	GLfloat rotationMatrix[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) rotationMatrix);
	glPopMatrix();

	Vector3 result = {0, 0, 0};

	result.x += (rotationMatrix[0][0] * input.x + rotationMatrix[1][0] * input.y + rotationMatrix[2][0] * input.z);
	result.y += (rotationMatrix[0][1] * input.x + rotationMatrix[1][1] * input.y + rotationMatrix[2][1] * input.z);
	result.z += (rotationMatrix[0][2] * input.x + rotationMatrix[1][2] * input.y + rotationMatrix[2][2] * input.z);

	return result;
}