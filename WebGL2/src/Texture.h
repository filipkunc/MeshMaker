#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#ifdef EMSCRIPTEN_BUILD
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

// Forward declarations
class TextureManager;

// Represents a GPU texture
class Texture {
public:
    // Static flag to disable GPU operations (for testing)
    static bool s_disableGPU;
    
    Texture();
    ~Texture();
    
    // Prevent copying (GPU resource)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    
    // Allow moving
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    
    // Load from raw pixel data (RGBA format expected)
    bool loadFromRGBA(const uint8_t* data, int width, int height);
    
    // Load from image file data (PNG, JPG, etc. - decoded by stb_image)
    bool loadFromFileData(const uint8_t* fileData, size_t dataSize);
    
    // Bind texture to a texture unit (default: GL_TEXTURE0)
    void bind(uint32_t textureUnit = 0) const;
    
    // Unbind texture
    static void unbind(uint32_t textureUnit = 0);
    
    // Check if texture is valid
    bool isValid() const { 
        if (s_disableGPU) {
            return m_width > 0 && m_height > 0;
        }
        return m_textureId != 0; 
    }
    
    // Get texture ID
    uint32_t getId() const { return m_textureId; }
    
    // Get dimensions
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    // Get name (for UI/debugging)
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

private:
    void release();
    void createGLTexture(const uint8_t* rgba, int width, int height);
    
    uint32_t m_textureId = 0;
    int m_width = 0;
    int m_height = 0;
    std::string m_name;
};

// Manages texture loading and caching
class TextureManager {
public:
    static TextureManager& instance();
    
    // Create a new texture from RGBA data
    std::shared_ptr<Texture> createFromRGBA(const std::string& name, 
                                            const uint8_t* data, 
                                            int width, int height);
    
    // Create a new texture from file data (PNG, JPG, etc.)
    std::shared_ptr<Texture> createFromFileData(const std::string& name,
                                                 const uint8_t* fileData, 
                                                 size_t dataSize);
    
    // Get texture by name (returns nullptr if not found)
    std::shared_ptr<Texture> getTexture(const std::string& name) const;
    
    // Remove texture by name
    void removeTexture(const std::string& name);
    
    // Clear all textures
    void clear();
    
    // Get all texture names
    std::vector<std::string> getTextureNames() const;
    
    // Get default white texture (1x1 white pixel) - useful for untextured objects
    std::shared_ptr<Texture> getWhiteTexture();

private:
    TextureManager() = default;
    ~TextureManager() = default;
    
    std::vector<std::pair<std::string, std::shared_ptr<Texture>>> m_textures;
    std::shared_ptr<Texture> m_whiteTexture;
};
