#include "ArcCam.hpp"

#include <cmath>

ArcCam::ArcCam(){
	 _viewMatrix = glm::mat4(1.0f);
    _position = glm::vec3(0.0f, 0.0f, 0.0f );
    _direction = glm::vec3( 0.0f, 0.0f, -1.0f );
    _lookAtPoint = glm::vec3( 0.0f, 0.0f, -1.0f );
    _upVector = glm::vec3(0,1,0);
	 _theta = 0.0f;
    _phi = 3.14f / 2.0f;
    _radius = 10.0f;
}

void ArcCam::recomputeOrientation() {
	double tmp = sin(_phi)*_radius;
	double x = -tmp*sin(_theta);
	double y = _radius*cos(_phi);
	double z = tmp*cos(_theta);

	_direction = glm::vec3(x, y, z);
	_position = _direction + _lookAtPoint;
	computeViewMatrix();
}

void ArcCam::moveForward(GLfloat movementFactor) {
	if (_radius > 0.5){//do not go past the look at point
		_radius -= movementFactor;
		recomputeOrientation();
	}
}

void ArcCam::moveBackward(GLfloat movementFactor) {
	if (_radius < 100.0){//do not go too far out, then we can't see
		_radius += movementFactor;
		recomputeOrientation();
	}
}

void ArcCam::rotate(GLfloat dTheta, GLfloat dPhi) {
    _theta += dTheta;          // update theta
    _phi += dPhi;             // update phi
    if(_phi <= 0)       _phi = 0+0.001;
    if(_phi >= 3.1415)    _phi = 3.1415-0.001;
    recomputeOrientation(); 
}

