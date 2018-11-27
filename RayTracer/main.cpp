#include <iostream>
#include "Vector3.h"
#include "Vector2.h"
#include "Lighting.h"
using namespace std;

struct Eye
{
	Vector3<float> position = Vector3<float>(0, 0, 0);
	Vector3<float> forward = Vector3<float>(0, 0, 1);
	Vector3<float> up = Vector3<float>(0, 1, 0);
	Vector3<float> right = Vector3<float>(1, 0, 0);

	float FOV = 90; // Degrees
};
struct Ray
{
	Vector3<float> origin;
	Vector3<float> direction;
	float depth;

	Ray(Vector3<float> origin, Vector3<float> dir, float depth)
	{
		this->origin = origin;
		this->direction = dir;
		this->depth = depth;
	}
};
/*! Struct PPM
Holds all data to do with a .ppm file include:
	-File type (i.e. P3)
	-Texture Width
	-Texture Height
	-Teture colour component (i.e. 255)
	-2D Array of all pixel colours
*/
struct PPM
{
	string fileType;
	unsigned int width, height, colourComp;
	Vector3<int>** values;
	PPM() {};
	PPM(string fileType, unsigned int width, unsigned int height, unsigned int colourComp)
	{
		this->fileType = fileType;
		this->width = width;
		this->height = height;
		this->colourComp = colourComp;

		//Set up the 2D array for the PPM depending on header infomation (width, height)
		values = new Vector3<int>*[height];
		for (int i = 0; i < height; ++i)
			values[i] = new Vector3<int>[width];
	}
};
struct Triangle
{
	Vector3<float> A = Vector3<float>(10, 20, 100);
	Vector3<float> B = Vector3<float>(100, 30, 150);
	Vector3<float> C = Vector3<float>(40, 100, 200);

	Vector3<float> Ac = Vector3<float>(255, 0, 0);
	Vector3<float> Bc = Vector3<float>(0, 255, 0);
	Vector3<float> Cc = Vector3<float>(0, 0, 255);

	Vector2<float> Auv = Vector2<float>(0.5f, 1.0f);
	Vector2<float> Buv = Vector2<float>(0.6f, 0.5f);
	Vector2<float> Cuv = Vector2<float>(0.44434f, 0.87186f);

	Material Mat = Material();

	Triangle() {};
	Triangle(Vector3<float> vertA, Vector3<float> vertB, Vector3<float> vertC)
	{
		A = vertA;
		B = vertB;
		C = vertC;
	}
};
//Function Declarations
int RenderImage(const char* fileName, PPM ppm);
double deg2rad(double degrees);
bool RayTriangleIntersect(Ray ray, Triangle tri, Vector3<float>& hitPoint, Vector3<float>& outNorm, Vector3<float>& outBary);
//Variables
Vector2<int> imageSize = Vector2<int>(512, 512);
Vector3<int> imageColour = Vector3<int>(255, 255, 192);

float clampFloat(float N, float minN, float maxN)
{
	if (N > maxN) N = maxN;
	else if (N < minN) N = minN;

	return N;
}
int clampInt(int N, int minN, int maxN)
{
	if (N > maxN) N = maxN;
	else if (N < minN) N = minN;

	return N;
}




