#pragma once

#include "MathDeclaration.h"

class Camera
{
private:
    Vector2D radians;
    float zoom;
	float minZoom;
	Vector3D center;
	Vector3D position;
	Vector3D axisX;
	Vector3D axisY;
	Vector3D axisZ;
private:
	void ComputeVectors();
public:
	Camera();
    
	Vector2D GetRadians() const;
	void SetRadians(const Vector2D &newRadians);
	float GetZoom() const;
	void SetZoom(float value);
	Vector3D GetCenter() const;
	void SetCenter(const Vector3D & v);
	Vector3D GetPosition() const;
	void SetPosition(const Vector3D & v);
	Vector3D GetAxisX() const;
	Vector3D GetAxisY() const;
	Vector3D GetAxisZ() const;
	Matrix4x4 GetViewMatrix() const;
	Quaternion GetRotationQuaternion() const;
	void MoveDirection(const Vector3D & v);
	void Move(float s);
	void LeftRight(float s);
	void UpDown(float s);
	void RotateLeftRight(float radians);
	void RotateUpDown(float radians);
	void Zoom(float s);
};