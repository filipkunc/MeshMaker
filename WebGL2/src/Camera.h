#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    
    // Get matrices for rendering
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    // Orbit camera controls
    void rotateLeftRight(float delta);
    void rotateUpDown(float delta);
    void zoom(float delta);
    void pan(float deltaX, float deltaY);
    
    // Reset to default view
    void reset();
    
    // Accessors
    glm::vec3 getPosition() const;
    glm::vec3 getCenter() const { return m_center; }
    float getZoom() const { return m_distance; }
    
private:
    void updatePosition();
    
    glm::vec3 m_center;      // Look-at point
    glm::vec3 m_position;    // Camera position (computed)
    float m_yaw;             // Horizontal rotation (radians)
    float m_pitch;           // Vertical rotation (radians)
    float m_distance;        // Distance from center (zoom)
    
    // Projection parameters
    float m_fov;
    float m_nearPlane;
    float m_farPlane;
    
    // Limits
    static constexpr float MIN_DISTANCE = 0.1f;
    static constexpr float MAX_DISTANCE = 500.0f;
    static constexpr float MIN_PITCH = -1.5f;  // ~-85 degrees
    static constexpr float MAX_PITCH = 1.5f;   // ~85 degrees
};
