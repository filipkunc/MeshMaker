/*
 *  Matrix4x4.h
 *  OpenGLWidgets
 *
 *  Created by Filip Kunc on 6/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "MathForwardDeclaration.h"

class Matrix4x4
{
public:
	float m[16];

	Matrix4x4();
    Matrix4x4(const float * m);
    Matrix4x4(const Matrix4x4 & m);
	Matrix4x4(const Vector3D & translate, const Quaternion & rotate, const Vector3D & scale);

    float & operator() (int row, int col);
    float operator() (int row, int col) const;

    operator float * ();
    operator const float * () const;

    Matrix4x4 & operator *= (const Matrix4x4 & m);
    Matrix4x4 & operator += (const Matrix4x4 & m);
    Matrix4x4 & operator -= (const Matrix4x4 & m);
    Matrix4x4 & operator *= (float s);
    Matrix4x4 & operator /= (float s);

    Matrix4x4 operator + () const;
    Matrix4x4 operator - () const;

    Matrix4x4 operator * (const Matrix4x4 & m) const;
    Matrix4x4 operator + (const Matrix4x4 & m) const;
    Matrix4x4 operator - (const Matrix4x4 & m) const;
    Matrix4x4 operator * (float s) const;
    Matrix4x4 operator / (float s) const;

    friend Matrix4x4 operator * (float s, const Matrix4x4 & m);

    bool operator == (const Matrix4x4 & m) const;
    bool operator != (const Matrix4x4 & m) const;

	void Identity();
	void Translate(float x, float y, float z);
	void Translate(const Vector3D & v);
	void Rotate(const Quaternion & q);
	void Rotate(float x, float y, float z);
	void Rotate(const Vector3D & v);
	void Scale(float uniformScale);
	void Scale(float x, float y, float z);
	void Scale(const Vector3D & v);
	void TranslateRotateScale(const Vector3D & translate, const Quaternion & rotate, const Vector3D & scale);

	Matrix4x4 Inverse() const;
	Matrix4x4 Transpose() const;
};

float Det2x2(float a1, float a2, float b1, float b2);

float Det3x3(float a1, float a2, float a3,
			 float b1, float b2, float b3,
             float c1, float c2, float c3);