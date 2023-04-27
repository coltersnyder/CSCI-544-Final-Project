#ifndef SCENEBASIC_H
#define SCENEBASIC_H

#include "scene.h"

#include "cookbookogl.h"
#include <string>
#include "ArcCam.hpp"

class SceneBasic : public Scene
{
private:
    GLuint vaoHandle, texHandle[3];
    GLuint programHandle;
	ArcCam* _arcCam;
    void linkMe(GLint vertShader, GLint tcsShader, GLint tesShader, GLint fragShader);
	void compileShaderProgram();
	GLuint setupTexture(const char *FILENAME);

	float angle;
	float tPrev, rotSpeed;

    unsigned int resolution;
    int width, height;
    std::string getShaderInfoLog(GLuint shader);
    std::string getProgramInfoLog(GLuint program);
	glm::vec2 _mousePosition;//where is the mouse?
	void setCallbacks();


		

public:
    SceneBasic();
	 ~SceneBasic();
    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
	void handleCursor(glm::vec2 pos);

};

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods);
    void cursor_callback(GLFWwindow* window, double x, double y);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // SCENEBASIC_H
