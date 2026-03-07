#include "Texture.h"

#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Static member initialization
bool Texture::s_disableGPU = false;

// ============================================================================
// Texture
// ============================================================================

Texture::Texture() = default;

Texture::~Texture() {
    release();
}

Texture::Texture(Texture&& other) noexcept 
    : m_textureId(other.m_textureId)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_name(std::move(other.m_name))
{
    other.m_textureId = 0;
    other.m_width = 0;
    other.m_height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        release();
        m_textureId = other.m_textureId;
        m_width = other.m_width;
        m_height = other.m_height;
        m_name = std::move(other.m_name);
        
        other.m_textureId = 0;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

void Texture::release() {
    if (m_textureId != 0 && !s_disableGPU) {
        glDeleteTextures(1, &m_textureId);
    }
    m_textureId = 0;
    m_width = 0;
    m_height = 0;
}

void Texture::createGLTexture(const uint8_t* rgba, int width, int height) {
    release();
    
    m_width = width;
    m_height = height;
    
    // Skip GPU operations when disabled (for testing)
    if (s_disableGPU) {
        return;
    }
    
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    
    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    m_width = width;
    m_height = height;
}

bool Texture::loadFromRGBA(const uint8_t* data, int width, int height) {
    if (!data || width <= 0 || height <= 0) {
        return false;
    }
    
    createGLTexture(data, width, height);
    return m_textureId != 0;
}

bool Texture::loadFromFileData(const uint8_t* fileData, size_t dataSize) {
    if (!fileData || dataSize == 0) {
        return false;
    }
    
    // Use stb_image to decode the image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // OpenGL expects bottom-left origin
    
    uint8_t* pixels = stbi_load_from_memory(
        fileData, 
        static_cast<int>(dataSize), 
        &width, &height, &channels, 
        4  // Force RGBA
    );
    
    if (!pixels) {
        return false;
    }
    
    createGLTexture(pixels, width, height);
    
    stbi_image_free(pixels);
    
    return m_textureId != 0;
}

void Texture::bind(uint32_t textureUnit) const {
    if (s_disableGPU) return;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void Texture::unbind(uint32_t textureUnit) {
    if (s_disableGPU) return;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ============================================================================
// TextureManager
// ============================================================================

TextureManager& TextureManager::instance() {
    static TextureManager instance;
    return instance;
}

std::shared_ptr<Texture> TextureManager::createFromRGBA(
    const std::string& name,
    const uint8_t* data,
    int width, int height)
{
    auto texture = std::make_shared<Texture>();
    if (texture->loadFromRGBA(data, width, height)) {
        texture->setName(name);
        
        // Remove existing texture with same name
        removeTexture(name);
        
        m_textures.push_back({name, texture});
        return texture;
    }
    return nullptr;
}

std::shared_ptr<Texture> TextureManager::createFromFileData(
    const std::string& name,
    const uint8_t* fileData,
    size_t dataSize)
{
    auto texture = std::make_shared<Texture>();
    if (texture->loadFromFileData(fileData, dataSize)) {
        texture->setName(name);
        
        // Remove existing texture with same name
        removeTexture(name);
        
        m_textures.push_back({name, texture});
        return texture;
    }
    return nullptr;
}

std::shared_ptr<Texture> TextureManager::getTexture(const std::string& name) const {
    for (const auto& [texName, texture] : m_textures) {
        if (texName == name) {
            return texture;
        }
    }
    return nullptr;
}

void TextureManager::removeTexture(const std::string& name) {
    m_textures.erase(
        std::remove_if(m_textures.begin(), m_textures.end(),
            [&name](const auto& pair) { return pair.first == name; }),
        m_textures.end());
}

void TextureManager::clear() {
    m_textures.clear();
    m_whiteTexture.reset();
}

std::vector<std::string> TextureManager::getTextureNames() const {
    std::vector<std::string> names;
    names.reserve(m_textures.size());
    for (const auto& [name, texture] : m_textures) {
        names.push_back(name);
    }
    return names;
}

std::shared_ptr<Texture> TextureManager::getWhiteTexture() {
    if (!m_whiteTexture) {
        // Create a 1x1 white texture
        uint8_t white[] = { 255, 255, 255, 255 };
        m_whiteTexture = std::make_shared<Texture>();
        m_whiteTexture->loadFromRGBA(white, 1, 1);
        m_whiteTexture->setName("__white__");
    }
    return m_whiteTexture;
}
