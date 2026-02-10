
// Main include:
#include "engine.h"
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>
#include <FreeImage.h>

// C/C++:
#include <iostream>
#include <source_location>


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Base class reserved structure (using PIMPL/Bridge design pattern https://en.wikipedia.org/wiki/Opaque_pointer).
 */
struct Eng::Base::Reserved
{
   // Flags:
   bool initFlag;


   /**
    * Constructor.
    */
   Reserved() : initFlag{ false }
   {
   }
};



////////////////////////
// BODY OF CLASS Base //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>())
{
#ifdef _DEBUG
   std::cout << "[+] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Base::~Base()
{
#ifdef _DEBUG
   std::cout << "[-] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets a reference to the (unique) singleton instance.
 * @return reference to singleton instance
 */
Eng::Base ENG_API& Eng::Base::getInstance()
{
   static Base instance;
   return instance;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Init internal components.
 * @return TF
 */
bool ENG_API Eng::Base::init(const char* title, unsigned int width, unsigned int height, int argc, char* argv[])
{
   if (reserved->initFlag)
   {
      std::cout << "ERROR: engine already initialized" << std::endl;
      return false;
   }

   // FreeGLUT can parse command-line params, in case:
   if (!reserved->initFlag) {
      glutInit(&argc, argv);  // Deve essere chiamato per primo
      std::cout << "ENGINE INITIALIZED" << std::endl;
   }

   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowPosition(100, 100);

   m_window_width = width;
   m_window_height = height;
   glutInitWindowSize(width, height);  // Imposta una dimensione predefinita per la finestra

   // Create the window with a specific title:
   this->windowId = glutCreateWindow(title);
   std::cout << "   renderer . . : " << glGetString(GL_RENDERER) << std::endl;

   // Set some optional flags:
   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS); //quando la finestra si chiude il glutmainloop si interrompe
   glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f); // default 0

   glEnable(GL_DEPTH_TEST); //attiva z buffer
   glEnable(GL_LIGHTING); //attiva le luci
   glEnable(GL_NORMALIZE);
   glEnable(GL_CULL_FACE); //le facce viste non vengono renderizzate
   glEnable(GL_TEXTURE_2D); //permette il texture mapping per i material
   FreeImage_Initialise();
   glm::vec4 gAmbient(0.1f, 0.1f, 0.1f, 0.1f);

   //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(gAmbient));//li setta alla scena l'illuminazione ambientale se non ci sono luci

   glShadeModel(GL_SMOOTH);

   // Done:
   std::cout << "[>] " << LIB_NAME << " initialized" << std::endl;
   reserved->initFlag = true;
   return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Free internal components.
 * @return TF
 */
bool ENG_API Eng::Base::free()
{
   // Not initialized?
   if (!reserved->initFlag)
   {
      std::cout << "ERROR: engine not initialized" << std::endl;
      return false;
   }

   // Here you can properly dispose of any allocated resource (including third-party dependencies)...
   m_render_list.clear();

   // Done:
   std::cout << "[<] " << LIB_NAME << " deinitialized" << std::endl;
   reserved->initFlag = false;

   FreeImage_DeInitialise();

   return true;
}

bool ENG_API Eng::Base::update()
{
   glutMainLoop();

   return true;
}



void ENG_API Eng::Base::renderScene()
{
   if (m_scene_graph != nullptr) {
      m_render_list.clear();
      m_render_list.pass(m_scene_graph, glm::mat4(1));
      m_render_list.render(currentCamera->get_inverse_camera());
   }

}

void ENG_API Eng::Base::loadScene(Node* root)
{
   m_render_list.clear();
   m_scene_graph = root;
}

void ENG_API Eng::Base::loadScene(const char* file_path, const char* texture_dir) {

   std::cout << "[ENGINE] Loading scene: " << file_path << std::endl;
   OvoReader ovoreader{};
   m_scene_graph = ovoreader.readFile(file_path, texture_dir);
}

void Eng::reshapeCallback(int width, int height) {
   glViewport(0, 0, width, height);  // Imposta il viewport
   auto& engine = Eng::Base::getInstance();

   engine.m_window_height = height;
   engine.m_window_width = width;

   glMatrixMode(GL_PROJECTION);
   if (engine.reshape_function != nullptr) {
      engine.reshape_function(width, height);  // Chiama la funzione passata dall'esterno
   }
   engine.currentCamera->update_window_size(width, height);
   glLoadMatrixf(glm::value_ptr(engine.currentCamera->get_properties()));

   glMatrixMode(GL_MODELVIEW);
}

void Eng::displayCallback() {
   glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   auto& engine = Eng::Base::getInstance();

   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(glm::value_ptr(engine.currentCamera->get_properties()));

   glMatrixMode(GL_MODELVIEW);
   if (engine.display_function != nullptr) {
      engine.display_function();  // Chiama la funzione passata dall'esterno
   }

   // SWITCH TO 2D RENDERING
   glDisable(GL_LIGHTING); 
   glDisable(GL_TEXTURE_2D); //si sposta in 2d per stampare le scritte

   // Set orthographic projection:
   glMatrixMode(GL_PROJECTION);
   glm::mat4 ortho;
   ortho = glm::ortho(0.0f, engine.m_window_width, 0.0f, engine.m_window_height, -1.0f, 1.0f);
   glLoadMatrixf(glm::value_ptr(ortho));
   glMatrixMode(GL_MODELVIEW);
   glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

   engine.calculateFPS();

   char buffer[32];



   float yPosition = engine.m_window_height - 12;
   float xPosition = engine.m_window_width - 220;
   snprintf(buffer, sizeof(buffer), ("=========== FPS ==========="));
   glRasterPos2f(xPosition, yPosition);
   glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);


   yPosition -= 11;
   snprintf(buffer, sizeof(buffer), "%.2f", engine.fps);
   glRasterPos2f(xPosition + 90, yPosition);
   glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

   yPosition -= 11;
   snprintf(buffer, sizeof(buffer), ("==========================="));
   glRasterPos2f(xPosition, yPosition);
   glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);
   
   yPosition = engine.m_window_height - 12;
   if (engine.textToRender.size() > 0)
   {
      for (auto& toPrint : engine.textToRender) {
         snprintf(buffer, sizeof(buffer), "%s", toPrint.c_str());
         glRasterPos2f(0.0f, yPosition);
         glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);
         yPosition -= 11;
      }
   }

   // END OF 2D RENDERING


   glEnable(GL_LIGHTING); 
   glEnable(GL_TEXTURE_2D); //ritorna in 3d


   glutSwapBuffers();
   glutPostWindowRedisplay(engine.windowId);//prende la nuova scena renderizzata e lo mette nella finestra corrispondente
}

