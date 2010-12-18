//
//  Vector2D.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#include "MathDeclaration.h"

Vector2D::Vector2D()
{
	x = y = 0.0f;
}

Vector2D::Vector2D(const float * v)
{
	x = v[0];
	y = v[1];
}

Vector2D::Vector2D(const Vector2D & v)
{
	x = v.x;
	y = v.y;
}

Vector2D::Vector2D(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector2D::operator float *()
{
	return &x;
}

Vector2D::operator const float *() const
{
	return &x;
}

Vector2D & Vector2D::operator += (const Vector2D &v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2D & Vector2D::operator -= (const Vector2D & v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}
	
Vector2D & Vector2D::operator *= (float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vector2D & Vector2D::operator /= (float s)
{
	float oos = 1.0f / s;
	x *= oos;
	y *= oos;
	return *this;
}

Vector2D Vector2D::operator + () const
{
	return Vector2D(x, y);
}

Vector2D Vector2D::operator - () const
{
	return Vector2D(-x, -y);
}

Vector2D Vector2D::operator + (const Vector2D & v) const
{
	return Vector2D(x + v.x, y + v.y);
}
   
Vector2D Vector2D::operator - (const Vector2D & v) const
{
	return Vector2D(x - v.x, y - v.y);
}

Vector2D Vector2D::operator * (float s) const
{
	return Vector2D(x * s, y * s);
}

Vector2D Vector2D::operator / (float s) const
{
	float oos = 1.0f / s;
	return Vector2D(x * oos, y * oos);
}

Vector2D operator * (float s, const class Vector2D & v)
{
	return Vector2D(v.x * s, v.y * s);
}

bool Vector2D::operator == (const Vector2D & v) const
{
	return (v.x == x && v.y == y);
}

bool Vector2D::operator != (const Vector2D & v) const
{
	return (v.x != x || v.y != y);
}

float Vector2D::Dot(const Vector2D & v) const
{
	return x * v.x + y * v.y;
}
	
float Vector2D::GetLength() const
{
	return sqrtf(GetLengthSq());
}
	
float Vector2D::GetLengthSq() const
{
	return x * x + y * y;
}
	
void Vector2D::SetLength(float length)
{
	float s = length / GetLength();
	*this *= s;
}

void Vector2D::Normalize()
{
	SetLength(1.0f);
}
	
float Vector2D::Distance(const Vector2D & v) const
{
	Vector2D mag = v - *this;
	return mag.GetLength();
}

float Vector2D::SqDistance(const Vector2D & v) const
{
	Vector2D mag = v - *this;
	return mag.GetLengthSq();
}

Vector2D Vector2D::Lerp(float w, const Vector2D & v) const
{
	 return (*this * (1.0f - w) + v * w);
}
