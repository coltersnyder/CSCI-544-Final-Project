#ifndef SCENEBASIC_H
#define SCENEBASIC_H

#include "scene.h"

#include "cookbookogl.h"
#include <string>

class SceneBasic : public Scene
{
private:
    GLuint vaoHandle, texHandle;
    GLuint programHandle;

   void linkMe(GLint vertShader, GLint fragShader);
	void compileShaderProgram();
	GLuint setupTexture();

	float angle;
	float tPrev, rotSpeed;
   std::string getShaderInfoLog(GLuint shader);
   std::string getProgramInfoLog(GLuint program);
	

public:
    SceneBasic();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);

};

#endif // SCENEBASIC_H
