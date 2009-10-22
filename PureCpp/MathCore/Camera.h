#pragma once

#include "MathDeclaration.h"

class Camera
{
private:
	Vector3D radians;
	float minZoom;
	Vector3D center;
	Vector3D startPosition;
	Vector3D position;
	Vector3D startAxisX;
	Vector3D startAxisY;
	Vector3D startAxisZ;
	Vector3D axisX;
	Vector3D axisY;
	Vector3D axisZ;
	bool isLimitAxisY;
private:
	void ComputeVectors();
public:
	Camera();

	float GetRadX() const;
	void SetRadX(float value);
	float GetRadY() const;
	void SetRadY(float value);
	float GetRadZ() const;
	void SetRadZ(float value);
	Vector3D GetRadians() const;
	void SetRadians(const Vector3D & newRadians);
	float GetZoom() const;
	void SetZoom(float value);
	Vector3D GetCenter() const;
	void SetCenter(const Vector3D & v);
	Vector3D GetPosition() const;
	void SetPosition(const Vector3D & v);
	Vector3D GetAxis(int index) const;
	Vector3D GetAxisX() const;
	Vector3D GetAxisY() const;
	Vector3D GetAxisZ() const;
	bool GetLimitAxisY() const;
	void SetLimitAxisY(bool value);
	Vector3D GetLimitedAxisX() const;
	Vector3D GetLimitedAxisY() const;
	Vector3D GetLimitedAxisZ() const;
	Matrix4x4 GetViewMatrix() const;
	Matrix4x4 GetBillboardMatrix() const;
	void MoveDirection(const Vector3D & v);
	void Move(float s);
	void LeftRight(float s);
	void UpDown(float s);
	void RotateLeftRight(float radians);
	void RotateUpDown(float radians);
	void RotateUpDown(float radians, float minRadians, float maxRadians);
	void Zoom(float zoom);
};