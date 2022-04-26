#pragma once


#include "Renderer.h"

class Texture2D
{
private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
public:
    Texture2D(const std::string& path, bool invert);
    ~Texture2D();

    void Bind(unsigned int slot = 0) const;
    void Unbind();

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
};