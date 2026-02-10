#pragma once

#include "object.h"
#include <string>
#include <vector>
#include <glm/mat4x4.hpp>

/* ===== ENUM ===== */

enum ENG_API Filter {
   NEAREST,
   LINEAR
};

enum ENG_API TextureMipmap {
   DISABLE,
   MIPMAP_NEAREST,
   MIPMAP_LINEAR
};

enum ENG_API TextureWrap {
   REPEAT,
   CLAMP_TO_EDGE
};

/**
 * @class Texture2D
 * @brief Classe unica per la gestione delle texture 2D OpenGL
 */
class ENG_API Texture2D : public Object {
public:
   Texture2D(const std::string& name, const std::string& file_path);
   ~Texture2D();

   Texture2D(const Texture2D&) = delete;
   Texture2D& operator=(const Texture2D&) = delete;

   void render(glm::mat4 model_view) override;

   void set_filter(Filter f, TextureMipmap m = DISABLE);
   void set_texture_wrap(TextureWrap wrap);
   void set_anisotropic_level(unsigned int level);

private:
   /* ===== CORE ===== */
   void load_texture();
   void generate_default_texture();

   /* ===== OPENGL STATE ===== */
   void apply_filter_and_mipmap() const;
   void apply_texture_wrap() const;
   void apply_anisotropic_filter() const;

private:
   unsigned int m_texture_id = 0;
   static unsigned int m_default_texture_id;

   std::string m_file_path;
   bool m_loaded = false;

   Filter m_filter = NEAREST;
   TextureMipmap m_mipmap = DISABLE;
   TextureWrap m_wrap = REPEAT;
   unsigned int m_anisotropic_level = 1;

   static std::vector<Texture2D*> m_textures;
};
