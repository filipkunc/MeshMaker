#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
    Camera();
    
    // Get matrices for rendering
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    // Orbit camera controls (matching original MeshMaker)
    void rotateLeftRight(float radians);
    void rotateUpDown(float radians);
    void zoom(float delta);
    void leftRight(float delta);  // Pan left/right
    void upDown(float delta);     // Pan up/down
    
    // Reset to default view
    void reset();
    
    // Accessors
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getCenter() const { return m_center; }
    float getZoom() const { return m_zoom; }
    float getDistance() const { return m_zoom; }  // Alias for zoom (distance from center)
    glm::vec3 getAxisX() const { return m_axisX; }
    glm::vec3 getAxisY() const { return m_axisY; }
    glm::vec3 getAxisZ() const { return m_axisZ; }
    glm::vec3 getForwardDirection() const { return -m_axisZ; }  // Camera looks along -Z
    
private:
    void computeVectors();
    glm::quat getRotationQuaternion() const;
    void moveDirection(const glm::vec3& v);
    
    glm::vec2 m_radians;     // x = pitch (around X), y = yaw (around Y)
    float m_zoom;            // Distance from center
    float m_minZoom;
    glm::vec3 m_center;
    glm::vec3 m_position;
    glm::vec3 m_axisX;
    glm::vec3 m_axisY;
    glm::vec3 m_axisZ;
    
    // Projection parameters
    float m_fov;
    float m_nearPlane;
    float m_farPlane;
};
