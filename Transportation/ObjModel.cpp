#include <cfloat>
#include "ObjModel.h"
#include "PPMImage.h"

ObjModel::ObjModel(void)
{
	Reset();
}

ObjModel::~ObjModel(void)
{
}

void ObjModel::Reset()
{
	vertices.clear();
	normals.clear();
	textureCoords.clear();
	mtlNames.clear();
	materials.clear();
	facesInMaterials.clear();
	faces.clear();
	this->translucentMaterials.clear();
	this->opaqueMaterials.clear();

	if (textures.size() > 0)
	{
		map<string, int>::iterator p;

		for (p = textures.begin(); p != textures.end(); p++)
			glDeleteTextures(1, (GLuint *) &p->second);

		textures.clear();
	}

	xmin = FLT_MAX;
	xmax = -FLT_MAX;
	ymin = FLT_MAX;
	ymax = -FLT_MAX;
	zmin = FLT_MAX;
	zmax = -FLT_MAX;

	displayMode = Filled;
}

void ObjModel::ReadFile(string fileName)
{
	Reset();

	int t1 = fileName.find_last_of('/');
	if (t1 == string::npos)
	{
		path = "";
	}
	else
	{
		path = fileName.substr(0, t1 + 1);
	}

	string temp;
	char c, buf[256];

	ifstream objFile(fileName);

	if (!objFile.is_open())
	{
		cout << fileName << " couldn't be opened.\n";
		system("pause");
		exit(1);
	}

	c = objFile.peek();

	while (c == '#' || c == '\n' || c == ' ')
	{
		objFile.getline(buf, 255);
		c = objFile.peek();
	}

	c = objFile.peek();
	if (c == 'm')
	{
		objFile >> temp;
		objFile >> temp;
	}

	string mtlFileName = path + temp;
	ifstream mtlFile(mtlFileName);

	if (!mtlFile.is_open())
	{
		cout << mtlFileName << " cannot be opened." << endl;
		system("pause");
		exit(1);
	}

	c = mtlFile.peek();
	while (c == '#')
	{
		mtlFile.getline(buf, 127);
		c = mtlFile.peek();
	}

	mtlFile >> temp;

	string mtlName;
	Material *mtl;
	while (!mtlFile.eof())
	{
		if (temp == "newmtl")
		{
			mtlFile >> mtlName;
			mtlNames.push_back(mtlName);
			mtl = &materials[mtlName];
			mtl->Ns = 900;
			mtl->d = 1.0;
		}
		else if (temp == "Ka")
			mtlFile >> mtl->Ka[0] >> mtl->Ka[1] >> mtl->Ka[2];
		else if (temp == "Kd")
			mtlFile >> mtl->Kd[0] >> mtl->Kd[1] >> mtl->Kd[2];
		else if (temp == "Ks")
			mtlFile >> mtl->Ks[0] >> mtl->Ks[1] >> mtl->Ks[2];
		else if (temp == "Ns")
			mtlFile >> mtl->Ns;
		else if (temp == "d")
			mtlFile >> mtl->d;
		else if (temp == "illum")
			mtlFile >> mtl->illum;
		else if (temp == "map_Kd")
		{
			mtlFile >> mtl->textureFileName;

			PPMImage texture;
			texture.ReadFile(path + mtl->textureFileName);

			GLuint textureNumber;
			glGenTextures(1, &textureNumber);
			glBindTexture(GL_TEXTURE_2D, textureNumber);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, 
				GL_RGB, GL_UNSIGNED_BYTE, texture.image);

			textures[mtlName] = textureNumber;
		}

		mtlFile >> temp;
	}

	string faceMaterial;
	int faceIndex = 0;
	objFile >> temp;

	while (!objFile.eof())
	{
		bool readNewLine = false;
		float x, y, z;
		
		if (temp == "v")
		{	
			objFile >> x >> y >> z;
			if (x < xmin)
				xmin = x;
			if (x > xmax)
				xmax = x;
			if (y < ymin)
				ymin = y;
			if (y > ymax)
				ymax = y;
			if (z < zmin)
				zmin = z;
			if (z > zmax)
				zmax = z;

			Vertex v = {x, y, z};
			vertices.push_back(v);

		}
		else if (temp == "vn")
		{
			objFile >> x >> y >> z;
			Normal n = {x, y, z};
			normals.push_back(n);
		}
		else if (temp == "vt")
		{
			objFile >> x >> y;
			TextureCoord textureCoord = {x, y};
			textureCoords.push_back(textureCoord);
		}
		else if (temp == "usemtl")
			objFile >> faceMaterial;
		else if (temp == "f")
		{
			Face face;
			objFile >> temp;
			
			do {
				int t1 = temp.find_first_of('/');
				int t2 = temp.find_first_of('/', t1 + 1);

				string tmp = temp.substr(0, t1);
				face.vertices.push_back(atoi(tmp.c_str()));

				if (t2 - t1 > 1)
				{
					tmp = temp.substr(t1 + 1, t2 - t1 - 1);
					face.textures.push_back(atoi(tmp.c_str()));
				}

				tmp = temp.substr(t2 + 1, temp.size() - t2);			
				face.normals.push_back(atoi(tmp.c_str()));

				objFile >> temp;
				if (temp[0] == 'v' || temp[0] == 'u' || temp[0] == 'f')
					readNewLine = true;

			} while (temp[0] != 'f' && temp[0] != '#' && temp[0] != 'v' && temp[0] != 's' 
				&& temp[0] != 'g' && temp[0] != 'u' && !objFile.eof());

			faces.push_back(face);
			facesInMaterials[faceMaterial].push_back(faceIndex);
			faceIndex++;
		}

		if (temp == "#" || temp == "g" || temp == "s")
		{
			objFile.getline(buf, 255);
		}

		if (readNewLine)
			continue;

		objFile >> temp;
	}

	bb.xmin = xmin;
	bb.xmax = xmax;
	bb.ymin = ymin;
	bb.ymax = ymax;
	bb.zmin = zmin;
	bb.zmax = zmax;

	for (int i = 0; i < mtlNames.size(); i++)
	{
		Material mtl = materials[mtlNames[i]];
		if (mtl.d == 1)
			opaqueMaterials.push_back(i);
		else
			translucentMaterials.push_back(i);
	}
}

