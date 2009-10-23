//
//  Vector3D.cpp
//  OpenGLWidgets
//
//  Created by Filip Kunc on 6/21/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#include "MathDeclaration.h"

Vector3D::Vector3D()
{
	x = y = z = 0.0f;
}

Vector3D::Vector3D(const float * v)
{
	x = v[0];
	y = v[1];
	z = v[2];
}

Vector3D::Vector3D(const Vector3D & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector3D::Vector3D(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D::operator float *()
{
	return &x;
}

Vector3D::operator const float *() const
{
	return &x;
}

Vector3D & Vector3D::operator += (const Vector3D & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3D & Vector3D::operator -= (const Vector3D & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
	
Vector3D & Vector3D::operator *= (float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3D & Vector3D::operator /= (float s)
{
	float oos = 1.0f / s;
	x *= oos;
	y *= oos;
	z *= oos;
	return *this;
}

Vector3D Vector3D::operator + () const
{
	return Vector3D(x, y, z);
}

Vector3D Vector3D::operator - () const
{
	return Vector3D(-x, -y, -z);
}

Vector3D Vector3D::operator + (const Vector3D & v) const
{
	return Vector3D(x + v.x, y + v.y, z + v.z);
}
   
Vector3D Vector3D::operator - (const Vector3D & v) const
{
	return Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D Vector3D::operator * (float s) const
{
	return Vector3D(x * s, y * s, z * s);
}

Vector3D Vector3D::operator / (float s) const
{
	float oos = 1.0f / s;
	return Vector3D(x * oos, y * oos, z * oos);
}

Vector3D operator * (float s, const class Vector3D & v)
{
	return Vector3D(v.x * s, v.y * s, v.z * s);
}

bool Vector3D::operator == (const Vector3D & v) const
{
	return (v.x == x && v.y == y && v.z == z);
}

bool Vector3D::operator != (const Vector3D & v) const
{
	return (v.x != x || v.y != y || v.z != z);
}

float Vector3D::Dot(const Vector3D & v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3D Vector3D::Cross(const Vector3D & v) const
{
	/*
	i = (1,0,0)
	j = (0,1,0)
	k = (0,0,1)
				| i  j  k  |
	w = u x v =	| u1 u2 u3 | = i(u2v3 - u3v2) - j(u1v3 - v1u3) + k(u1v2 - v1u2)
				| v1 v2 v3 |

	w.x = u2v3 - u3v2
	w.y = u1v3 - v1u3
	w.z = u1v2 - v1u2
	*/
	Vector3D n;	
    n.x = ((y * v.z) - (z * v.y));
    n.y = ((z * v.x) - (x * v.z));
    n.z = ((x * v.y) - (y * v.x));
    return n;			
}
	
float Vector3D::GetLength() const
{
	return sqrtf(GetLengthSq());
}
	
float Vector3D::GetLengthSq() const
{
	return x * x + y * y + z * z;
}
	
void Vector3D::SetLength(float length)
{
	float s = length / GetLength();
	*this *= s;
}

void Vector3D::Normalize()
{
	SetLength(1.0f);
}
	
float Vector3D::Distance(const Vector3D & v) const
{
	Vector3D mag = v - *this;
	return mag.GetLength();
}

float Vector3D::SqDistance(const Vector3D & v) const
{
	Vector3D mag = v - *this;
	return mag.GetLengthSq();
}

Vector3D Vector3D::Lerp(float w, const Vector3D & v) const
{
	 return (*this * (1.0f - w) + v * w);
}

void Vector3D::Transform(const Matrix4x4 & m)
{
	Vector3D v(x,y,z);
         
	x = v.x * m[0] + v.y * m[4] + v.z * m[8] + m[12];
    y = v.x * m[1] + v.y * m[5] + v.z * m[9] + m[13];
    z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
}

void Vector3D::Transform(const Quaternion & q)
{
	Matrix4x4 m;
	q.ToMatrix(m);
	Transform(m);
}

float Vector3D::GetAngle(const Vector3D & v) const
{
	float dot = this->Dot(v);
	float len = this->GetLength() * v.GetLength();
	float angle = acosf(dot / len);

#ifdef WIN32
	if (_isnan(angle))
#else
	if (isnan(angle))
#endif
		return 0.0f;
	return angle;
}