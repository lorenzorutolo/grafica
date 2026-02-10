// Author: Niccolo Pasqualetti, Ivan Martinetti, Mattia Pasquini
//////////////
// #INCLUDE //
//////////////

	// Base class include
#include "spotLight.h"

//FreeGLUT:
#include <GL/freeglut.h>

//GLM:
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>



ENG_API SpotLight::SpotLight(const std::string& name, glm::mat4 model_view, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction, float constant_att, float linear_att, float quadratic_att, float cutoff) : AttenuationLight(name, model_view, ambient, diffuse, specular, constant_att, linear_att, quadratic_att), m_direction{ direction }, m_cutoff{ cutoff }
{
	//empty constructor

}

void ENG_API SpotLight::render(glm::mat4 M, const glm::mat4& C)
{
   GLenum id = GL_LIGHT0 + m_light_id;

   // Se la luce è spenta, assicurati che sia DISABILITATA e basta
   if (!isOn) {
      glDisable(id);
      return;
   }

   // Luce accesa: abilita e configura
   glEnable(id);

   glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f);

   glLoadMatrixf(glm::value_ptr(C * M));

   glLightfv(id, GL_POSITION, glm::value_ptr(position));
   glLightf(id, GL_SPOT_CUTOFF, m_cutoff);
   glLightfv(id, GL_SPOT_DIRECTION, glm::value_ptr(m_direction));

   glLightf(id, GL_CONSTANT_ATTENUATION, m_constant_attenuation);
   glLightf(id, GL_LINEAR_ATTENUATION, m_linear_attenuation);
   glLightf(id, GL_QUADRATIC_ATTENUATION, m_quadratic_attenuation);

   glLightfv(id, GL_AMBIENT, glm::value_ptr(m_ambient));
   glLightfv(id, GL_DIFFUSE, glm::value_ptr(m_diffuse));
   glLightfv(id, GL_SPECULAR, glm::value_ptr(m_specular));
}


glm::vec3 ENG_API SpotLight::get_direction() const
{
	return m_direction;
}

void ENG_API SpotLight::set_direction(const glm::vec3& direction)
{
	m_direction = direction;
}

float ENG_API SpotLight::get_cutoff() const
{
	return m_cutoff;
}

void ENG_API SpotLight::set_cutoff(float cutoff)
{
	m_cutoff = cutoff;
}

