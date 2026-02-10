
   #include "node.h"
   #include <iostream>
   #include <algorithm> // per std::find
   #include <GL/freeglut.h>
   #include <GL/freeglut_ext.h>
   #include <GL/freeglut_std.h>
   #include <GL/freeglut_ucall.h>
   #include "glm/glm.hpp"
   #include <glm/gtc/type_ptr.hpp>



      //---------------------------------------------------------
      // Costruttore
      //---------------------------------------------------------
      ENG_API Node::Node(const std::string& name, glm::mat4 model_view) : Object(name), model_view { model_view }, parent { nullptr }
      {

      }

      //---------------------------------------------------------
      // Distruttore
      //---------------------------------------------------------
      ENG_API Node::~Node()
      {
        for (Node* child : children)
            delete child;
        children.clear();
      }

      void ENG_API Node::render(glm::mat4 M)
      {
          // Empty method for now
      }

      //---------------------------------------------------------
      // addChild
      //---------------------------------------------------------
      void ENG_API Node::addChild(Node* node)
      {
         if (node == nullptr)
            return;

         node->setParent(this);
         children.push_back(node);
      }

      //---------------------------------------------------------
      // removeChild
      //---------------------------------------------------------
      void ENG_API Node::removeChild(unsigned int childIndex)
      {
         if (childIndex < children.size()) {
            children.erase(children.begin() + childIndex);
         }
      }

      //---------------------------------------------------------
      // getParent / setParent
      //---------------------------------------------------------
      Node ENG_API* Node::getParent() {
         return parent;
      }

      void ENG_API Node::setParent(Node* node) {
         parent = node;
      }

      //---------------------------------------------------------
      // getChild / getNumberChildren
      //---------------------------------------------------------
      Node ENG_API* Node::getChild(unsigned int childIndex) {
         if (childIndex < children.size()) {
            return children[childIndex];
         }
         return nullptr;
      }

      unsigned int Node::getNumberChildren() {
         return static_cast<unsigned int>(children.size());
      }

      //---------------------------------------------------------
      // getLocalMatrix / setLocalMatrix
      //---------------------------------------------------------
      glm::mat4 ENG_API Node::getLocalMatrix() {
         return model_view;
      }

      void ENG_API Node::setLocalMatrix(const glm::mat4& newM) {
         model_view = newM;
      }

      //---------------------------------------------------------
      // getWCMatrix (World Coordinates Matrix)
      //---------------------------------------------------------
      glm::mat4 ENG_API Node::getWCMatrix() {
         if (parent != nullptr)
            return parent->getWCMatrix() * model_view;
         return model_view;
      }

      //---------------------------------------------------------
      // searchByName
      //---------------------------------------------------------
      Node ENG_API* Node::searchByName(std::string name) {
         if (getName() == name)
            return this;

         for (Node* child : children) {
            Node* result = child->searchByName(name);
            if (result)
               return result;
         }
         return nullptr;
      }

      //---------------------------------------------------------
      // searchById
      //---------------------------------------------------------
      Node ENG_API* Node::searchById(unsigned id) {
         if (getID() == id)
            return this;

         for (Node* child : children) {
            Node* result = child->searchById(id);
            if (result)
               return result;
         }
         return nullptr;
      }

