#include "Camera.h"
#include <algorithm>

Camera::Camera()
    : m_center(0.0f, 0.0f, 0.0f)
    , m_position(0.0f, 0.0f, 5.0f)
    , m_yaw(0.0f)
    , m_pitch(0.3f)  // Slight downward angle
    , m_distance(5.0f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(500.0f)
{
    updatePosition();
}

void Camera::updatePosition() {
    // Spherical to Cartesian conversion
    float x = m_distance * cos(m_pitch) * sin(m_yaw);
    float y = m_distance * sin(m_pitch);
    float z = m_distance * cos(m_pitch) * cos(m_yaw);
    
    m_position = m_center + glm::vec3(x, y, z);
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    return glm::lookAt(m_position, m_center, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::rotateLeftRight(float delta) {
    m_yaw += delta;
    updatePosition();
}

void Camera::rotateUpDown(float delta) {
    m_pitch += delta;
    m_pitch = std::clamp(m_pitch, MIN_PITCH, MAX_PITCH);
    updatePosition();
}

void Camera::zoom(float delta) {
    m_distance -= delta;
    m_distance = std::clamp(m_distance, MIN_DISTANCE, MAX_DISTANCE);
    updatePosition();
}

void Camera::pan(float deltaX, float deltaY) {
    // Calculate right and up vectors from current view
    glm::vec3 forward = glm::normalize(m_center - m_position);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::cross(right, forward);
    
    // Scale pan by distance for consistent feel
    float scale = m_distance * 0.002f;
    m_center += right * deltaX * scale;
    m_center += up * deltaY * scale;
    
    updatePosition();
}

void Camera::reset() {
    m_center = glm::vec3(0.0f, 0.0f, 0.0f);
    m_yaw = 0.0f;
    m_pitch = 0.3f;
    m_distance = 5.0f;
    updatePosition();
}

glm::vec3 Camera::getPosition() const {
    return m_position;
}
