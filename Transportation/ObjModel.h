#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <gl/glut.h>
#include "Material.h"
#include "Vertex.h"
#include "Normal.h"
#include "Face.h"
#include "Texture.h"
#include "BoundingBox.h"
#include "Vector3D.h"

using namespace std;

enum DisplayMode {Point, WireFrame, Filled};

class ObjModel
{
public:
	ObjModel(void);
	~ObjModel(void);
	virtual void ReadFile(string fileName);
	void Draw();
	vector<Vertex> GetVertices() { return vertices; }
	vector<Normal> GetNormals() { return normals; }
	vector<Face> GetFaces() { return faces; }
	vector<string> GetMaterialNames() { return mtlNames; }
	BoundingBox GetBoundingBox() { return bb; }
	void SetDisplayMode(DisplayMode displayMode);
	void Reset();
	Vertex GetCenter();
	Vector3 GetSize();

protected:
	vector<Vertex> vertices;
	vector<Normal> normals;
	vector<TextureCoord> textureCoords;
	vector<string> mtlNames;
	map<string, Material> materials;
	map<string, vector<int> > facesInMaterials;
	map<string, int> textures;
	vector<Face> faces;
	BoundingBox bb;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	DisplayMode displayMode;
	vector<int> opaqueMaterials;
	vector<int> translucentMaterials;
	void DrawMaterials(int mtlIndex, bool opaque);
	string path;
};

