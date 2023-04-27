#ifndef ARC_CAM_HPP
#define ARC_CAM_HPP
#include <glm/glm.hpp>
#include "cookbookogl.h"
#include <glm/gtc/matrix_transform.hpp>

class ArcCam{
private:
	   //view matrix, the inverse of the camera Matrix
        glm::mat4 _viewMatrix;
        //position of the camera
        glm::vec3 _position;
        //direction the camera is facing
        glm::vec3 _direction;
        //what the camera is looking at
        glm::vec3 _lookAtPoint;
		  //Wich way is up?
		  glm::vec3 _upVector;


        //Angles and such for arcball
        GLfloat _theta;
        GLfloat _phi;
public:
		 	ArcCam();
		  GLfloat _radius;
    void recomputeOrientation();
    void moveForward(GLfloat movementFactor);
    void moveBackward(GLfloat movementFactor);
	 void moveVert(GLfloat amount);
	 void moveHorz(GLfloat amount);
	 void rotate(GLfloat dTheta, GLfloat dPhi);
	   void setTheta( GLfloat t ) { _theta = t; }
		void setPhi( GLfloat p ) { _phi = p; }
	void setPosition( glm::vec3 pos ) { _position = pos; }
	void computeViewMatrix() { this->_viewMatrix = glm::lookAt( this->_position, this->_lookAtPoint, this->_upVector ); }
	 glm::mat4 getViewMatrix() { return this->_viewMatrix; }
	 glm::vec3 getPos(){return _position;};
	 void setLookAtPoint( glm::vec3 lookAt ) { _lookAtPoint = lookAt; recomputeOrientation();}
};

#endif // ARC_CAM_HPP
