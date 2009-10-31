/*
 *  Quaternion.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 6/21/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "MathForwardDeclaration.h"

class Quaternion
{
public:
	float x,y,z,w;

	Quaternion();
	Quaternion(const float * q);
	Quaternion(const Quaternion & q);
    Quaternion(float x, float y, float z, float w);

    operator float * ();
    operator const float * () const;

    Quaternion & operator += (const Quaternion & q);
    Quaternion & operator -= (const Quaternion & q);
    Quaternion & operator *= (const Quaternion & q);
    Quaternion & operator *= (float s);
    Quaternion & operator /= (float s);

    Quaternion  operator + () const;
    Quaternion  operator - () const;

    Quaternion operator + (const Quaternion & q) const;
    Quaternion operator - (const Quaternion & q) const;
    Quaternion operator * (const Quaternion & q) const;
    Quaternion operator * (float s) const;
    Quaternion operator / (float s) const;

    friend Quaternion operator * (float s, const Quaternion & q);

    bool operator == (const Quaternion & q) const;
    bool operator != (const Quaternion & q) const;

	float Dot(const Quaternion & q) const;
	Quaternion Conjugate() const;

	void FromAngleAxis(float radians, const Vector3D & axis);
	void ToAngleAxis(float & radians, Vector3D & axis) const;

	void FromMatrix(const Matrix4x4 & m);
	void ToMatrix(Matrix4x4 & m) const;

	void FromEulerAngles(const Vector3D & v);
	void ToEulerAngles(Vector3D & v) const;

	Quaternion Slerp(float s, const Quaternion & q) const;
};