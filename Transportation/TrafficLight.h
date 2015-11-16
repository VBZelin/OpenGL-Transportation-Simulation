#pragma once
#include "objmodel.h"

enum Signal {Green, Yellow, Red};

class TrafficLight :
	public ObjModel
{
public:
	TrafficLight(void);
	~TrafficLight(void);
	virtual void ReadFile(string fileName);
	void setMaterials();
	void setSignal(Signal signal);

private:
	Material redOn, redOff, yellowOn, yellowOff, greenOn, greenOff;
};

