#include <cassert>
#include <iostream>
#include <vector>

#include "list.h"
#include "material.h"
#include "node.h"
#include "light.h"

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct DummyLight : public Light {
   DummyLight(const std::string& name, const glm::mat4& model)
      : Light(name, model, glm::vec3(0.1f), glm::vec3(0.2f), glm::vec3(0.3f)) {}

   void render(glm::mat4, const glm::mat4&) override {}
};

bool mat4AlmostEqual(const glm::mat4& a, const glm::mat4& b, float eps = 0.0001f) {
   for (int i = 0; i < 4; ++i) {
      if (!glm::all(glm::epsilonEqual(a[i], b[i], eps))) {
         return false;
      }
   }
   return true;
}

void test_node_hierarchy_and_search() {
   Node root("root", glm::mat4(1.0f));
   auto* child = new Node("child", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
   auto* grandchild = new Node("grandchild", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f)));

   child->addChild(grandchild);
   root.addChild(child);

   assert(root.getNumberChildren() == 1);
   assert(root.getChild(0) == child);
   assert(child->getParent() == &root);

   glm::mat4 expectedChild = root.getLocalMatrix() * child->getLocalMatrix();
   glm::mat4 expectedGrandChild = expectedChild * grandchild->getLocalMatrix();

   assert(mat4AlmostEqual(child->getWCMatrix(), expectedChild));
   assert(mat4AlmostEqual(grandchild->getWCMatrix(), expectedGrandChild));
   assert(root.searchByName("grandchild") == grandchild);
   assert(root.searchById(grandchild->getID()) == grandchild);
}

void test_remove_child_bounds() {
   Node root("root", glm::mat4(1.0f));
   auto* first = new Node("first", glm::mat4(1.0f));
   auto* second = new Node("second", glm::mat4(1.0f));

   root.addChild(first);
   root.addChild(second);

   assert(root.getNumberChildren() == 2);
   root.removeChild(5);
   assert(root.getNumberChildren() == 2);

   root.removeChild(0);
   assert(root.getNumberChildren() == 1);
   assert(root.getChild(0) == second);

   delete first;
}

void test_render_list_pass_orders_and_transforms() {
   RenderList renderList("render_list");
   Node root("root", glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f)));
   auto* light = new DummyLight("light", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
   auto* node = new Node("node", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)));

   root.addChild(light);
   root.addChild(node);

   renderList.pass(&root, glm::mat4(1.0f));
   auto renderNodes = renderList.get_render_list();

   assert(renderNodes.size() == 3);

   auto it = renderNodes.begin();
   RenderNode* first = *it++;
   RenderNode* second = *it++;
   RenderNode* third = *it;

   assert(first->m_object == light);
   assert(second->m_object == &root);
   assert(third->m_object == node);

   glm::mat4 expectedRoot = root.getLocalMatrix();
   glm::mat4 expectedLight = expectedRoot * light->getLocalMatrix();
   glm::mat4 expectedNode = expectedRoot * node->getLocalMatrix();

   assert(mat4AlmostEqual(second->m_mat, expectedRoot));
   assert(mat4AlmostEqual(first->m_mat, expectedLight));
   assert(mat4AlmostEqual(third->m_mat, expectedNode));

   for (auto* rn : renderNodes) {
      delete rn;
   }
   renderList.clear();
}

void test_material_property_roundtrip() {
   Material material(
      "mat",
      glm::vec4(0.1f, 0.2f, 0.3f, 1.0f),
      glm::vec4(0.2f, 0.3f, 0.4f, 1.0f),
      glm::vec4(0.3f, 0.4f, 0.5f, 1.0f),
      glm::vec4(0.4f, 0.5f, 0.6f, 1.0f),
      8.0f,
      0.75f);

   assert(glm::all(glm::epsilonEqual(material.get_emission(), glm::vec3(0.1f, 0.2f, 0.3f), 0.0001f)));
   assert(glm::all(glm::epsilonEqual(material.get_ambient(), glm::vec3(0.2f, 0.3f, 0.4f), 0.0001f)));
   assert(glm::all(glm::epsilonEqual(material.get_diffuse(), glm::vec3(0.3f, 0.4f, 0.5f), 0.0001f)));
   assert(glm::all(glm::epsilonEqual(material.get_specular(), glm::vec3(0.4f, 0.5f, 0.6f), 0.0001f)));
   assert(glm::epsilonEqual(material.get_shininess(), 8.0f, 0.0001f));
   assert(glm::epsilonEqual(material.get_transparency(), 0.75f, 0.0001f));

   material.set_emission(glm::vec3(1.0f, 0.0f, 0.0f));
   material.set_ambient(glm::vec3(0.0f, 1.0f, 0.0f));
   material.set_diffuse(glm::vec3(0.0f, 0.0f, 1.0f));
   material.set_specular(glm::vec3(0.5f, 0.5f, 0.5f));
   material.set_shininess(32.0f);
   material.set_transparency(0.5f);

   assert(glm::all(glm::epsilonEqual(material.get_emission(), glm::vec3(1.0f, 0.0f, 0.0f), 0.0001f)));
   assert(glm::all(glm::epsilonEqual(material.get_ambient(), glm::vec3(0.0f, 1.0f, 0.0f), 0.0001f)));
   assert(glm::all(glm::epsilonEqual(material.get_diffuse(), glm::vec3(0.0f, 0.0f, 1.0f), 0.0001f)));
   assert(glm::all(glm::epsilonEqual(material.get_specular(), glm::vec3(0.5f, 0.5f, 0.5f), 0.0001f)));
   assert(glm::epsilonEqual(material.get_shininess(), 32.0f, 0.0001f));
   assert(glm::epsilonEqual(material.get_transparency(), 0.5f, 0.0001f));
}

int main() {
   test_node_hierarchy_and_search();
   test_remove_child_bounds();
   test_render_list_pass_orders_and_transforms();
   test_material_property_roundtrip();

   std::cout << "All engine tests passed\n";
   return 0;
}
