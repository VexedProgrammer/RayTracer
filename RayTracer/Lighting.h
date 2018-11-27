#pragma once
#include "Vector3.h"

//Ambient
static float AmbientIntensity = 1.0f;
static Vector3<float> AmbientColour = Vector3<float>(0.33f, 0.33f, 0.33f);


//All values between 0 and 1
struct Material
{
	Vector3<float> dColour = Vector3<float>(0.0f, 0.0f, 0.8f);
	Vector3<float> sColour = Vector3<float>(1.0f, 1.0f, 1.0f);
	float Shininess = 16.0f;

	Material() {};
	Material(Vector3<float> kd, Vector3<float> ks, float shine)
	{
		dColour = kd;
		sColour = ks;
		Shininess = shine;
	}

};
class PointLight
{
public:
	PointLight() {};
	~PointLight() {};

	Vector3<float> Position = Vector3<float>(35, 10, 75);// Vector3<float>(40.0f, 40.0f, 70.0f);
	//Vector3<float> Position = Vector3<float>(11, 40, 15);
	Vector3<float> LightColour = Vector3<float>(1.0f, 1.0f, 1.0f); //White

	float Intensity = 1.0f;
	float Radius = 150;
};