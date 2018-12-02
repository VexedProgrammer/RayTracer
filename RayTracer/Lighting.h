#pragma once
#include "Vector3.h"

//Ambient
static float AmbientIntensity = 0.15f;
static Vector3<float> AmbientColour = Vector3<float>(1.f, 1.f, 1.f);


//All values between 0 and 1
struct Material
{
	Vector3<float> dColour = Vector3<float>(0.0f, 0.0f, 0.8f);
	Vector3<float> sColour = Vector3<float>(1.0f, 1.0f, 1.0f);
	float SpecInten = 0.5f;
	float Shininess = 32.0f;

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

	//Vector3<float> Position = Vector3<float>(35, 10, 75);// Vector3<float>(40.0f, 40.0f, 70.0f);
	Vector3<float> Position = Vector3<float>(100, 150, 70);
	Vector3<float> LightColour = Vector3<float>(1.0f, 1.0f, 1.0f); //White

	float Intensity = 1.0f;
	float Radius = 300;
};