/*
 *  Vector3D.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 6/21/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "MathForwardDeclaration.h"

class Vector3D
{
public:
	float x, y, z;

	Vector3D();
	Vector3D(const float * v);
	Vector3D(const Vector3D & v);
	Vector3D(float x, float y, float z);

	operator float * ();
	operator const float * () const;

	Vector3D & operator += (const Vector3D & v);
	Vector3D & operator -= (const Vector3D & v);
	Vector3D & operator *= (float s);
    Vector3D & operator /= (float s);

	Vector3D operator + () const;
    Vector3D operator - () const;

    Vector3D operator + (const Vector3D & v) const;
    Vector3D operator - (const Vector3D & v) const;
    Vector3D operator * (float s) const;
    Vector3D operator / (float s) const;

    friend Vector3D operator * (float s, const class Vector3D & v);

    bool operator == (const Vector3D & v) const;
    bool operator != (const Vector3D & v) const;

	float Dot(const Vector3D & v) const;
	Vector3D Cross(const Vector3D & v) const;
	float GetLength() const;
	float GetLengthSq() const;
	void SetLength(float length);
	void Normalize();
	float Distance(const Vector3D & v) const;
	float SqDistance(const Vector3D & v) const;
	Vector3D Lerp(float w, const Vector3D & v) const;
	void Transform(const Matrix4x4 & m);
	void Transform(const Quaternion & q);
	float GetAngle(const Vector3D & v) const;
};