void ObjModel::DrawMaterials(int mtlIndex, bool opaque)
{
	Material mtl = materials[mtlNames[mtlIndex]];

	if (mtl.textureFileName != "")
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[mtlNames[mtlIndex]]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	}
	else
		glDisable(GL_TEXTURE_2D);

	if (opaque)
	{
		glDisable(GL_BLEND);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtl.Kd);
	}
	else
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float kd[4] = {mtl.Kd[0], mtl.Kd[1], mtl.Kd[2], mtl.d};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mtl.Ka);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtl.Ks);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mtl.Ns / 1000.0 * 128);

	vector<int> faceIndices = facesInMaterials[mtlNames[mtlIndex]];

	for (int j = 0; j < faceIndices.size(); j++)
	{
		Face face = faces[faceIndices[j]];

		int numVertices = face.vertices.size();
		if (displayMode == Point)
		{	
			glBegin(GL_POINTS);
			glColor3f(1, 0, 0);
		}
		else if (displayMode == WireFrame)
		{	
			glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
		}
		else
		{
			glBegin(GL_POLYGON);
		}

		for (int k = 0; k < numVertices; k++)
		{
			if (normals.size() != 0)
			{
				Normal normal = normals[face.normals[k] - 1];
				glNormal3f(normal.x, normal.y, normal.z);
			}

			if (mtl.textureFileName != "")
			{
				TextureCoord textureCoord = textureCoords[face.textures[k] - 1];
				glTexCoord2f(textureCoord.u, textureCoord.v);
			}

			Vertex vertex = vertices[face.vertices[k] - 1];
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
		glEnd();
	}
}

void ObjModel::Draw()
{
	glPointSize(1.5);
	glLineWidth(2);
	glEnable(GL_POINT_SMOOTH);

	for (int i = 0; i < this->opaqueMaterials.size(); i++)
		this->DrawMaterials(opaqueMaterials[i], true);

	for (int i = 0; i < this->translucentMaterials.size(); i++)
		this->DrawMaterials(translucentMaterials[i], false);
}

Vertex ObjModel::GetCenter()
{
	Vertex center = {
		(xmin + xmax) / 2,
		(ymin + ymax) / 2,
		(zmin + zmax) / 2
	};

	return center;
}

Vector3 ObjModel::GetSize()
{
	Vector3 size = {
		xmax - xmin,
		ymax - ymin,
		zmax - zmin
	};
	return size;
}

void ObjModel::SetDisplayMode(DisplayMode displayMode)
{
	this->displayMode = displayMode;
	switch (displayMode)
	{
	case WireFrame:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_LIGHTING);
		break;
	case Filled:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		break;
	case Point:
		glDisable(GL_LIGHTING);
	}
	glutPostRedisplay();
}