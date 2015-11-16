#pragma once
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class PPMImage
{
public:
	PPMImage(void);
	~PPMImage(void);
	int width, height;
	unsigned char* image;
	void ReadFile(string fileName);

private:
	char c, buf[128];
	ifstream file;
	void CheckComment();
};

