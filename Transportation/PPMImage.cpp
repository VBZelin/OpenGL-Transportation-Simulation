#include "PPMImage.h"


PPMImage::PPMImage(void)
{
}


PPMImage::~PPMImage(void)
{
	delete image;
}

void PPMImage::ReadFile(string fileName)
{
	file.open(fileName, ios_base::binary);

	if (!file.is_open())
	{
		cout << fileName << " couldn't be opened.\n";
		system("pause");
		exit(1);
	}
	
	string fileType;
	file >> fileType;

	CheckComment();
	file >> width >> height;
	image = new unsigned char [width * height * 3];

	int maxValue;

	file >> maxValue;

	if (fileType == "P3")
	{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			for (int k = 0; k < 3; k++)
			{
				string temp;
				file >> temp;
				image[(i * width + j) * 3 + k] = (unsigned char) atoi(temp.c_str());
			}
	}

	else if (fileType == "P6")
	{
		file.getline(buf, 127);
		file.read((char *) image, height * width * 3);
	}
}

void PPMImage::CheckComment()
{
	c = file.peek();

	while (c == '#' || c == '\n' || c == ' ')
	{
		file.getline(buf, 127);
		c = file.peek();
	}
}