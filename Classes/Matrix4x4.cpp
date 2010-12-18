//
//  Matrix4x4.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#include "MathDeclaration.h"

Matrix4x4::Matrix4x4()
{
	Identity();
}

Matrix4x4::Matrix4x4(const float * m)
{
	memcpy(this->m, m, 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(const Matrix4x4 & m)
{
	memcpy(this->m, m.m, 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(const Vector3D & translate, const Quaternion & rotate, const Vector3D & scale)
{
	TranslateRotateScale(translate, rotate, scale);
}

void Matrix4x4::TranslateRotateScale(const Vector3D & translate, const Quaternion & rotate, const Vector3D & scale)
{
	Matrix4x4 t;
	Matrix4x4 r;
	Matrix4x4 s;
	t.Translate(translate);
	rotate.ToMatrix(r);
	s.Scale(scale);
	*this = t * r * s;
}

float & Matrix4x4::operator() (int row, int col)
{
	return m[row + col * 4];
}

float Matrix4x4::operator() (int row, int col) const
{
	return m[row + col * 4];
}

Matrix4x4::operator float * ()
{
	return m;
}
    
Matrix4x4::operator const float * () const
{
	return m;
}

Matrix4x4 & Matrix4x4::operator *= (const Matrix4x4 & m)
{
	*this = *this * m;
	return *this;
}

Matrix4x4 & Matrix4x4::operator += (const Matrix4x4 & m)
{
	*this = *this * m;
	return *this;
}
    
Matrix4x4 & Matrix4x4::operator -= (const Matrix4x4 & m)
{
	*this = *this - m;
	return *this;
}

Matrix4x4 & Matrix4x4::operator *= (float s)
{
	*this = *this * s;
	return *this;
}
    
Matrix4x4 & Matrix4x4::operator /= (float s)
{
	*this = *this / s;
	return *this;
}

Matrix4x4 Matrix4x4::operator + () const
{
	return Matrix4x4(m);
}

Matrix4x4 Matrix4x4::operator - () const
{
	Matrix4x4 result;

	for (int i = 0; i < 16; i++)
		result.m[i] = -m[i];

	return result;
}

    
Matrix4x4 Matrix4x4::operator * (const Matrix4x4 & m) const
{
	Matrix4x4 result;
	const float *a = this->m, *b = m.m;

	result.m[0] = a[0]*b[0] + a[4]*b[1] + a[8]*b[2];
	result.m[1] = a[1]*b[0] + a[5]*b[1] + a[9]*b[2];
	result.m[2] = a[2]*b[0] + a[6]*b[1] + a[10]*b[2];
	result.m[3] = 0;

	result.m[4] = a[0]*b[4] + a[4]*b[5] + a[8]*b[6];
	result.m[5] = a[1]*b[4] + a[5]*b[5] + a[9]*b[6];
	result.m[6] = a[2]*b[4] + a[6]*b[5] + a[10]*b[6];
	result.m[7] = 0;

	result.m[8] = a[0]*b[8] + a[4]*b[9] + a[8]*b[10];
	result.m[9] = a[1]*b[8] + a[5]*b[9] + a[9]*b[10];
	result.m[10] = a[2]*b[8] + a[6]*b[9] + a[10]*b[10];
	result.m[11] = 0;

	result.m[12] = a[0]*b[12] + a[4]*b[13] + a[8]*b[14] + a[12];
	result.m[13] = a[1]*b[12] + a[5]*b[13] + a[9]*b[14] + a[13];
	result.m[14] = a[2]*b[12] + a[6]*b[13] + a[10]*b[14] + a[14];
	result.m[15] = 1;

	return result;
}
    
Matrix4x4 Matrix4x4::operator + (const Matrix4x4 & m) const
{
	Matrix4x4 result;
	const float *a = this->m, *b = m.m;

	for (int i = 0; i < 16; i++)
		result.m[i] = a[i] + b[i];

	return result;
}

Matrix4x4 Matrix4x4::operator - (const Matrix4x4 & m) const
{
	Matrix4x4 result;
	const float *a = this->m, *b = m.m;

	for (int i = 0; i < 16; i++)
		result.m[i] = a[i] - b[i];

	return result;
}
   
Matrix4x4 Matrix4x4::operator * (float s) const
{
	Matrix4x4 result;
	const float *a = this->m;

	for (int i = 0; i < 16; i++)
		result.m[i] = a[i] * s;

	return result;
}
    
Matrix4x4 Matrix4x4::operator / (float s) const
{
	Matrix4x4 result;
	const float *a = this->m;

	float oos = 1.0f / s;
	for (int i = 0; i < 16; i++)
		result.m[i] = a[i] * oos;

	return result;
}

Matrix4x4 operator * (float s, const Matrix4x4 & m)
{
	return m * s;
}

bool Matrix4x4::operator == (const Matrix4x4 & m) const
{
	for (int i = 0; i < 16; i++)
	{
		if (this->m[i] != m.m[i])
			return false;
	}
	return true;
}
    
bool Matrix4x4::operator != (const Matrix4x4 & m) const
{
	for (int i = 0; i < 16; i++)
	{
		if (this->m[i] == m.m[i])
			return false;
	}
	return true;
}
	
void Matrix4x4::Identity()
{
	for (int i = 0; i < 16; i++)
		m[i] = 0;
	m[0] = m[5] = m[10] = m[15] = 1;
}
	
void Matrix4x4::Translate(float x, float y, float z)
{
	m[12] = x;
	m[13] = y;
	m[14] = z;
}
	
void Matrix4x4::Translate(const Vector3D & v)
{
	m[12] = v.x;
	m[13] = v.y;
	m[14] = v.z;
}

void Matrix4x4::Rotate(const Quaternion & q)
{
	q.ToMatrix(*this);
}
	
void Matrix4x4::Rotate(float x, float y, float z)
{
	float cr = cosf(x);
	float sr = sinf(x);
	float cp = cosf(y);
	float sp = sinf(y);
	float cy = cosf(z);
	float sy = sinf(z);

	m[0] = cp * cy;
	m[1] = cp * sy;
	m[2] = -sp;

	float srsp = sr * sp;
	float crsp = cr * sp;

	m[4] = srsp * cy - cr * sy;
	m[5] = srsp * sy + cr * cy;
	m[6] = sr * cp;

	m[8] = crsp * cy + sr * sy;
	m[9] = crsp * sy - sr * cy;
	m[10] = cr * cp;
}
	
void Matrix4x4::Rotate(const Vector3D & v)
{
	Rotate(v.x, v.y, v.z);
}
	
void Matrix4x4::Scale(float uniformScale)
{
	m[0] = m[5] = m[10] = uniformScale;
}
	
void Matrix4x4::Scale(float x, float y, float z)
{
	m[0] = x;
	m[5] = y;
	m[10] = z;
}
	
void Matrix4x4::Scale(const Vector3D & v)
{
	m[0] = v.x;
	m[5] = v.y;
	m[10] = v.z;
}

Matrix4x4 Matrix4x4::Inverse() const
{
	Matrix4x4 result;
	float det, oodet;
    
	float * m = result.m;
	const float * t = this->m;

    m[0] = Det3x3(t[5], t[6], t[7], t[9], t[10], t[11], t[13], t[14], t[15]);
    m[1] = -Det3x3(t[1], t[2], t[3], t[9], t[10], t[11], t[13], t[14], t[15]);
    m[2] = Det3x3(t[1], t[2], t[3], t[5], t[6], t[7], t[13], t[14], t[15]);
    m[3] = -Det3x3(t[1], t[2], t[3], t[5], t[6], t[7], t[9], t[10], t[11]);

    m[4] = -Det3x3(t[4], t[6], t[7], t[8], t[10], t[11], t[12], t[14], t[15]);
    m[5] = Det3x3(t[0], t[2], t[3], t[8], t[10], t[11], t[12], t[14], t[15]);
    m[6] = -Det3x3(t[0], t[2], t[3], t[4], t[6], t[7], t[12], t[14], t[15]);
    m[7] = Det3x3(t[0], t[2], t[3], t[4], t[6], t[7], t[8], t[10], t[11]);

    m[8] = Det3x3(t[4], t[5], t[7], t[8], t[9], t[11], t[12], t[13], t[15]);
    m[9] = -Det3x3(t[0], t[1], t[3], t[8], t[9], t[11], t[12], t[13], t[15]);
    m[10] = Det3x3(t[0], t[1], t[3], t[4], t[5], t[7], t[12], t[13], t[15]);
    m[11] = -Det3x3(t[0], t[1], t[3], t[4], t[5], t[7], t[8], t[9], t[11]);

    m[12] = -Det3x3(t[4], t[5], t[6], t[8], t[9], t[10], t[12], t[13], t[14]);
    m[13] = Det3x3(t[0], t[1], t[2], t[8], t[9], t[10], t[12], t[13], t[14]);
    m[14] = -Det3x3(t[0], t[1], t[2], t[4], t[5], t[6], t[12], t[13], t[14]);
    m[15] = Det3x3(t[0], t[1], t[2], t[4], t[5], t[6], t[8], t[9], t[10]);

    det = (t[0] * m[0]) + (t[4] * m[1]) + (t[8] * m[2]) + (t[12] * m[3]);

    // The following divions goes unchecked for division
    // by zero. We should consider throwing an exception
    // if det < eps.
    oodet = 1.0f / det;

    for (int i = 0; i < 16; i++)
        m[i] *= oodet;

    return result;
}

Matrix4x4 Matrix4x4::Transpose() const
{
	Matrix4x4 result;
	result.m[1] = this->m[4];
	result.m[2] = this->m[8];
	result.m[4] = this->m[1];
	result.m[6] = this->m[9];
	result.m[8] = this->m[2];
	result.m[9] = this->m[6];
	return result;
}

float Det2x2(float a1, float a2, float b1, float b2)
{
	return a1 * b2 - b1 * a2;
}

float Det3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3)
{
	return a1 * Det2x2(b2, b3, c2, c3) - b1 * Det2x2(a2, a3, c2, c3) + c1 * Det2x2(a2, a3, b2, b3);
}
