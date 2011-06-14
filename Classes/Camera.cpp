#include "Camera.h"

Camera::Camera()
{
    zoom = 300.0f;
	minZoom = 0.1f;
    ComputeVectors();
}

void Camera::ComputeVectors()
{
	Matrix4x4 rot;
    rot = GetRotationQuaternion().Conjugate().ToMatrix();
    
    axisX = rot.Transform(Vector3D(1, 0, 0));
    axisY = rot.Transform(Vector3D(0, 1, 0));
    axisZ = rot.Transform(Vector3D(0, 0, 1));

    position = center - axisZ * zoom;
}

Vector2D Camera::GetRadians() const
{
	return radians;
}

void Camera::SetRadians(const Vector2D &newRadians)
{
	radians = newRadians;
	ComputeVectors();
}
	
float Camera::GetZoom() const
{
	return zoom;
}
	
void Camera::SetZoom(float value)
{
	zoom = value;
    if (zoom < minZoom)
        zoom = minZoom;
    ComputeVectors();
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
	
Matrix4x4 Camera::GetViewMatrix() const
{
	Matrix4x4 trans, rot;
    trans.Translate(position);
    rot = GetRotationQuaternion().ToMatrix();   
    return rot * trans;
}

Quaternion Camera::GetRotationQuaternion() const
{
    Quaternion q_x, q_y, q;
    q_x.FromAngleAxis(-radians.x, Vector3D(1, 0, 0));
	q_y.FromAngleAxis(-radians.y, Vector3D(0, 1, 0));
    q = q_x * q_y;
    return q;
}
	
void Camera::MoveDirection(const Vector3D & v)
{
	center += v;
    position += v;
}
	
void Camera::Move(float s)
{
	MoveDirection(GetAxisZ() * s);
}
	
void Camera::LeftRight(float s)
{
    MoveDirection(-GetAxisX() * s);
}
	
void Camera::UpDown(float s)
{
	MoveDirection(GetAxisY() * s);
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

void Camera::Zoom(float zoom)
{
    this->zoom -= zoom;
    if (zoom < minZoom)
        zoom = minZoom;
    ComputeVectors();
}
