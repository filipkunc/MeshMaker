//
//  Vector4D.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/23/12.
//  For license see LICENSE.TXT
//

#include "MathDeclaration.h"

Vector4D::Vector4D()
{
	x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
}

Vector4D::Vector4D(const float * v)
{
	x = v[0];
	y = v[1];
	z = v[2];
    w = v[3];
}

Vector4D::Vector4D(const Vector3D & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
    w = 1.0f;
}

Vector4D::Vector4D(const Vector4D & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
    w = v.w;
}

Vector4D::Vector4D(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
    this->w = w;
}

Vector4D::operator float *()
{
	return &x;
}

Vector4D::operator const float *() const
{
	return &x;
}

Vector4D & Vector4D::operator += (const Vector4D & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
    w += v.w;
	return *this;
}

Vector4D & Vector4D::operator -= (const Vector4D & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
    w -= v.w;
	return *this;
}

Vector4D & Vector4D::operator *= (float s)
{
	x *= s;
	y *= s;
	z *= s;
    w *= s;
	return *this;
}

Vector4D & Vector4D::operator /= (float s)
{
	float oos = 1.0f / s;
	x *= oos;
	y *= oos;
	z *= oos;
    w *= oos;
	return *this;
}

Vector4D Vector4D::operator + () const
{
	return Vector4D(x, y, z, w);
}

Vector4D Vector4D::operator - () const
{
	return Vector4D(-x, -y, -z, -w);
}

Vector4D Vector4D::operator + (const Vector4D & v) const
{
	return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4D Vector4D::operator - (const Vector4D & v) const
{
	return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4D Vector4D::operator * (float s) const
{
	return Vector4D(x * s, y * s, z * s, w * s);
}

Vector4D Vector4D::operator / (float s) const
{
	float oos = 1.0f / s;
	return Vector4D(x * oos, y * oos, z * oos, w * oos);
}

Vector4D operator * (float s, const class Vector4D & v)
{
	return Vector4D(v.x * s, v.y * s, v.z * s, v.w * s);
}

bool Vector4D::operator == (const Vector4D & v) const
{
	return (v.x == x && v.y == y && v.z == z && v.w == w);
}

bool Vector4D::operator != (const Vector4D & v) const
{
	return (v.x != x || v.y != y || v.z != z || v.w != w);
}

float Vector4D::Dot(const Vector4D & v) const
{
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

float Vector4D::GetLength() const
{
	return sqrtf(GetLengthSq());
}

float Vector4D::GetLengthSq() const
{
	return x * x + y * y + z * z + w * w;
}

void Vector4D::SetLength(float length)
{
	float s = length / GetLength();
	*this *= s;
}

void Vector4D::Normalize()
{
	SetLength(1.0f);
}