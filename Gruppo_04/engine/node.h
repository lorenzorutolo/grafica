#pragma once

//INCLUDE
#include <string>
#include "object.h"
#include "glm/glm.hpp"
#include <vector>
#include <stdio.h>


class ENG_API Node : public Object {

protected:

    /**
	* \var model_view
	* \brief The node's matrix in Object Coordinates.
	*
	* Member variable that stores the node's children.
	*/
    glm::mat4 model_view;

    /**
	* \var parent
	* \brief Current parent node.
	*
	* Member variable that stores the node's parent.
	*/
    Node* parent;

    /**
	* \var children
	* \brief Current children nodes.
	*
	* Member variable that stores the node's children.
	*/
    std::vector<Node*> children;

public:

    /** ADD OTHER COMMENTS FOR DOXYFILE ~Mattia */

    Node(const std::string& name, glm::mat4 model_view);                    // costruttore di default
    virtual ~Node();
    void render(glm::mat4 M = glm::mat4(1.0f)) override;
    void addChild(Node* node);
    glm::mat4 getLocalMatrix();
    void setLocalMatrix(const glm::mat4& newM);
    Node* searchByName(std::string name);
    Node* searchById(unsigned id);
    unsigned int getNumberChildren();
    Node* getChild(unsigned int child);
    Node* getParent();
    void setParent(Node* node);
    void removeChild(unsigned int child);
    glm::mat4 getWCMatrix();
};

