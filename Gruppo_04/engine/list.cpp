//////////////
// #INCLUDE //
//////////////

#include <vector>
#include <iostream>

#include "list.h"

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


ENG_API RenderList::RenderList(const std::string& name) : Object(name)
{
	//Empty Constructor
}

ENG_API RenderList::~RenderList()
{
	//Empty Deconstructor
}

void ENG_API RenderList::load_render_node(RenderNode* node)
{
	if (dynamic_cast<const Light*>(node->m_object) != nullptr)
		m_list.push_front(node);
	else
		m_list.push_back(node);
}

std::list<RenderNode*> ENG_API RenderList::get_render_list()
{
	return m_list;
}

void ENG_API RenderList::clear()
{
	m_list.clear();
}

void ENG_API RenderList::pass(Node* node, glm::mat4 parent_matrix)
{
	if (node == nullptr)			return;

	glm::mat4 matrix_world_coordinates = parent_matrix * node->getLocalMatrix();

	//We accept only light or mesh (no camera) concrete type
	if (dynamic_cast<const Camera*>(node) == nullptr && (dynamic_cast<const Light*>(node) != nullptr || dynamic_cast<const Node*>(node) != nullptr))
		load_render_node(new RenderNode{node, matrix_world_coordinates});

	if(node->getNumberChildren() > 0)
	{
        for (int i = 0; i < node->getNumberChildren(); i++) {
            pass(node->getChild(i), matrix_world_coordinates);
        }

	}
}

void ENG_API RenderList::render(glm::mat4 view)
{
	std::vector<RenderNode*> lights;
	unsigned int activated_lights = 0;
   std::vector<RenderNode*> floor_meshes;

	for(auto i = m_list.begin(); i != m_list.end(); i++ )
	{
		if(dynamic_cast<Light*>((*i)->m_object) != nullptr)
		{
			lights.push_back(*i);
		}
		else
		{
			break;
		}
	}

	int number_of_lights = lights.size();

	//remove the lights from the m_list
	for (auto i = lights.begin(); i != lights.end(); i++)
		m_list.remove(*i);


	std::vector<RenderNode*> solid_meshes;

	for(auto i = m_list.begin(); i != m_list.end(); i++ )
	{
		if(dynamic_cast<Mesh*>((*i)->m_object) != nullptr && dynamic_cast<Mesh*>((*i)->m_object)->get_material()->get_transparency() == 1.0f)
		{
         if ((*i)->m_object->getName() == "Floor") {
            floor_meshes.push_back(*i);
         } else {
            solid_meshes.push_back(*i);
         }
		}
	}

	int number_of_solid_meshes = solid_meshes.size();

	//remove the lights from the m_list
	for (auto i = solid_meshes.begin(); i != solid_meshes.end(); i++)
		m_list.remove(*i);
   for (auto i = floor_meshes.begin(); i != floor_meshes.end(); i++)
      m_list.remove(*i);

   // Build list of meshes to reflect (all meshes except the reflector itself)
   std::vector<RenderNode*> reflection_meshes = solid_meshes;
   reflection_meshes.insert(reflection_meshes.end(), m_list.begin(), m_list.end());
   reflection_meshes.insert(reflection_meshes.end(), floor_meshes.begin(), floor_meshes.end());
   std::vector<RenderNode*> reflection_lights = lights;

   auto buildPlaneFromFloor = [](RenderNode* floorNode) {
      // Assume floor normal points up in local space (Y axis)
      glm::vec3 localNormal(0.0f, 1.0f, 0.0f);
      glm::vec4 worldNormal4 = floorNode->m_mat * glm::vec4(localNormal, 0.0f);
      glm::vec3 worldNormal = glm::normalize(glm::vec3(worldNormal4));
      glm::vec3 point = glm::vec3(floorNode->m_mat[3]);
      float d = -glm::dot(worldNormal, point);
      return glm::vec4(worldNormal, d);
   };

   auto reflectionMatrix = [](const glm::vec4& plane) {
      glm::vec4 p = plane;
      glm::vec3 n = glm::vec3(p);
      float a = n.x, b = n.y, c = n.z, d = p.w;
      glm::mat4 R(1.0f);
      R[0][0] = 1 - 2 * a * a; R[1][0] = -2 * a * b;   R[2][0] = -2 * a * c;   R[3][0] = -2 * a * d;
      R[0][1] = -2 * a * b;   R[1][1] = 1 - 2 * b * b; R[2][1] = -2 * b * c;   R[3][1] = -2 * b * d;
      R[0][2] = -2 * a * c;   R[1][2] = -2 * b * c;   R[2][2] = 1 - 2 * c * c; R[3][2] = -2 * c * d;
      R[0][3] = 0.0f;         R[1][3] = 0.0f;         R[2][3] = 0.0f;         R[3][3] = 1.0f;
      return R;
   };

   if (!floor_meshes.empty()) {
      glm::vec4 plane = buildPlaneFromFloor(floor_meshes.front());
      glm::mat4 reflectMat = reflectionMatrix(plane);

      if (glIsEnabled(GL_CULL_FACE))
         glDisable(GL_CULL_FACE);

      glEnable(GL_STENCIL_TEST);
      glClear(GL_STENCIL_BUFFER_BIT);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

      for (auto* floorNode : floor_meshes) {
         floorNode->m_object->render(view * floorNode->m_mat);
      }

      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glStencilFunc(GL_EQUAL, 1, 0xFF);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

      GLint oldFrontFace = GL_CCW;
      glGetIntegerv(GL_FRONT_FACE, &oldFrontFace);
      glFrontFace(GL_CW); // mirrored winding

      // Render mirrored lights first
      unsigned int refl_light_id = 0;
      for (auto* l : reflection_lights) {
         if (refl_light_id >= 8) break;
         glEnable(GL_LIGHT0 + refl_light_id);
         dynamic_cast<Light*>(l->m_object)->render(reflectMat * l->m_mat, view);
         refl_light_id++;
      }

      for (auto* meshNode : reflection_meshes) {
         if (meshNode->m_object->getName() == "Floor") {
            continue;
         }
         meshNode->m_object->render(view * reflectMat * meshNode->m_mat);
      }

      glFrontFace(oldFrontFace);
      glDisable(GL_STENCIL_TEST);

      if (glIsEnabled(GL_CULL_FACE))
         glEnable(GL_CULL_FACE);
   }

	for (auto &l : lights) {
		if (activated_lights < 8) {
			glEnable(GL_LIGHT0 + dynamic_cast<Light*>(l->m_object)->get_light_id());
			dynamic_cast<Light*>(l->m_object)->render(l->m_mat, view);
			activated_lights++;
		}
	}

	for (auto &l : solid_meshes) {
		l->m_object->render(view * l->m_mat);
	}

   for (auto& f : floor_meshes) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_FALSE);
      if (auto* mesh = dynamic_cast<Mesh*>(f->m_object)) {
         auto* mat = mesh->get_material();
         float oldAlpha = mat ? mat->get_transparency() : 1.0f;
         if (mat) mat->set_transparency(0.35f);
         f->m_object->render(view * f->m_mat);
         if (mat) mat->set_transparency(oldAlpha);
      } else {
         f->m_object->render(view * f->m_mat);
      }
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
   }



	for (auto& m : m_list) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m->m_object->render(view * m->m_mat);
		glDisable(GL_BLEND);
	}



}
