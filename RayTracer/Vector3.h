#pragma once
#include <cmath>
template<typename T>
/*! Class Vector3
	3D Vector Class that holds 3 variables (x, y, z) and has all the relevent functions and overloaders
*/
class Vector3
{
public:
	//Default Contructor
	Vector3() {};
	/*! Contructor Vector3
		Sets inital values
		\param x, y, z template type T, inital x, y and z values
	*/
	Vector3(T x, T y, T z) : x(x), y(y), z(z) {};
    ~Vector3(){};

	//Variables
    T x, y, z;

	/*! Magnitude function
		returns the magniutde of the Vector3
	*/
    float Magnitude() const {  return sqrt(pow(x,2) + pow(y,2) + pow(z,2)); };
	/*! Dot function
		returns the dot product of this and another vector
		\param Vector3 ref other, other vector to get dot product
	*/
	float Dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; };
	Vector3 Cross(const Vector3& other) const { return Vector3(y*other.z - z * other.y, z*other.x - x * other.z, x * other.y - y * other.x); }

    //Operator Overloaders
    bool operator==(const Vector3& other) const { return (x == other.x && y == other.y && z = other.z); }
    bool operator!=(const Vector3& other) const { return (x != other.x || y != other.y || z != other.z); }
	Vector3& operator=(const Vector3& other) { x = other.x; y = other.y; z = other.z; return *this; }
	const Vector3 operator-() { return Vector3(-x, -y, -z); }

    const Vector3 operator + (const Vector3 & other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
	const Vector3 operator - (const Vector3 & other) const { return Vector3(x - other.x, y - other.y, z - other.z ); }
    const Vector3 operator * (const Vector3 & other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
    const Vector3 operator / (const Vector3 & other) const { return Vector3(x / other.x, y / other.y, z / other.z); }

	Vector3 & operator += (const Vector3 & other) { x += other.x; y += other.y; z += other.z; return *this; }
	Vector3 & operator -= (const Vector3 & other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
	Vector3 & operator *= (const Vector3 & other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
	Vector3 & operator /= (const Vector3 & other) { x /= other.x; y /= other.y; z /= other.z; return *this; }

    const Vector3 operator + (T value) const { return Vector3(x + value, y + value, z + value); }
    const Vector3 operator - (T value) const { return Vector3(x - value, y - value, z - value); }
    const Vector3 operator * (T value) const { return Vector3(x * value, y * value, z * value); }
    const Vector3 operator / (T value) const { return Vector3(x / value, y / value, z / value); }

	Vector3 & operator += (T value) { x += value; y += value; z += value; return *this; }
	Vector3 & operator -= (T value) { x -= value; y -= value; z -= value; return *this; }
	Vector3 & operator *= (T value) { x *= value; y *= value; z *= value; return *this; }
	Vector3 & operator /= (T value) { x /= value; y /= value; z /= value; return *this; }


};
