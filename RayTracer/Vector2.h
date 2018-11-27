#pragma once
#include <cmath>
template<typename T>
/*! Class Vector2
	2D Vector Class that holds 2 variables (x, y) and has all the relevent functions and overloaders
*/
class Vector2
{
public:
	//Default Contructor
    Vector2() {};
	/*! Contructor Vector2
		Sets inital values
		\param x, y template type T, inital x and y values
	*/
    Vector2(T x, T y) : x(x), y(y) {};
    ~Vector2(){};

	//Variables
    T x, y;

	/*! Magnitude function
		returns the magniutde of the vector2
	*/
    float Magnitude() const {  return sqrt(pow(x,2) + pow(y,2)); };
	/*! Dot function
		returns the dor product of this and another vector
		\param Vector2 ref other, other vector to get dot product
	*/
	float Dot(const Vector2& other) const { return x * other.x + y * other.y; };

    //Operator Overloaders
    bool operator==(const Vector2& other) const { return (x == other.x && y == other.y); }
    bool operator!=(const Vector2& other) const { return (x != other.x || y != other.y); }
    Vector2& operator=(const Vector2& other) { x = other.x; y = other.y; return *this; }

    const Vector2 operator + (const Vector2 & other) const { return Vector2(x + other.x, y + other.y); }
    const Vector2 operator - (const Vector2 & other) const { return Vector2(x - other.x, y - other.y); }
    const Vector2 operator * (const Vector2 & other) const { return Vector2(x * other.x, y * other.y); }
    const Vector2 operator / (const Vector2 & other) const { return Vector2(x / other.x, y / other.y); }

    Vector2 & operator += (const Vector2 & other) { x += other.x; y += other.y; return *this; }
    Vector2 & operator -= (const Vector2 & other) { x -= other.x; y -= other.y; return *this; }
    Vector2 & operator *= (const Vector2 & other) { x *= other.x; y *= other.y; return *this; }
    Vector2 & operator /= (const Vector2 & other) { x /= other.x; y /= other.y; return *this; }

    const Vector2 operator + (T value) const { return Vector2(x + value, y + value); }
    const Vector2 operator - (T value) const { return Vector2(x - value, y - value); }
    const Vector2 operator * (T value) const { return Vector2(x * value, y * value); }
    const Vector2 operator / (T value) const { return Vector2(x / value, y / value); }

    Vector2 & operator += (T value) { x += value; y += value; return *this; }
    Vector2 & operator -= (T value) { x -= value; y -= value; return *this; }
    Vector2 & operator *= (T value) { x *= value; y *= value; return *this; }
    Vector2 & operator /= (T value) { x /= value; y /= value; return *this; }


};