void Eng::specialCallback(int key, int mouseX, int mouseY) {
   auto& engine = Eng::Base::getInstance();

   if (engine.special_function != nullptr) {
      engine.special_function(key, mouseX, mouseY);  // Chiama la funzione passata dall'esterno
   }

   glutPostWindowRedisplay(engine.windowId); //quando degli elementi si muovono tramite bottoni speciali dobbiamo renderizzare di nuovo la scena

}

void ENG_API Eng::Base::registerDisplayCallback(void (*function)(void)) {
   display_function = function;
   glutDisplayFunc(displayCallback);
}

void ENG_API Eng::Base::registerReshapeCallback(void (*function)(int, int)) {
   reshape_function = function;
   glutReshapeFunc(reshapeCallback);
}

void ENG_API Eng::Base::registerKeyboardCallback(
   void (*function)(unsigned char, int, int))
{
   keyboard_function = function;
   glutKeyboardFunc([](unsigned char key, int x, int y) {
      Eng::Base::getInstance().internalKeyboardCallback(key, x, y);
      });
}

void ENG_API Eng::Base::registerSpecialCallback(void (*function)(int, int, int)) {
   special_function = function;
   glutSpecialFunc(specialCallback);
}

void ENG_API Eng::Base::internalKeyboardCallback(unsigned char key, int x, int y)
{
   // 1 - Callback del client (se esiste)
   if (keyboard_function)
      keyboard_function(key, x, y);
   // Vieni sicuro di ridisegnare
   glutPostRedisplay();
}

void ENG_API Eng::Base::toggleLight(Node* light) {

   if (Light* l = dynamic_cast<Light*>(light))
   {
      std::cout << "vai" << std::endl;
      l->toggleLight();
      return;
   }

   std::cerr << "[Eng] toggleLight() called on a non-light node\n";

}

void ENG_API Eng::Base::setCamera(PerspectiveCamera* cam)
{
   currentCamera = cam;
}

void  ENG_API Eng::Base::addCamera(PerspectiveCamera* cam)
{
   cameras.push_back(cam);
   if (currentCamera == nullptr)
      currentCamera = cam;
}


void ENG_API Eng::Base::calculateFPS() {
   frameCount++;
   auto currentTime = std::chrono::steady_clock::now();
   std::chrono::duration<float> elapsedTime = currentTime - lastTime;

   if (elapsedTime.count() >= 1.0f) { // Aggiorna ogni secondo
      fps = frameCount / elapsedTime.count();
      frameCount = 0;
      lastTime = currentTime;
   }
}

Node ENG_API* Eng::Base::getRoot() {
   return m_scene_graph;
}

float ENG_API Eng::Base::getWindowHeight() {
   return m_window_height;
}

float ENG_API Eng::Base::getWindowWidth() {
   return m_window_width;
}

void ENG_API Eng::Base::printText(const std::string& text) {
   textToRender.push_back(text);
}

