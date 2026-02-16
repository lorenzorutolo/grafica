#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

//INCLUDE
#include "engine_api.h"

#include <string>
#include "glm/glm.hpp"


//CLASS
class ENG_API Object {

protected:

    /**
     * \var ID
     * \brief Unique object identifier.
     *
     * A member variable that stores the unique identifier assigned to the object. Identifiers start from 1 (0 is not a valid identifier).
     */
    unsigned int ID;

    static unsigned int ID_counter;

    /**
     * \var name
     * \brief The name of the object.
     *
     * A member variable that stores the name of the object.
     */
    std::string name;


public:

    Object(const std::string& name);

    /**
     * \brief Renders the object
     *
     * A pure virtual function that must be overriden by derivated classes to define the rendering behaviour
     *
     * \param M A matrix expressing the Model-View transformation.
     * \param C A matrix that (probably ???) expressing the Camera
     */
    virtual void render(glm::mat4 M = glm::mat4(1.0f)) = 0;


    /**
     * \brief Gets the object's unique ID
     *
     * Retrieves the unique identifier assigned to the object.
     *
     * \return The object's unique identifier as an unsigned integer.
     */
    unsigned int getID();


    /**
     * \brief Sets the object's unique ID
     *
     * BUT IT SHOULD BE REMOVED BECAUSE IT'S USELESS, I WOULD RATHER MAKING A STATIC ATTRIBUTE AS A COUNTER ~Mattia
     */
    void setID(const unsigned int newID);


    /**
     * \brief Gets the object's name.
     *
     * Retrieves the current name of the object.
     *
     * \return The name of the object as a string.
     */
    std::string getName();


    /**
     * \brief Sets the object's name.
     *
     * Updates the name of the object to the specified value.
     *
     * \param name The new name of the object. It's a string value.
     */
    void setName(const std::string& newName);

};

#endif // OBJECT_H_INCLUDED

