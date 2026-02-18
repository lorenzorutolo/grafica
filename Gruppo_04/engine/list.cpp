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
    clear();
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
    // Dealloca ogni singola struttura RenderNode
    for (RenderNode* rn : m_list)
    {
        delete rn;
    }
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
    unsigned int activated_lights = 0;
    auto it = m_list.begin();
    //render delle luci
    while (it != m_list.end())
    {
        Light* light = dynamic_cast<Light*>((*it)->m_object);
        if (light != nullptr) {
            if (activated_lights < 8) {
                glEnable(GL_LIGHT0 + activated_lights);
                light->render((*it)->m_mat, view);
                activated_lights++;
            }
            ++it;
        }
        else break;
    }

    // Salviamo la posizione dell'iteratore per le mesh
    auto mesh_start_it = it;

    glDisable(GL_LIGHTING);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f); // Ombra grigia semitrasparente

    glm::mat4 shadowMat = glm::mat4(1.0f);
    shadowMat[1][1] = 0.0f;
    //render delle ombre 
    for (auto shadow_it = mesh_start_it; shadow_it != m_list.end(); ++shadow_it)
    {
        Mesh* mesh = dynamic_cast<Mesh*>((*shadow_it)->m_object);
     
        if (mesh != nullptr)
        {
            std::string name = mesh->getName();

            //Escludi oggetti che non devono proiettare ombra a terra
            if (name == "w" || name == "C-Ext001" || name == "Plane007")
                continue;

            glm::mat4 lift = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 0.0f));
            mesh->render(view * lift * shadowMat * (*shadow_it)->m_mat);
          
        }
    }
    glEnable(GL_LIGHTING); // Riattiviamo la luce per gli oggetti reali

    //render mesh reali
    for (it = mesh_start_it; it != m_list.end(); ++it)
    {
        Mesh* mesh = dynamic_cast<Mesh*>((*it)->m_object);
        if (mesh != nullptr)
        {
            mesh->render(view * (*it)->m_mat);
        }
    }
}
