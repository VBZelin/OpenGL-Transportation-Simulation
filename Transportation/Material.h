#include <string>

using namespace std;

struct Material {
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Ns;
	float d;
	int illum;
	string textureFileName;
};