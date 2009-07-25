#include "Camera.h"

Camera::Camera()
{
	radians = Vector3D(0, 0, 0);
	minZoom = 1.0f;
	center = Vector3D(0, 0, 0);
	startPosition = Vector3D(0, 0, 300);
	position = Vector3D(0, 0, 300);
	startAxisX = Vector3D(1, 0, 0);
	startAxisY = Vector3D(0, 1, 0);
	startAxisZ = Vector3D(0, 0, 1);
	axisX = Vector3D(1, 0, 0);
	axisY = Vector3D(0, 1, 0);
	axisZ = Vector3D(0, 0, 1);
}

void Camera::ComputeVectors()
{
	Matrix4x4 rotX, rotZ;
    Quaternion q_x, q_y, q_z;
    Quaternion q_yx, q_yz;
    q_x.FromAngleAxis(radians.x, startAxisX);
	q_y.FromAngleAxis(radians.y, startAxisY);
	q_z.FromAngleAxis(radians.z, startAxisZ);
    q_yx = q_y * q_x;
    q_yx.ToMatrix(rotZ);
    q_yz = q_y * q_z;
    q_yz.ToMatrix(rotX);
    axisX = startAxisX;
    axisZ = startAxisZ;
    axisX.Transform(rotX);
    axisZ.Transform(rotZ);
    axisY = axisX.Cross(axisZ);

    position = startPosition;
    position -= center;
    position.Transform(q_yx);
    position += center;
}

float Camera::GetRadX() const
{
	return radians.x;
}

void Camera::SetRadX(float value)
{
	radians.x = value;
	ComputeVectors();
}
	
float Camera::GetRadY() const
{
	return radians.y;
}
	
void Camera::SetRadY(float value)
{
	radians.y = value;
	ComputeVectors();
}
	
float Camera::GetRadZ() const
{
	return radians.z;
}
	
void Camera::SetRadZ(float value)
{
	radians.z = value;
	ComputeVectors();
}
	
float Camera::GetZoom() const
{
	return position.Distance(center);
}
	
void Camera::SetZoom(float value)
{
	float newZoom = position.Distance(center);
	newZoom -= value;
	Zoom(newZoom);
}
	
Vector3D Camera::GetCenter() const
{
	return center;	
}

void Camera::SetCenter(const Vector3D & v)
{
	MoveDirection(v - center);
}
	
Vector3D Camera::GetPosition() const
{
	return position;
}

void Camera::SetPosition(const Vector3D & v)
{
	MoveDirection(v - position);
}

Vector3D Camera::GetAxis(int index) const
{
	switch (index)
	{
		case 0:
			return axisX;
		case 1:
			return axisY;
		case 2:
			return axisZ;
		default:
			return Vector3D();
	}
}
	
Vector3D Camera::GetAxisX() const
{
	return axisX;
}
	
Vector3D Camera::GetAxisY() const
{
	return axisY;
}
	
Vector3D Camera::GetAxisZ() const
{
	return axisZ;
}

bool Camera::GetLimitAxisY() const
{
	return isLimitAxisY;
}

void Camera::SetLimitAxisY(bool value)
{
	isLimitAxisY = value;
}

Vector3D Camera::GetLimitedAxisX() const
{
	if (isLimitAxisY)
	{
		Vector3D limitedAxisX = axisX;
		limitedAxisX.y = 0.0f;
		limitedAxisX.Normalize();
		return limitedAxisX;
	}
	return axisX;
}

Vector3D Camera::GetLimitedAxisY() const
{
	if (isLimitAxisY)
		return Vector3D(0, 1, 0);
	return axisY;
}

Vector3D Camera::GetLimitedAxisZ() const
{
	if (isLimitAxisY)
	{
		Vector3D limitedAxisZ = axisZ;
		limitedAxisZ.y = 0.0f;
		limitedAxisZ.Normalize();
		return limitedAxisZ;
	}
	return axisZ;
}
	
Matrix4x4 Camera::GetViewMatrix() const
{
	Matrix4x4 trans, rot;
    trans.Translate(-position);
    Quaternion q_x, q_y, q_z, q;
    q_x.FromAngleAxis(-radians.x, startAxisX);
	q_y.FromAngleAxis(-radians.y, startAxisY);
	q_z.FromAngleAxis(-radians.z, startAxisZ);
    q = q_z * q_x * q_y;
    q.ToMatrix(rot);
    return (rot * trans);
}
	
Matrix4x4 Camera::GetBillboardMatrix() const
{
	Matrix4x4 rot;
	Quaternion q_x,q_y,q_z,q;
	q_x.FromAngleAxis(radians.x, startAxisX);
    q_y.FromAngleAxis(radians.y, startAxisY);
    q_z.FromAngleAxis(radians.z, startAxisZ);
	q = q_z * q_y * q_x;
	q.ToMatrix(rot);
	return rot;
}
	
void Camera::MoveDirection(const Vector3D & v)
{
	center += v;
	startPosition += v;
    position += v;
}
	
void Camera::Move(float s)
{
	MoveDirection(GetLimitedAxisZ() * s);
}
	
void Camera::LeftRight(float s)
{
    MoveDirection(GetLimitedAxisX() * s);
}
	
void Camera::UpDown(float s)
{
	MoveDirection(GetLimitedAxisY() * s);
}
	
void Camera::RotateLeftRight(float radians)
{
	this->radians.y -= radians;
	ComputeVectors();
}
	
void Camera::RotateUpDown(float radians)
{
	this->radians.x -= radians;
	ComputeVectors();
}

void Camera::RotateUpDown(float radians, float minRadians, float maxRadians)
{
	this->radians.x -= radians;
	this->radians.x = Min<float>(this->radians.x, maxRadians);
	this->radians.x = Max<float>(this->radians.x, minRadians);
	ComputeVectors();
}
	
void Camera::Zoom(float zoom)
{
	float currentZoom = GetZoom();
	if (zoom > currentZoom - minZoom)
        zoom = currentZoom - minZoom;
    startPosition -= startAxisZ * zoom;
    position -= axisZ * zoom;
}
