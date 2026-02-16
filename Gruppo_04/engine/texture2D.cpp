#define FREEIMAGE_LIB
#include "texture2D.h"

#include <GL/glew.h>
#include <FreeImage.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>

/* ===== STATIC ===== */
unsigned int Texture2D::m_default_texture_id = 0;
std::vector<Texture2D*> Texture2D::m_textures{};

/* ===== CTOR / DTOR ===== */

Texture2D::Texture2D(const std::string& name, const std::string& file_path)
   : Object(name), m_file_path(file_path)
{
   if (!m_default_texture_id)
      generate_default_texture();

   m_textures.push_back(this);
}

Texture2D::~Texture2D() {
   glDeleteTextures(1, &m_texture_id);
}

/* ===== RENDER ===== */

void Texture2D::render(glm::mat4) {
   if (!m_loaded)
      load_texture();

   glBindTexture(GL_TEXTURE_2D, m_texture_id);

   apply_filter_and_mipmap();
   apply_texture_wrap();
}

/* ===== LOAD ===== */

void Texture2D::load_texture() {

   if (m_file_path == "[none]") {
      m_texture_id = m_default_texture_id;
      m_loaded = true;
      return;
   }

   glGenTextures(1, &m_texture_id);
   glBindTexture(GL_TEXTURE_2D, m_texture_id);

   std::ifstream file(m_file_path);
   if (!file.good())
      throw std::invalid_argument("Texture file not found");

   FREE_IMAGE_FORMAT format = FreeImage_GetFileType(m_file_path.c_str(), 0);
   if (format == FIF_UNKNOWN)
      throw std::invalid_argument("Unsupported texture format");

   FIBITMAP* bitmap = FreeImage_Load(format, m_file_path.c_str());
   if (!bitmap)
      throw std::invalid_argument("FreeImage load failed");

   if (format == FIF_DDS)
      FreeImage_FlipVertical(bitmap);

   glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA,
      FreeImage_GetWidth(bitmap),
      FreeImage_GetHeight(bitmap),
      0, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
      FreeImage_GetBits(bitmap)
   );

   FreeImage_Unload(bitmap);
   m_loaded = true;
}

/* ===== DEFAULT TEXTURE ===== */

void Texture2D::generate_default_texture() {

   unsigned char data[16] = {
       255,255,255,255, 0,0,0,255,
       0,0,0,255,       255,255,255,255
   };

   glGenTextures(1, &m_default_texture_id);
   glBindTexture(GL_TEXTURE_2D, m_default_texture_id);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, data);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/* ===== PARAMS ===== */

void Texture2D::set_filter(Filter f, TextureMipmap m) {
   m_filter = f;
   m_mipmap = m;
}

void Texture2D::set_texture_wrap(TextureWrap wrap) {
   m_wrap = wrap;
}

void Texture2D::set_anisotropic_level(unsigned int level) {
   m_anisotropic_level = level;
}

void Texture2D::apply_filter_and_mipmap() const {
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
      m_filter == LINEAR ? GL_LINEAR : GL_NEAREST);

   if (m_mipmap == MIPMAP_LINEAR)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   else if (m_mipmap == MIPMAP_NEAREST)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
   else
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
         m_filter == LINEAR ? GL_LINEAR : GL_NEAREST);
}

void Texture2D::apply_texture_wrap() const {
   GLenum w = (m_wrap == REPEAT) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, w);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, w);
}

void Texture2D::apply_anisotropic_filter() const {
   if (strstr((const char*)glGetString(GL_EXTENSIONS),
      "GL_EXT_texture_filter_anisotropic")) {

      float max;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
      glTexParameterf(GL_TEXTURE_2D,
         GL_TEXTURE_MAX_ANISOTROPY_EXT,
         std::min<float>(m_anisotropic_level, max));
   }
}
