#include <cmath>
#include <iostream>
#include "TrackBall.h"

using namespace std;

TrackBall::TrackBall(void)
{
	Reset();
}

TrackBall::~TrackBall(void)
{
}

void TrackBall::UpdateWindowSize(int x, int y)
{
	w = x;
	h = y;
}

void TrackBall::Project(int x, int y, float v[3])
{
	float d, a;
	v[0] = (2.0 * x - w) / w;
	v[1] = (h - 2.0 * y) / h;
	d = (float) sqrt(v[0] * v[0] + v[1] * v[1]);
	v[2] = (float) sqrt(1 - ((d < 1.0) ? d : 1));
	a = 1 / (float) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

void TrackBall::StartMotion(int x, int y)
{
	Project(x, y, lastPos);
}

void TrackBall::ComputeMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, axis[0], axis[1], axis[2]);
	glMultMatrixf((GLfloat *) objectXform);
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) objectXform);
	glPopMatrix();
}

void TrackBall::ContinueMotion(int x, int y)
{
	float curPos[3], dx, dy, dz;
	Project(x, y, curPos);

	dx = curPos[0] - lastPos[0];
	dy = curPos[1] - lastPos[1];
	dz = curPos[2] - lastPos[2];

	if (dx || dy || dz) {
		angle = 90.0F * sqrt(dx * dx + dy * dy + dz * dz) / 3;

		axis[0] = lastPos[1] * curPos[2] - lastPos[2] * curPos[1];
		axis[1] = lastPos[2] * curPos[0] - lastPos[0] * curPos[2];
		axis[2] = lastPos[0] * curPos[1] - lastPos[1] * curPos[0];

		lastPos[0] = curPos[0];
		lastPos[1] = curPos[1];
		lastPos[2] = curPos[2];
	}

	ComputeMatrix();
}

void TrackBall::Reset()
{
	angle = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			objectXform[i][j] = 0;
			if (i == j)
				objectXform[i][j] = 1;
		}
}