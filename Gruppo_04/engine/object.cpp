// Author: Niccolo Pasqualetti, Ivan Martinetti, Mattia Pasquini
#include "object.h"

//using namespace Eng;

unsigned int ENG_API Object::ID_counter = 0;

ENG_API Object::Object(const std::string& name) :
		ID{ ++ID_counter },
		name{ name }
{
		//Empty contructor
}

unsigned int ENG_API Object::getID() {
    return ID;
};

void ENG_API Object::setID(const unsigned int newID) {
    ID = newID;
};

std::string ENG_API Object::getName() {
    return name;
};

void ENG_API Object::setName(const std::string& newName) {
    name = newName;
};
