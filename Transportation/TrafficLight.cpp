#include "TrafficLight.h"


TrafficLight::TrafficLight(void)
{
}


TrafficLight::~TrafficLight(void)
{
}

void TrafficLight::setMaterials()
{
	// Assign value to yellowOff
	yellowOff.Ka[0] = 0.0; yellowOff.Ka[1] = 0.0; yellowOff.Ka[2] = 0.0;
	yellowOff.Kd[0] = 0.1; yellowOff.Kd[1] = 0.1; yellowOff.Kd[2] = 0.0;
	yellowOff.Ks[0] = 0.33; yellowOff.Ks[1] = 0.33; yellowOff.Ks[2] = 0.33;
	yellowOff.illum = 0; yellowOff.d = 1.0;
	// Assign value to yellowOn
	yellowOn.Ka[0] = 0.0; yellowOn.Ka[1] = 0.0; yellowOn.Ka[2] = 0.0;
	yellowOn.Kd[0] = 1.0; yellowOn.Kd[1] = 1.0; yellowOn.Kd[2] = 0.0;
	yellowOn.Ks[0] = 0.33; yellowOn.Ks[1] = 0.33; yellowOn.Ks[2] = 0.33;
	yellowOn.illum = 10; yellowOn.d = 1.0;
	// Assign value to redOff
	redOff.Ka[0] = 0.0; redOff.Ka[1] = 0.0; redOff.Ka[2] = 0.0;
	redOff.Kd[0] = 0.1; redOff.Kd[1] = 0.0; redOff.Kd[2] = 0.0;
	redOff.Ks[0] = 0.33; redOff.Ks[1] = 0.33; redOff.Ks[2] = 0.33;
	redOff.illum = 0; redOff.d = 1.0;
	// Assign value to redOn
	redOn.Ka[0] = 0.0; redOn.Ka[1] = 0.0; redOn.Ka[2] = 0.0;
	redOn.Kd[0] = 1.0; redOn.Kd[1] = 0.0; redOn.Kd[2] = 0.0;
	redOn.Ks[0] = 0.33; redOn.Ks[1] = 0.33; redOn.Ks[2] = 0.33;
	redOn.illum = 10; redOn.d = 1.0;
	// Assign value to greenOff
	greenOff.Ka[0] = 0.0; greenOff.Ka[1] = 0.0; greenOff.Ka[2] = 0.0;
	greenOff.Kd[0] = 0.0; greenOff.Kd[1] = 0.1; greenOff.Kd[2] = 0.0;
	greenOff.Ks[0] = 0.33; greenOff.Ks[1] = 0.33; greenOff.Ks[2] = 0.33;
	greenOff.illum = 0; greenOff.d = 1.0;
	// Assign value to greenOn
	greenOn.Ka[0] = 0.0; greenOn.Ka[1] = 0.0; greenOn.Ka[2] = 0.0;
	greenOn.Kd[0] = 0.0; greenOn.Kd[1] = 1.0; greenOn.Kd[2] = 0.0;
	greenOn.Ks[0] = 0.33; greenOn.Ks[1] = 0.33; greenOn.Ks[2] = 0.33;
	greenOn.illum = 10; greenOn.d = 1.0;
}

void TrafficLight::setSignal(Signal signal)
{
	// You are supposed to assign the materials used in the ObjModel class based on
	// values of the input signal.
	switch (signal) {
	case Green:
		this->materials["_Yellow_"] = yellowOff;
		this->materials["_Red_"] = redOff;
		this->materials["_Green_"] = greenOn;
		break;
	case Yellow:
		this->materials["_Yellow_"] = yellowOn;
		this->materials["_Red_"] = redOff;
		this->materials["_Green_"] = greenOff;
		break;
	case Red:
		this->materials["_Yellow_"] = yellowOff;
		this->materials["_Red_"] = redOn;
		this->materials["_Green_"] = greenOff;
		break;
	}
}

void TrafficLight::ReadFile(string fileName)
{
	ObjModel::ReadFile(fileName);
	setMaterials();
}