int main()
{
	cout << "Hello RayTracer!" << endl;

	Eye camera;
	Triangle test;
	PointLight light;
	PPM image = PPM("P3", imageSize.x, imageSize.y, 255);
	float aspect = imageSize.x / imageSize.y;

	for (unsigned int j = 0; j < imageSize.y; ++j)
	{
		for (unsigned int i = 0; i < imageSize.x; ++i)
		{

			Vector3<float> viewDir = (camera.forward);// -camera.position);
			Vector3<float> U = -viewDir.Cross(camera.up);
			Vector3<float> V = U.Cross(viewDir);
			float viewWidth = tan(deg2rad(camera.FOV /2));
			float viewHeight = aspect * viewWidth;
			
			Vector3<float> viewCorner = camera.position + (camera.forward - V * viewHeight - U * viewWidth);

			Vector3<float> xVec = (U * 2 * viewWidth) / imageSize.x; //  U * (2 * (i / imageSize.x))*viewWidth;//
			Vector3<float> yVec = (V * 2 * viewHeight) / imageSize.y; //  V * (2 * (j / imageSize.y))*viewHeight;//
			Vector3<float> viewPoint = viewCorner + (xVec * i) + (yVec * j);
			Ray ray = Ray(camera.position, (viewPoint - camera.position), 100.0f);


			Vector3<float> bary = Vector3<float>(0, 0, 0);
			Vector3<float> hitPoint = Vector3<float>(0, 0, 0);
			Vector3<float> triNorm = Vector3<float>(0, 0, 0);
			if (RayTriangleIntersect(ray, test, hitPoint, triNorm, bary))
			{
				Vector3<float> colour = AmbientColour * AmbientIntensity;// Vector3<float>(test.Ac * bary.x + test.Bc * bary.y + test.Cc * bary.z);
				Vector3<float> lightVec = (light.Position - hitPoint);
				float dist = lightVec.Magnitude();
				lightVec = lightVec / dist;

				float lambertian = lightVec.Dot(triNorm);
				lambertian = clampFloat(lambertian, 0, lambertian);
				double spec = 0.0;

				if (lambertian > 0.0f)
				{
					Vector3<float> lightVec = (hitPoint - camera.position);

					Vector3<float> halfVec = (light.Position - lightVec)/2;
					halfVec = halfVec / halfVec.Magnitude();
					float specAngle = halfVec.Dot(triNorm);
					specAngle = clampFloat(specAngle, 0, specAngle);
					spec = pow(specAngle, test.Mat.Shininess);
				}

				Vector3<float> Ambient = (AmbientColour*AmbientIntensity);
				Vector3<float> Diffuse = (test.Mat.dColour * lambertian * light.LightColour * light.Intensity);
				Vector3<float> Specular = (test.Mat.sColour * spec * light.LightColour * light.Intensity);

				if (spec > 0.000001)
					cout << "yay" << endl;

				colour = Ambient + Diffuse + Specular;

				colour *= 255;
				
				image.values[j][i].x = clampInt(colour.x, 0, 255);
				image.values[j][i].y = clampInt(colour.y, 0, 255);
				image.values[j][i].z = clampInt(colour.z, 0, 255);
			}
			else
			{
				image.values[j][i] = imageColour;
			}
		}
	}
	
	RenderImage("test.ppm", image);

	//system("pause");
	return 0;
}
double deg2rad(double degrees) {
	return degrees * 3.14159265359 / 180.0;
}
int RenderImage(const char* fileName, PPM ppm)
{

	int i, j;
	FILE *fp = fopen(fileName, "w");
	(void)fprintf(fp, "P3\n%d %d\n255\n", imageSize.x, imageSize.x);
	for (j = 0; j < imageSize.y; ++j)
	{
		for (i = 0; i < imageSize.x; ++i)
		{
			int color[3];
			color[0] = ppm.values[j][i].x;
			color[1] = ppm.values[j][i].y;
			color[2] = ppm.values[j][i].z;
			fprintf(fp, "%d %d %d\n", color[0], color[1], color[2]);
		}
	}
	(void)fclose(fp);
	return 0;
}

bool RayTriangleIntersect(Ray ray, Triangle tri, Vector3<float>& hitPoint, Vector3<float>& outNorm, Vector3<float>& outBary)
{
	float eps = 0.000001f;

	Vector3<float> U = tri.B - tri.A;
	Vector3<float> V = tri.C - tri.A;
	U = U / U.Magnitude();
	Vector3<float> N = U.Cross(V);

	N = N / N.Magnitude();
	outNorm = N;
	Vector3<float> W = N.Cross(U);
	float test = U.Dot(W);

	Vector3<float> sPrime = Vector3<float>(ray.origin - tri.A);
	Vector3<float> lPrime = Vector3<float>(ray.direction.Dot(U), ray.direction.Dot(W), ray.direction.Dot(N));

	float sn = sPrime.Dot(N);
	float t = sn / lPrime.z;
	Vector3<float> o = ray.origin + ray.direction*fabs(t);

	o = o - tri.A;
	hitPoint = o;

	Vector3<float> A = tri.A - tri.A;
	Vector3<float> B = tri.B - tri.A;
	Vector3<float> C = tri.C - tri.A;


	Vector3<float> AC = Vector3<float>(C - A); //Vector for line AC
	Vector3<float> BA = Vector3<float>(A - B); //Vector for line BA

	Vector3<float> BCNormal = Vector3<float>((C.y - B.y), -(C.x - B.x), 0); //Get the normal vector of line BC
	float distABC = ((BCNormal.x * AC.x) + (BCNormal.y*AC.y)) / BCNormal.Magnitude(); //Distance from vertex A to line BC
	Vector3<float> pixelBC = (C - o); //Pixel to line vector
	float distPixelBC = ((BCNormal.x * pixelBC.x) + (BCNormal.y * pixelBC.y)) / BCNormal.Magnitude(); //Distance from pixel to line BC
	float alpha = distPixelBC / distABC; //Divide to get distance between 0 and 1

	Vector3<float> CANormal = Vector3<float>((A.y - C.y), -(A.x - C.x), 0); //Get the normal vector of line CA
	float distBCA = ((CANormal.x * BA.x) + (CANormal.y*BA.y)) / CANormal.Magnitude(); //Distance from vertex B to line CA
	Vector3<float> pixelCA = (A - o); //Pixel to line vector
	float distPixelCA = ((CANormal.x * pixelCA.x) + (CANormal.y * pixelCA.y)) / CANormal.Magnitude(); //Distance from pixel to line CA
	float beta = distPixelCA / distBCA; //Divide to get distance between 0 and 1

	float gamma = 1.0 - alpha - beta; //Get gamma knowing that a+b+y = 1

	outBary = Vector3<float>(alpha, beta, gamma);

	if (alpha > 1.0f || beta > 1.0f || gamma > 1.0f || alpha < 0.0f || beta < 0.0f || gamma < 0.0f) return false;

	return true;
}

