#define GLM_ENABLE_EXPERIMENTAL
#include "engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "omnidirectionalLight.h"
#include <GL/freeglut.h>
#include "node.h"

// --- Variabili Globali ---
Eng::Base& eng = Eng::Base::getInstance();
PerspectiveCamera* freeCamera = nullptr;
Node* lightBulb = nullptr;

float cameraSpeed = 0.5f; 

// --- Funzioni di supporto ---
void moveCameraLocal(const glm::vec3& delta) {
    if (!freeCamera) return;
    glm::mat4 m = freeCamera->getLocalMatrix();
    m = glm::translate(m, delta);
    freeCamera->setLocalMatrix(m);
}

// --- Callbacks ---
void keyboardCallback(unsigned char key, int mouseX, int mouseY) {
    switch (key) {
    case 'a': case 'A': moveCameraLocal(glm::vec3(-cameraSpeed, 0.0f, 0.0f)); break;
    case 'd': case 'D': moveCameraLocal(glm::vec3(cameraSpeed, 0.0f, 0.0f));  break;
    case 'w': case 'W': moveCameraLocal(glm::vec3(0.0f, 0.0f, -cameraSpeed)); break;
    case 's': case 'S': moveCameraLocal(glm::vec3(0.0f, 0.0f, cameraSpeed));  break;
    case 'q': case 'Q': moveCameraLocal(glm::vec3(0.0f, -cameraSpeed, 0.0f)); break;
    case 'e': case 'E': moveCameraLocal(glm::vec3(0.0f, cameraSpeed, 0.0f));  break;
    case 27: exit(0); break;
    }
}

void displayCallback() {
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    if (lightBulb) {
        float raggio = 10.0f;
        float velocita = 1.0f;
        float altezza = 35.0f;

        float posX = raggio * cos(time * velocita);
        float posZ = raggio * sin(time * velocita);

        // Recuperiamo la matrice originale che contiene SCALA e ROTAZIONE di 3ds Max
        glm::mat4 m = lightBulb->getLocalMatrix();

        // Modifichiamo SOLO la parte della traslazione (colonna 3)
        // m[3] è il vec4 della posizione: [x, y, z, w]
        m[3][0] = posX;
        m[3][1] = altezza;
        m[3][2] = posZ;

        lightBulb->setLocalMatrix(m);
    }

    eng.renderScene();
}

void reshapeCallback(int w, int h) {
    if (eng.getCamera()) eng.getCamera()->update_window_size(w, h);
}

// --- Main ---
int main(int argc, char** argv) {
    eng.init("Scene Viewer", 1024, 768, argc, argv);

    eng.registerDisplayCallback(displayCallback);
    eng.registerKeyboardCallback(keyboardCallback);
    eng.registerReshapeCallback(reshapeCallback);

    freeCamera = new PerspectiveCamera(
        "FreeCamera",
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 15.0f)),
        1024, 768, 0.1f, 200.0f, 60.0f
    );

    eng.addCamera(freeCamera);
    eng.setCamera(freeCamera);

    eng.loadScene("../assets/scena.ovo", "../assets/");


    
    Node* root = eng.getRoot();
    lightBulb = root->searchByName("lightbulb_01");
    //mettiamo la luce di 3ds max che corrisponde al lightbulb
    if (lightBulb) {
       
        Node* lightNode = root->searchByName("Omni001"); 

        if (lightNode) {
            // RESETIAMO l'offset locale.
            // mat4(1.0f) mette la luce esattamente sulla posizione del padre (lampadina)
            lightNode->setLocalMatrix(glm::mat4(1.0f));
        }
    }
    // --> <-- // 
   //gettiamo il nodo del tetto di 3ds max
    Node* roofNode = root->searchByName("w");

    if (roofNode) {
       // 2. Otteniamo la matrice locale attuale
       glm::mat4 currentMatrix = roofNode->getLocalMatrix();
       // 3. Applichiamo una rotazione di 180 gradi sull'asse X (o Y/Z a seconda dell'export)
       glm::mat4 flippedMatrix = glm::rotate(currentMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
       // 4. Applichiamo la nuova matrice
       roofNode->setLocalMatrix(flippedMatrix);
    }
    else {
        std::cerr << "Errore: Nodo 'w' non trovato nella scena!" << std::endl;
    }
    //finito giramento tetto
    eng.printText("WASD: Muovi Camera | Q/E: Su/Giu");
    eng.update();
    eng.free();

    return 0;
}