#include <iostream>
#include "Vector3.h"
#include "Vector2.h"
#include "Lighting.h"
#include <vector>
using namespace std;

/*! Struct Eye
	Holds all data to do with the eye (camera)
	-Position
	-Forward Vector
	-Up Vector
	-Field of View
*/
struct Eye
{
	Vector3<float> position = Vector3<float>(0, 0, 0);
	Vector3<float> forward = Vector3<float>(0, 0, 1);
	Vector3<float> up = Vector3<float>(0, 1, 0);

	float FOV = 90; // Degrees
}camera;
/*! Struct Ray
Holds all data to do with each ray
	-Origin
	-Direction
*/
struct Ray
{
	Vector3<float> origin;
	Vector3<float> direction;

	Ray(Vector3<float> origin, Vector3<float> dir)
	{
		this->origin = origin;
		this->direction = dir;
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
/*! Struct Triangle
	Holds all data to do with each Triangle
	-Vertex Positions
	-Vertex Colours
	-UV Coords
	-Material
*/
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
Vector3<float> GetBaryCoords(Triangle& tri, Vector3<float>& o);
Ray CreateCameraRay(int i, int j);
void CreateImage(const char* fileName);
//Variables
Vector2<int> imageSize = Vector2<int>(128, 128);
Vector3<int> imageColour = Vector3<int>(255, 255, 192);
vector<Triangle> Triangles;


/*! Clamp Float function
	Clamps a floatinf point number between 2 numbers

	\param float N, original number
	\param float minN, minimum value
	\param float maxN, maximum value
*/
float clampFloat(float N, float minN, float maxN)
{
	if (N > maxN) N = maxN;
	else if (N < minN) N = minN;

	return N;
}
/*! Clamp Int function
	Clamps an integer between 2 numbers

	\param float N, original number
	\param float minN, minimum value
	\param float maxN, maximum value
	*/
int clampInt(int N, int minN, int maxN)
{
	if (N > maxN) N = maxN;
	else if (N < minN) N = minN;

	return N;
}

//Check bools for prints each image
bool bColour = false;
bool bAmbient = false;
bool bDiffuse = false;
bool bSpec = false;
bool bShadows = false;


int main()
{
	cout << "Working..." << endl;

	//Set up main triangle
	Triangles.push_back(Triangle());
	//Set up Floor triangles
	Vector3<float> A = Vector3<float>(-1000, -100.f, 0);
	Vector3<float> B = Vector3<float>(1000, -100.f, 0);
	Vector3<float> C = Vector3<float>(-1000, -100.f, 1000);
	Vector3<float> D = Vector3<float>(1000, -100.f, 1000);
	Triangle left = Triangle(A, D, C);
	left.Mat.SpecInten = 0.0f;
	left.Mat.dColour = Vector3<float>(0.8f, 0.0f, 0.0f);
	Triangle right = Triangle(A, B, D);
	right.Mat.SpecInten = 0.0f;
	right.Mat.dColour = Vector3<float>(0.8f, 0.0f, 0.0f);
	
	//Setup and print images
	bColour = true;
	CreateImage("BaryColour.ppm");
	bColour = false;
	bAmbient = true;
	CreateImage("Ambient.ppm");
	bAmbient = false;
	bDiffuse = true;
	CreateImage("Diffuse.ppm");
	bDiffuse = false;
	bSpec = true;
	CreateImage("Specular.ppm");
	bDiffuse = true;
	bAmbient = true;
	CreateImage("Blinn.ppm");
	bShadows = true;
	Triangles.push_back(left);
	Triangles.push_back(right);
	CreateImage("Shadows.ppm");

	return 0;
}
/*! Create Image function
	Casts a ray for each pixel.
	Checks for shadows.
	Computes lighting.
	Creates and sets up all the colours for each pixel.

	\param const char pointer fileName, the name of the output image file
*/
void CreateImage(const char* fileName)
{
	PointLight light; //Default point light
	PPM image = PPM("P3", imageSize.x, imageSize.y, 255); //Set up PPM image with header info.
	
	//Loop through all the pixels
	for (unsigned int j = 0; j < imageSize.y; ++j)
	{
		for (unsigned int i = 0; i < imageSize.x; ++i)
		{
			//Distance check (only want to draw the point closest to the camera
			double fragDist = 9999999; //Set max dist
			Triangle* tri = nullptr; //Set a point for the selected triangle

			//Three vectors to store info of closest point
			Vector3<float> closeBary = Vector3<float>(0, 0, 0);
			Vector3<float> closeHitPoint = Vector3<float>(0, 0, 0);
			Vector3<float> closeTriNorm = Vector3<float>(0, 0, 0);
			for (unsigned int t = 0; t < Triangles.size(); t++)
			{
				//Three vectors to store info of current point
				Vector3<float> bary = Vector3<float>(0, 0, 0);
				Vector3<float> hitPoint = Vector3<float>(0, 0, 0);
				Vector3<float> triNorm = Vector3<float>(0, 0, 0);
				//Cast the ray and do the intesection test
				if (RayTriangleIntersect(CreateCameraRay(i, j), Triangles[t], hitPoint, triNorm, bary))
				{
					//Get the vector between the camera and the hit point
					Vector3<float> hitVec = hitPoint - camera.position;
					float dist = hitVec.Magnitude(); //Get the distance
					if (dist < fragDist) //Check if it is closer than the last
					{
						//Set the info for the closest point
						fragDist = dist;
						closeBary = bary;
						closeHitPoint = hitPoint;
						closeTriNorm = triNorm;
						tri = &Triangles[t];
					}
				}
			}

			//If the ray hits a triangle
			if (tri != nullptr)
			{
				Vector3<float> Ambient = (AmbientColour*AmbientIntensity); //Set ambient colour
				Vector3<float> colour = Vector3<float>(0, 0, 0); //Set the default colours

				if(bAmbient)
					colour = Ambient; //Set ambient colour as default colour
				else if(bColour)
					colour = Vector3<float>(tri->Ac * closeBary.x + tri->Bc * closeBary.y + tri->Cc * closeBary.z)/255; //Set the bary centric colours as default colour

				Vector3<float> lightVec = closeHitPoint - light.Position; //Get the vector between the light and the point
				float dist = lightVec.Magnitude(); //Get the distance
				lightVec = lightVec / dist; //Normalize the light vector

				//Shadows
				bool shadow = false;

				if (bShadows) //If shadows are enabled
				{
					Vector3<float> shadowVec = (light.Position - closeHitPoint); //Get the vector between the point and the light
					shadowVec = shadowVec / shadowVec.Magnitude(); //Normalize shadow vector
					Ray shadowRay = Ray(closeHitPoint, shadowVec); //Create the shadow ray

					for (unsigned int t = 0; t < Triangles.size(); t++) //Loop through other triangles
					{
						if (tri == &Triangles[t]) continue;  //Don't check the same triangle

						Vector3<float> temp = Vector3<float>(0, 0, 0);
						if (RayTriangleIntersect(shadowRay, Triangles[t], temp, temp, temp)) //Check the for intersections
						{
							shadow = true; //If the shadow ray hits another triangles then it is in shadow!
							break; //Exit the loop
						}
					}
				}
				if (!shadow) //Don't do any other lighting if it is in shadow
				{
					//Calculate the diffuse value
					double lambertian = lightVec.Dot(closeTriNorm);
					lambertian = clampFloat(lambertian, 0, lambertian);
					//Set up default specular value
					double spec = 0.0;

					if (lambertian > 0.0f && bSpec) //If specular is enabled and there is lighting hitting this point calculate the specular value
					{
						Vector3<float> halfVec = (light.Position - closeHitPoint) / 2; //Get the half vector for Blinn-Phong specular
						halfVec = halfVec / halfVec.Magnitude(); //Normalize 
						float specAngle = halfVec.Dot(-closeTriNorm); //Get the angle between the surface normal and the half vector
						specAngle = clampFloat(specAngle, 0, specAngle); //Make sure this is not a minus value
						spec = pow(specAngle, tri->Mat.Shininess); //Calculate specular highlightwith coefficient
					}


					Vector3<float> Diffuse = (tri->Mat.dColour * lambertian * light.LightColour * light.Intensity); //Calculate diffuse lighting
					Vector3<float> Specular = (tri->Mat.sColour * spec * light.LightColour * light.Intensity) * tri->Mat.SpecInten; //calculate specular lighting
					
					if (bDiffuse) {
						colour += Diffuse; //Add defuse
					}

					colour += Specular; //Add specular
				}
				colour *= 255; //Multiply lighting up

				image.values[j][i].x = clampInt(colour.x, 0, 255);
				image.values[j][i].y = clampInt(colour.y, 0, 255);
				image.values[j][i].z = clampInt(colour.z, 0, 255);
			}
			else
			{
				image.values[j][i] = imageColour; //If no collition just use background colour
			}
		}
	}

	RenderImage(fileName, image); //Print the image
}

/*! Create Camera Ray Function
	Creates and returns a ray from the camera through a given pixel
	\param int i, pixel x coord
	\param int j, pixel y coord
*/
Ray CreateCameraRay(int i, int j)
{
	float aspect = imageSize.x / imageSize.y; //Get aspect ratio (1 for square image)
	Vector3<float> viewDir = camera.forward; //Set camera direction
	Vector3<float> U = viewDir.Cross(camera.up); //Calculate local coordanate base
	Vector3<float> V = U.Cross(viewDir);
	float viewWidth = tan(deg2rad(camera.FOV / 2)); //Calculate the normalized image width
	float viewHeight = aspect * viewWidth; //Calculate the normalized image height

	Vector3<float> viewCorner = camera.forward + V * viewHeight + U * viewWidth; //Get the top corner to use as a start point

	Vector3<float> xVec = (U * 2 * viewWidth) / imageSize.x;  // Calculate pizel offset for X 
	Vector3<float> yVec = (V * 2 * viewHeight) / imageSize.y; // Calculate pizel offset for Y 

	//Set the start point to be in the center of the pixel
	viewCorner -= xVec/2;
	viewCorner -= yVec/2;

	Vector3<float> viewPoint = viewCorner - (xVec * i) - (yVec * j); //Give current pixel offset

	Ray ray = Ray(camera.position, viewPoint - camera.position); //Create Ray

	return ray; //Return ray
}
/*! Degree to radians function
	Converts a angle in degrees to radians

	\param double degrees, original angle
*/
double deg2rad(double degrees) {
	return degrees * 3.14159265359 / 180.0;
}

/*! Render Image function
	Converts a angle in degrees to radians
	\param const char pointer fileName, the name of the output image file
	\param PPM ppm, the image data to print
*/
int RenderImage(const char* fileName, PPM ppm)
{

	int i, j;
	FILE *fp = fopen(fileName, "w"); //Set up a file to write too
	(void)fprintf(fp, "P3\n%d %d\n255\n", imageSize.x, imageSize.x); //Print the header to the file

	//Loop through all pixels
	for (j = 0; j < imageSize.y; ++j)
	{
		for (i = 0; i < imageSize.x; ++i)
		{
			//Set each colour
			int color[3];
			color[0] = ppm.values[j][i].x;
			color[1] = ppm.values[j][i].y;
			color[2] = ppm.values[j][i].z;
			fprintf(fp, "%d %d %d\n", color[0], color[1], color[2]); //Print the RGB values to the file
		}
	}
	(void)fclose(fp);
	return 0;
}
Vector3<float> NTest;

/*! Ray Triangle Intersect function
	Checks to see if a ray interects with a triangle.
	\param Ray ray, the casted ray
	\param Triangle tri, the triangle to check against
	\param Vector3 ref hitPoint, the 3D space point where the ray intesects
	\param Vector3 ref hitPoint, the normal of the hit triangle
	\param Vector3 ref hitPoint, the bary centric values for the hit point
*/
bool RayTriangleIntersect(Ray ray, Triangle tri, Vector3<float>& hitPoint, Vector3<float>& outNorm, Vector3<float>& outBary)
{

	//Create 2 edges
	Vector3<float> U = tri.B - tri.A;
	Vector3<float> V = tri.C - tri.A;
	U = U / U.Magnitude();
	//Cross the U and the V to get the normal
	Vector3<float> N = U.Cross(V);
	N = N / N.Magnitude(); //noralize normal
	outNorm = N; //Pass out normal
	NTest = N;
	Vector3<float> W = N.Cross(U); //Get the right vector by crossing N and U

	Vector3<float> sPrime = Vector3<float>(ray.origin - tri.A); //Get vector to a known point on the plane in PCS
	Vector3<float> lPrime = Vector3<float>(ray.direction.Dot(U), ray.direction.Dot(W), ray.direction.Dot(N)); //Move the ray into PCS

	float sn = sPrime.Dot(N); //Get the n value for sPrime
	float t = sn / lPrime.z; //Devide to get t
	Vector3<float> o = ray.origin + ray.direction*abs(t); //Get the point of intersections in 3D space
	hitPoint = o; //Pass out hit point
	o = o - tri.A; //Move hit point into PCS


	//Get all triangle vertecies in PCS
	Vector3<float> A = tri.A - tri.A;
	Vector3<float> B = tri.B - tri.A;
	Vector3<float> C = tri.C - tri.A;

	//Test to see if we are on the plane
	float check = (int)A.Dot(NTest);
	float check2 = (int)B.Dot(NTest);
	float check3 = (int)C.Dot(NTest);
	float check4 = (int)o.Dot(NTest);
	if (check != 0 || check2 != 0 || check3 != 0 || check4 != 0)
		return false;

	outBary = GetBaryCoords(tri, o); //Get the bary coords

	if (outBary.x > 1.0f || outBary.y > 1.0f || outBary.z > 1.0f || outBary.x < 0.0f || outBary.y < 0.0f || outBary.z < 0.0f) //check for valid bary coords
		return false;

	return true;
}
/*! Get Bary Coords function
	Calculates and returns the bary centric coordanates for a point in a triangle
	\param Tri tri, the current triangle
	\param Vector3 o, the hit point in PCS
*/
Vector3<float> GetBaryCoords(Triangle& tri, Vector3<float>& o)
{
	//Get all triangle vertecies in PCS
	Vector3<float> A = tri.A - tri.A; 
	Vector3<float> B = tri.B - tri.A;
	Vector3<float> C = tri.C - tri.A;

	Vector3<float> AC = Vector3<float>(C - A); //Vector for line AC 
	Vector3<float> BA = Vector3<float>(A - B); //Vector for line BA

	Vector3<float> BCNormal = Vector3<float>((C.z - B.z), -(C.x - B.x), 0); //Get the normal vector of line BC 
	float distABC = ((BCNormal.x * AC.x) + (BCNormal.y*AC.z)) / BCNormal.Magnitude(); //Distance from vertex A to line BC  
	Vector3<float> pointBC = (C - o); //Point to line vector
	float distPixelBC = ((BCNormal.x * pointBC.x) + (BCNormal.y * pointBC.z)) / BCNormal.Magnitude(); //Distance from pixel to line BC
	float alpha = distPixelBC / distABC; //Divide to get distance between 0 and 1 

	Vector3<float> CANormal = Vector3<float>((A.z - C.z), -(A.x - C.x), 0); //Get the normal vector of line CA
	float distBCA = ((CANormal.x * BA.x) + (CANormal.y*BA.z)) / CANormal.Magnitude(); //Distance from vertex B to line CA
	Vector3<float> pointCA = (A - o); //Point to line vector
	float distPixelCA = ((CANormal.x * pointCA.x) + (CANormal.y * pointCA.z)) / CANormal.Magnitude(); //Distance from pixel to line CA
	float beta = distPixelCA / distBCA; //Divide to get distance between 0 and 1

	float gamma = 1.0 - alpha - beta; //Get gamma knowing that a+b+y = 1

	return Vector3<float>(alpha, beta, gamma); //Return values
}

