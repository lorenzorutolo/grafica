#define GLM_ENABLE_EXPERIMENTAL
#include "engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "omnidirectionalLight.h"
#include <GL/freeglut.h>
#include "node.h"

// --- Variabili Globali ---
// --- Variabili Logica Hanoi ---
std::vector<Node*> pegs[3]; // Rappresenta i 3 pioli
int sourcePeg = -1;         // Memorizza il primo tasto premuto (-1 significa "nessuna selezione")

// Parametri di posizionamento
float CALIBRATED_BASE_HEIGHT = 0.0f;
float CALIBRATED_DISK_OFFSET = 0.0f;

Eng::Base& eng = Eng::Base::getInstance();

// setup freecamera
PerspectiveCamera* freeCamera = nullptr;
float cameraSpeed = 0.5f;


// setup camera fissa 
glm::vec3 posCam = glm::vec3(-30.0f, 25.0f, 8.0f);
glm::mat4 cameraM = glm::rotate(glm::translate(glm::mat4(1.0f), posCam), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
PerspectiveCamera* cameraFissa = new PerspectiveCamera("FreeCamera", cameraM, 1024, 768, 0.1f, 200.0f, 60.0f);

// setup nodi luce
Node* lightBulb = nullptr;
Node* DeskLight = nullptr;
SpotLight* lampSpotLight = nullptr;
OmniDirectionalLight* myOmni = nullptr; // <--- AGGIUNGI QUESTO
Node* oggetto = nullptr;

// cilindri
Node* root = nullptr;
Node* cylinders[3] = { nullptr, nullptr, nullptr };
Node* selectedCylinder = nullptr;
bool isBlinkVisible = true;
float lastBlinkTime = 0.0f;
float blinkInterval = 0.3f; // Velocità del lampeggio (0.3 secondi)
glm::mat4 originalCylinderMatrix;

// Flag di stato
bool isSceneInitialized = false;

// --- Funzioni di supporto ---

void setupHanoiLogic() {
    for (int i = 0; i < 3; i++) pegs[i].clear();

    // 1. Identifica i primi due dischi per calibrare le altezze
    Node* d1 = root->searchByName("Torus007"); // Il più grande (sotto)
    Node* d2 = root->searchByName("Torus006"); // Il secondo (sopra)

    if (d1 && d2) {
        // Leggiamo l'altezza reale (proviamo con l'indice [3][1] che è Y in OpenGL)
        // Se non funziona, cambieremo in [3][2]
        CALIBRATED_BASE_HEIGHT = d1->getLocalMatrix()[3][1];
        CALIBRATED_DISK_OFFSET = abs(d2->getLocalMatrix()[3][1] - d1->getLocalMatrix()[3][1]);
        std::cout << "Calibrazione completata: Base=" << CALIBRATED_BASE_HEIGHT
            << " Offset=" << CALIBRATED_DISK_OFFSET << std::endl;
    }

    // Carichiamo dal più grande (007) al più piccolo (001) 
    // così che nel vettore il 001 (il top) sia l'ultimo elemento (.back())
    for (int i = 7; i >= 1; i--) {
        std::string name = "Torus00" + std::to_string(i);
        Node* disk = root->searchByName(name.c_str());
        if (disk) pegs[0].push_back(disk);
    }
}

// per muovere la free cam
void moveCameraLocal(const glm::vec3& delta) {
    if (!freeCamera) return;
    glm::mat4 m = freeCamera->getLocalMatrix();
    m = glm::translate(m, delta);
    freeCamera->setLocalMatrix(m);
}

// Ripristina la matrice originale e ferma il lampeggio
void resetSelectedCylinder() {
    if (selectedCylinder) {
        // Ripristina la scala originale
        selectedCylinder->setLocalMatrix(originalCylinderMatrix);
        std::cout << "Reset visuale cilindro." << std::endl;
    }
    // Resetta le variabili di stato visivo
    selectedCylinder = nullptr;
    isBlinkVisible = true;
}

// Seleziona il cilindro per il lampeggio
void selectCylinder(int index) {
    if (index < 0 || index >= 3 || !cylinders[index]) return;

    // Se c'era già una selezione, resettala
    resetSelectedCylinder();

    // Imposta il nuovo cilindro
    selectedCylinder = cylinders[index];
    originalCylinderMatrix = selectedCylinder->getLocalMatrix();
    lastBlinkTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

//muovere il disco 
void moveDisk(int from, int to) {
    if (pegs[from].empty()) return;

    Node* diskToMove = pegs[from].back();

    // Controllo Hanoi
    if (!pegs[to].empty()) {
        Node* topDiskDest = pegs[to].back();
        if (diskToMove->getName() > topDiskDest->getName()) {
            eng.printText("non puoi posizionare un");
            eng.printText("disco grande su piccolo!");
            // Nota: Non serve resettare sourcePeg qui, lo fa la keyboardCallback
            return;
        }
    }

    // Logica spostamento vettori
    pegs[from].pop_back();

    int numDisksAlreadyThere = pegs[to].size();
    float targetHeight = CALIBRATED_BASE_HEIGHT + (numDisksAlreadyThere * CALIBRATED_DISK_OFFSET);

    pegs[to].push_back(diskToMove);

    // Applicazione trasformazione fisica
    glm::mat4 m = diskToMove->getLocalMatrix();
    float targetX = cylinders[to]->getLocalMatrix()[3][0];
    float targetZ = cylinders[to]->getLocalMatrix()[3][2];

    m[3][0] = targetX;
    m[3][1] = targetHeight;
    m[3][2] = targetZ;
    diskToMove->setLocalMatrix(m);

    // Rimosso resetSelectedCylinder() da qui perché ora è gestito nella input
}

// --- Logica di Inizializzazione Luci e Posizioni ---

//setup componenti della scena
void initializeSceneComponents() {
    setupHanoiLogic();
    if (!DeskLight || !oggetto || !cylinders[0]) return;

    //posizionare la lampadina nella posizione corretta
    //Ottieni la worldMatrix del paralume
    glm::mat4 paralumeWorld = oggetto->getWCMatrix();
    //Ottieni la localMatrix della lampadina
    glm::mat4 lampLocal = DeskLight->getLocalMatrix();
    //Sovrascrivi la parte della traslazione (colonna 3) della lampadina con quella del paralume
    lampLocal[3] = paralumeWorld[3];
    DeskLight->setLocalMatrix(lampLocal);

    // Setup Spotlight della lampada da tavolo
    // 1. Otteniamo le matrici World di entrambi
    glm::mat4 lampWorldMatrix = DeskLight->getWCMatrix();
    glm::mat4 targetWorldMatrix = cylinders[0]->getWCMatrix();

    // 2. Estraiamo le posizioni (colonna 3 della matrice)
    glm::vec3 lampPos = glm::vec3(lampWorldMatrix[3]);
    glm::vec3 targetPos = glm::vec3(targetWorldMatrix[3]);

    // 3. Calcoliamo il vettore direzione e lo normalizziamo
    glm::vec3 worldDirection = glm::normalize(targetPos - lampPos);
    glm::mat3 invRotation = glm::transpose(glm::mat3(DeskLight->getWCMatrix()));
    glm::vec3 localDirection = invRotation * worldDirection;

    // 4. Creazione SpotLight
    lampSpotLight = new SpotLight(
        "LampDeskLight",
        glm::mat4(1.0f),
        glm::vec3(0.2f),             // Ambient leggermente alzato (da 0.1 a 0.2)
        glm::vec3(0.0f, 0.0f, 3.0f), // Diffuse blu molto più intenso (da 1.0 a 3.0)
        glm::vec3(2.0f),             // Specular aumentato per riflessi più vividi
        localDirection,
        1.0f, 0.0f, 0.0f,
        15.0f
    );
    DeskLight->addChild(lampSpotLight);

    // Setup Luce Omni soffitto (lightbulb_01)
    if (lightBulb) {
        myOmni = new OmniDirectionalLight(
            "PowerfulOmni",
            glm::mat4(1.0f),
            glm::vec3(0.1f),             // Ambient ridotto (da 0.2 a 0.1)
            glm::vec3(0.8f, 0.8f, 0.8f), // Diffuse abbassato (da 1.5 a 0.8) per non accecare
            glm::vec3(0.8f, 0.8f, 0.8f), // Specular proporzionalmente ammorbidito
            1.0f, 0.0f, 0.0f
        );
        lightBulb->addChild(myOmni);
    }

    isSceneInitialized = true;
}

//resetta il game alla posizione iniziale
void resetGame() {
    eng.printText("Reset della partita...");

    // 1. Pulisci la logica dei pioli
    for (int i = 0; i < 3; i++) {
        pegs[i].clear();
    }

    // 2. Re-inserisci i dischi nel primo piolo (pegs[0]) e spostali fisicamente
    // Usiamo lo stesso ordine della setupHanoiLogic (Torus001 -> Torus007)
    for (int i = 7; i >= 1; i--) {
        std::string name = "Torus00" + std::to_string(i);
        Node* disk = root->searchByName(name.c_str());

        if (disk) {
            // Aggiungi al vettore logico
            pegs[0].push_back(disk);

            // Calcola posizione fisica
            glm::mat4 m = disk->getLocalMatrix();
            float targetX = cylinders[0]->getLocalMatrix()[3][0];
            float targetZ = cylinders[0]->getLocalMatrix()[3][2];

            // L'altezza dipende dalla posizione nel vettore (0 per il primo, 1 per il secondo, ecc.)
            float targetHeight = CALIBRATED_BASE_HEIGHT + ((pegs[0].size() - 1) * CALIBRATED_DISK_OFFSET);

            m[3][0] = targetX;
            m[3][1] = targetHeight;
            m[3][2] = targetZ;

            disk->setLocalMatrix(m);
        }
    }

    // 3. Reset delle variabili di selezione e lampeggio
    sourcePeg = -1;
    resetSelectedCylinder();
    selectedCylinder = nullptr;

    std::cout << "Gioco resettato correttamente." << std::endl;
}

// --- Callbacks ---

void keyboardCallback(unsigned char key, int mouseX, int mouseY) {

    // --- LOGICA DI GIOCO (HANOI) ---
    if (key >= '1' && key <= '3') {
        int pegIndex = key - '1'; // Converte char '1' in int 0

        // FASE 1: SELEZIONE SORGENTE
        if (sourcePeg == -1) {
            if (pegs[pegIndex].empty()) {
                eng.printText("Errore: Il piolo selezionato e' vuoto!");
            }
            else {
                sourcePeg = pegIndex;
                std::cout << "Sorgente selezionata: Piolo " << sourcePeg + 1 << std::endl;

                // >>> QUI ATTIVIAMO IL LAMPEGGIO DEL CILINDRO SORGENTE <<<
                selectCylinder(sourcePeg);
            }
        }
        // FASE 2: SELEZIONE DESTINAZIONE
        else {
            int destPeg = pegIndex;

            // Prima di fare qualsiasi mossa, SPEGNIAMO IL LAMPEGGIO
            resetSelectedCylinder();

            if (destPeg == sourcePeg) {
                std::cout << "Selezione annullata (Sorgente == Destinazione)." << std::endl;
            }
            else {
                // Tenta il movimento
                moveDisk(sourcePeg, destPeg);
            }

            // Resetta la logica per la prossima mossa
            sourcePeg = -1;
        }

        // Return anticipato per evitare che il tasto venga processato dallo switch sotto
        return;
    }

    // --- COMANDI GENERALI ---
    switch (key) {
    case 'a': case 'A': moveCameraLocal(glm::vec3(-cameraSpeed, 0.0f, 0.0f)); break;
    case 'd': case 'D': moveCameraLocal(glm::vec3(cameraSpeed, 0.0f, 0.0f));  break;
    case 'w': case 'W': moveCameraLocal(glm::vec3(0.0f, 0.0f, -cameraSpeed)); break;
    case 's': case 'S': moveCameraLocal(glm::vec3(0.0f, 0.0f, cameraSpeed));  break;
    case 'q': case 'Q': moveCameraLocal(glm::vec3(0.0f, -cameraSpeed, 0.0f)); break;
    case 'e': case 'E': moveCameraLocal(glm::vec3(0.0f, cameraSpeed, 0.0f));  break;
    case 't': {
        if (eng.getCamera() == freeCamera) eng.setCamera(cameraFissa);
        else eng.setCamera(freeCamera);
        break;
    }
    case 'r': case 'R':
        resetGame(); 
        break;
    case 'l': case 'L': 
        if (myOmni) {
            myOmni->toggleLight();
            if (myOmni->get_ambient() == glm::vec3(0.0f)) { // un trucco per sapere se è off, oppure stampi e basta
                std::cout << "Luce Omnidirezionale commutata." << std::endl;
            }
        }
        break;
    case '0':
        resetSelectedCylinder();
        sourcePeg = -1;
        std::cout << "Selezione resettata manualmente." << std::endl;
        break;
    case 27: exit(0); break;
    }
}

void displayCallback() {
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // Inizializzazione post-caricamento (eseguita una sola volta quando le matrici sono pronte)
    if (!isSceneInitialized) {
        initializeSceneComponents();
    }

    if (selectedCylinder) {
        if (time - lastBlinkTime > blinkInterval) {
            isBlinkVisible = !isBlinkVisible;
            lastBlinkTime = time;
            glm::mat4 m = selectedCylinder->getLocalMatrix();
            if (!isBlinkVisible) m = glm::scale(m, glm::vec3(0.0001f));
            else m = glm::scale(m, glm::vec3(10000.0f));
            selectedCylinder->setLocalMatrix(m);
        }
    }

    if (lightBulb) {
        float raggio = 10.0f, velocita = 1.0f, altezza = 43.0f;
        float posX = raggio * cos(time * velocita);
        float posZ = raggio * sin(time * velocita);
        glm::mat4 m = lightBulb->getLocalMatrix();
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

void rotateCameraFissa(float angleDeg, const glm::vec3& axis, bool local = true) {
    if (!freeCamera) return;
    glm::mat4 m = freeCamera->getLocalMatrix();
    glm::vec4 position = m[3];
    m[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if (local) m = glm::rotate(m, glm::radians(angleDeg), axis);
    else m = glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), axis) * m;
    m[3] = position;
    freeCamera->setLocalMatrix(m);
}

void specialCallback(int key, int mouseX, int mouseY) {
    float rotationSpeed = 2.5f;
    switch (key) {
    case GLUT_KEY_LEFT:  rotateCameraFissa(rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), false); break;
    case GLUT_KEY_RIGHT: rotateCameraFissa(-rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), false); break;
    case GLUT_KEY_UP:    rotateCameraFissa(rotationSpeed, glm::vec3(1.0f, 0.0f, 0.0f), true); break;
    case GLUT_KEY_DOWN:  rotateCameraFissa(-rotationSpeed, glm::vec3(1.0f, 0.0f, 0.0f), true); break;
    }
}

// --- Main ---

int main(int argc, char** argv) {
    // 1. Inizializzazione
    eng.init("Scene Viewer", 1024, 768, argc, argv);

    // setup callback
    eng.registerDisplayCallback(displayCallback);
    eng.registerKeyboardCallback(keyboardCallback);
    eng.registerReshapeCallback(reshapeCallback);
    eng.registerSpecialCallback(specialCallback);

    //Setup Camera
    glm::vec3 camPos = glm::vec3(-55.0f, 35.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::translate(glm::mat4(1.0f), camPos);
    cameraMatrix = glm::rotate(cameraMatrix, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    freeCamera = new PerspectiveCamera("FreeCamera", cameraMatrix, 1024, 768, 0.1f, 200.0f, 60.0f);
    eng.addCamera(freeCamera);
    eng.setCamera(freeCamera);

    //Caricamento Scena
    eng.loadScene("../assets/scena.ovo", "../assets/");
    root = eng.getRoot();
    if (!root) {
        std::cerr << "Errore: Root non trovata!" << std::endl;
        return -1;
    }

    // Ricerca nodi
    lightBulb = root->searchByName("lightbulb_01");
    DeskLight = root->searchByName("lampadina");
    oggetto = root->searchByName("Oggetto_1");

    // cilindri
    cylinders[0] = root->searchByName("Cylinder001");
    cylinders[1] = root->searchByName("Cylinder002");
    cylinders[2] = root->searchByName("Cylinder003");

    //Avvio Engine
    eng.printText("WASD: Muovi Camera");
    eng.printText("Frecce: Ruota Camera");
    eng.printText("Premi R per resettare il gioco");
    eng.printText("Premi T per telecamera fissa");
    eng.printText("Premi T per telecamera libera");
    eng.printText("Premi L per spegnere la luce");
    eng.printText("Premi L per accendere la luce");

    // Il primo update calcola le matrici, permettendo l'inizializzazione nel displayCallback
    eng.update();

    eng.free();
    return 0;
}