#pragma once

#include <string>
#include <cstdint>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();
    
    // Load and compile shaders from files
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    
    // Load and compile shaders from source strings
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    // Use this shader program
    void use() const;
    
    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat3(const std::string& name, const glm::mat3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
    // Get program ID
    uint32_t getProgram() const { return m_program; }
    
    // Check if valid
    bool isValid() const { return m_valid; }
    
private:
    uint32_t compileShader(uint32_t type, const std::string& source);
    bool linkProgram(uint32_t vertexShader, uint32_t fragmentShader);
    std::string readFile(const std::string& path);
    
    uint32_t m_program;
    bool m_valid;
};
