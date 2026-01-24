#include "Camera.h"
#include <algorithm>

Camera::Camera()
    : m_radians(glm::radians(-45.0f), glm::radians(45.0f))  // -45° and 45°, matching original
    , m_zoom(20.0f)  // Match original zoom
    , m_minZoom(0.1f)
    , m_center(0.0f, 0.0f, 0.0f)
    , m_position(0.0f, 0.0f, 5.0f)
    , m_axisX(1.0f, 0.0f, 0.0f)
    , m_axisY(0.0f, 1.0f, 0.0f)
    , m_axisZ(0.0f, 0.0f, 1.0f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(500.0f)
{
    computeVectors();
}

void Camera::computeVectors() {
    // Get rotation matrix from quaternion (conjugate for camera transform)
    glm::mat3 rot = glm::mat3_cast(glm::conjugate(getRotationQuaternion()));
    
    m_axisX = rot * glm::vec3(1.0f, 0.0f, 0.0f);
    m_axisY = rot * glm::vec3(0.0f, 1.0f, 0.0f);
    m_axisZ = rot * glm::vec3(0.0f, 0.0f, 1.0f);
    
    m_position = m_center - m_axisZ * m_zoom;
}

glm::quat Camera::getRotationQuaternion() const {
    // Match original: q_x * q_y with negative angles
    glm::quat q_x = glm::angleAxis(-m_radians.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat q_y = glm::angleAxis(-m_radians.y, glm::vec3(0.0f, 1.0f, 0.0f));
    return q_x * q_y;
}

glm::mat4 Camera::getViewMatrix() const {
    // Match original: rot * trans
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rot = glm::mat4_cast(getRotationQuaternion());
    return rot * trans;
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::moveDirection(const glm::vec3& v) {
    m_center += v;
    computeVectors();
}

void Camera::rotateLeftRight(float radians) {
    // Match original: radians.y -= radians
    m_radians.y -= radians;
    computeVectors();
}

void Camera::rotateUpDown(float radians) {
    // Match original: radians.x -= radians
    m_radians.x -= radians;
    computeVectors();
}

void Camera::zoom(float delta) {
    // Match original: zoom -= s
    m_zoom -= delta;
    if (m_zoom < m_minZoom)
        m_zoom = m_minZoom;
    computeVectors();
}

void Camera::leftRight(float delta) {
    // Match original: MoveDirection(-GetAxisX() * s)
    moveDirection(-m_axisX * delta);
}

void Camera::upDown(float delta) {
    // Match original: MoveDirection(GetAxisY() * s)
    moveDirection(m_axisY * delta);
}

void Camera::reset() {
    m_radians = glm::vec2(glm::radians(-45.0f), glm::radians(45.0f));
    m_zoom = 20.0f;
    m_center = glm::vec3(0.0f, 0.0f, 0.0f);
    computeVectors();
}

