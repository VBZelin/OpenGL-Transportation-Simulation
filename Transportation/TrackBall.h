#pragma once
#include <gl/glut.h>

class TrackBall
{
public:
	TrackBall(void);
	~TrackBall(void);
	void UpdateWindowSize(int x, int y);
	void Project(int x, int y, float v[3]);
	void StartMotion(int x, int y);
	void ContinueMotion(int x, int y);
	void ComputeMatrix();
	GLfloat * GetMatrix() { return (GLfloat *) objectXform; }
	void Reset();

private:
	int w, h;
	float angle, axis[3];
	float lastPos[3];
	GLfloat objectXform[4][4];
};

