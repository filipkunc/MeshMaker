//
//  Quaternion.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/21/09.
//  For license see LICENSE.TXT
//

#include "MathDeclaration.h"

Quaternion::Quaternion()
{
	x = y = z = 0.0f;
	w = 1.0f;
}

Quaternion::Quaternion(const float * q)
{
	x = q[0];
	y = q[1];
	z = q[2];
	z = q[3];
}

Quaternion::Quaternion(const Quaternion & q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion::operator float *()
{
	return &x;
}

Quaternion::operator const float *() const
{
	return &x;
}

Quaternion & Quaternion::operator +=(const Quaternion & q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

Quaternion & Quaternion::operator -= (const Quaternion & q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}
	
Quaternion & Quaternion::operator *= (const Quaternion & q)
{
	*this = *this * q;
	return *this;
}

Quaternion & Quaternion::operator *= (float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Quaternion & Quaternion::operator /= (float s)
{
	float oos = 1.0f / s;
	x *= oos;
	y *= oos;
	z *= oos;
	w *= oos;
	return *this;
}

Quaternion Quaternion::operator + () const
{
	return Quaternion(x, y, z, w);
}

Quaternion Quaternion::operator - () const
{
	return Quaternion(-x, -y, -z, -w);
}

Quaternion Quaternion::operator + (const Quaternion & q) const
{
	return Quaternion(x + q.x, y + q.y, z + q.z , w + q.w);
}
   
Quaternion Quaternion::operator - (const Quaternion & q) const
{
	return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
}

Quaternion Quaternion::operator * (const Quaternion & q) const
{
	return Quaternion(
		this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y,
		this->w * q.y + this->y * q.w + this->z * q.x - this->x * q.z,
		this->w * q.z + this->z * q.w + this->x * q.y - this->y * q.x,
		this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z
	);
}

Quaternion Quaternion::operator * (float s) const
{
	return Quaternion(x * s, y * s, z * s, w * s);
}

Quaternion Quaternion::operator / (float s) const
{
	float oos = 1.0f / s;
	return Quaternion(x * oos, y * oos, z * oos, w * oos);
}

Quaternion operator * (float s, const class Quaternion & q)
{
	return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
}

bool Quaternion::operator == (const Quaternion & q) const
{
	return (q.x == x && q.y == y && q.z == z && q.w == w);
}

bool Quaternion::operator != (const Quaternion & q) const
{
	return (q.x != x || q.y != y || q.z != z || q.w != w);
}

float Quaternion::Dot(const Quaternion & q) const
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

void Quaternion::FromAngleAxis(float radians, const Vector3D & axis)
{
	radians *= 0.5f;
	float s = sinf(radians);
	Vector3D tempAxis = axis;
	tempAxis.SetLength(s);
	x = tempAxis.x;
    y = tempAxis.y;
    z = tempAxis.z;
	w = cosf(radians);
}

void Quaternion::ToAngleAxis(float & radians, Vector3D & axis) const
{
	float sum = x * x + y * y + z * z;
	if (fabsf(sum) < FLOAT_EPS)
	{
		radians = 0.0f;
		axis = Vector3D(1.0f, 0.0f, 0.0f);
		return;
	}
    float OneOver = 1.0f / sqrtf(sum);
    axis = Vector3D(OneOver * x, OneOver * y, OneOver * z);
	if (fabsf(w - 1.0f) < FLOAT_EPS)
		radians = 0.0f;
	else
		radians = 2.0f * acosf(w);
}

void Quaternion::FromMatrix(const Matrix4x4 & m)
{
	float trace = m(0, 0) + m(1, 1) + m(2, 2);
	if (trace > 0)
	{
		float scale = sqrtf(trace + 1.0f);
		w = 0.5f * scale;
		scale = 0.5f / scale;
		x = scale * (m(2, 1) - m(1, 2));
		y = scale * (m(0, 2) - m(2, 0));
		z = scale * (m(1, 0) - m(0, 1));
	}
	else
	{
		static int next[] = { 1, 2, 0 };
		int i = 0;
		if (m(1, 1) > m(0, 0))
			i = 1;
		if (m(2, 2) > m(i, i))
			i = 2;
		int j = next[i];
		int k = next[j];
		float scale = sqrtf(m(i, i) - m(j, j) - m(k, k) + 1);
		float * q[] = { &x, &y, &z };
		*q[i] = 0.5f * scale;
		scale = 0.5f / scale;
		w = scale * (m(k, j) - m(j, k));
		*q[j] = scale * (m(j, i) + m(i, j));
		*q[k] = scale * (m(k, i) + m(i, k));
	}
}

void Quaternion::ToMatrix(Matrix4x4 & m) const
{
	float x2 = x * 2.0f;
	float y2 = y * 2.0f;
	float z2 = z * 2.0f;
	float wx = x2 * w;
	float wy = y2 * w;
	float wz = z2 * w;
	float xx = x2 * x;
	float xy = y2 * x;
	float xz = z2 * x;
	float yy = y2 * y;
	float yz = z2 * y;
	float zz = z2 * z;
	m[0] = 1.0f - (yy + zz);
	m[4] = xy - wz;
	m[8] = xz + wy;
	m[1] = xy + wz;
	m[5] = 1 - (xx + zz);
	m[9] = yz - wx;
	m[2] = xz - wy;
	m[6] = yz + wx;
	m[10] = 1 - (xx + yy);
}

void Quaternion::FromEulerAngles(const Vector3D & v)
{
	float c1 = cosf(v.y / 2.0f);
    float s1 = sinf(v.y / 2.0f);
    float c2 = cosf(v.z / 2.0f);
    float s2 = sinf(v.z / 2.0f);
    float c3 = cosf(v.x / 2.0f);
    float s3 = sinf(v.x / 2.0f);
    float c1c2 = c1 * c2;
    float s1s2 = s1 * s2;
	w = c1c2 * c3 - s1s2 * s3;
    x = c1c2 * s3 + s1s2 * c3;
    y = s1 * c2 * c3 + c1 * s2 * s3;
    z = c1 * s2 * c3 - s1 * c2 * s3;
}

void Quaternion::ToEulerAngles(Vector3D & v) const
{
    float sqw = w * w;
    float sqx = x * x;
    float sqy = y * y;
    float sqz = z * z;
    float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
    float test = x * y + z * w;
    if (test > 0.499f * unit) // singularity at north pole
    {
		v.y = 2.0f * atan2f(x, w);
        v.z = FLOAT_PI / 2.0f;
        v.x = 0.0f;
		return;
    }
    if (test < -0.499f * unit) // singularity at south pole
    {
        v.y = -2.0f * atan2f(x, w);
        v.z = -FLOAT_PI / 2.0f;
        v.x = 0;
        return;
    }
    v.y = atan2f(2.0f * y * w - 2.0f * x * z, sqx - sqy - sqz + sqw);
	v.z = asinf(2.0f * test / unit);
    v.x = atan2f(2.0f * x * w - 2.0f * y * z, -sqx + sqy - sqz + sqw);
    return;
}

Quaternion Quaternion::Slerp(float s, const Quaternion & q) const
{
	float cosine = this->Dot(q);
	if (cosine < -1.0f)
		cosine = -1.0f;
	else if (cosine > 1.0f)
		cosine = 1.0f;
    float angle = acosf(cosine);
    if (fabsf(angle) < FLOAT_EPS)
		return *this;
  
	float sine = sinf(angle);
    float sineInv = 1.0f / sine;
    float c1 = sinf((1.0f - s) * angle) * sineInv;
    float c2 = sinf(s * angle) * sineInv;

	return Quaternion(
		c1 * x + c2 * q.x,
		c1 * y + c2 * q.y,
		c1 * z + c2 * q.z,
		c1 * w + c2 * q.w
	);
}