#include "scenebasic.h"

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using std::string;
#include <vector>
#include <iterator>

#include "glutils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "stb/stb_image.h" //for loading images
using glm::vec3;
using glm::mat4;


#ifndef PI
#define PI 3.14159265
#endif

//Camera Variables
glm::vec2 _newMousePos = glm::vec2(-1,-1);
GLuint _leftMouseButtonState = 0;
int zoom = 0;
int vert = 0;
int horz = 0;

//what texture we should be on
int texID = 0;
SceneBasic::SceneBasic() {
	 _arcCam = new ArcCam();//make a new arcCamera
    _arcCam->setPosition(vec3(10.0f * cos(angle), 1.5f, 10.0f * sin(angle)));
    _arcCam->setTheta( -M_PI / 3.0f );
    _arcCam->setPhi( M_PI / 3.0f );
    _arcCam->setLookAtPoint(glm::vec3(0,0,0));
    _arcCam->recomputeOrientation();
}
SceneBasic::~SceneBasic(){
	delete _arcCam;
}

GLuint SceneBasic::setupTexture(const char *FILENAME){ //this could just be the texture library in ingredients
	GLuint textureHandle = 0;
	GLint imageWidth, imageHeight, imageChannels;

   GLubyte* data = stbi_load( FILENAME, &imageWidth, &imageHeight, &imageChannels, 0);
	
	
	if( data ) {
		glGenTextures(1, &textureHandle);
   	glBindTexture(GL_TEXTURE_2D,textureHandle);
      const GLint STORAGE_TYPE = (imageChannels == 4 ? GL_RGBA : GL_RGB);
   	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		
   	glTexImage2D(GL_TEXTURE_2D,0,STORAGE_TYPE,
         imageWidth,imageHeight,0,
         STORAGE_TYPE,GL_UNSIGNED_BYTE,data);
		fprintf( stdout, "%s texture map read in with handle %d\n", FILENAME, textureHandle);
		stbi_image_free(data);//clean up
	}
	else{
		fprintf( stderr, "ERROR: Could not load texture map \"%s\"\n", FILENAME );
	}
	
	return textureHandle;
}

void SceneBasic::setCallbacks(){
	glfwSetMouseButtonCallback(this->window, mouse_button_callback);
   glfwSetCursorPosCallback(this->window, cursor_callback);
   glfwSetKeyCallback(this->window, key_callback);
}
void SceneBasic::initScene(){
    // **************************************************************************************
    // Choose one of the following options for the shader program.
    //  1)  Compile the shader program normally
    //  2)  Load a binary (pre-compiled) shader program.  (file: "shader/program.bin")
    //  3)  Load a SPIR-V shader program. (files: "shader/vert.spv" and "shader/frag.spv")
    //
    // Optionally, you may attempt to write out the shader program binary using the function
    // writeShaderBinary().
    // **************************************************************************************
	glEnable(GL_DEPTH_TEST);
	projection = mat4(1.0f);
	angle = glm::radians(140.0f);
	tPrev = 0;
	rotSpeed = PI/16.0f;

    // (1) Use this to load and compile the shader program.
    compileShaderProgram();

    /////////////////// Create the VBO ////////////////////
    // float quadVertices[] = {
    //          -2.5f, -2.5f,  0.0f, 0.0f, 0.0f, // 0 - BL
    //           2.5f, -2.5f,  0.0f, 1.0f, 0.0f,// 1 - BR
    //          -2.5f,  2.5f,  0.0f, 0.0f, 1.0f,// 2 - TL
    //           2.5f,  2.5f,  0.0f, 1.0f, 1.0f// 3 - TR
    // };
	//  GLuint indexData[] = {0,1,2,3};

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	std::vector<float> quadVertices;
	resolution = 20;

	// TODO: Load Height Map isntead of temp width, height
	width = 1028;
	height = 1028;
	// END TODO

	for(int i = 0; i < resolution; i++){
		for(int j = 0; j < resolution; j++){
			quadVertices.push_back(-width/2.0f + width*i/(float)resolution); // v.x
			quadVertices.push_back(0.0f); // v.y
			quadVertices.push_back(-height/2.0f + height*j/(float)resolution); // v.z
			quadVertices.push_back(i / (float)resolution); // u
			quadVertices.push_back(j / (float)resolution); // v

			quadVertices.push_back(-width/2.0f + width*(i+1)/(float)resolution); // v.x
			quadVertices.push_back(0.0f); // v.y
			quadVertices.push_back(-height/2.0f + height*j/(float)resolution); // v.z
			quadVertices.push_back((i+1) / (float)resolution); // u
			quadVertices.push_back(j / (float)resolution); // v

			quadVertices.push_back(-width/2.0f + width*i/(float)resolution); // v.x
			quadVertices.push_back(0.0f); // v.y
			quadVertices.push_back(-height/2.0f + height*(j+1)/(float)resolution); // v.z
			quadVertices.push_back(i / (float)resolution); // u
			quadVertices.push_back((j+1) / (float)resolution); // v

			quadVertices.push_back(-width/2.0f + width*(i+1)/(float)resolution); // v.x
			quadVertices.push_back(0.0f); // v.y
			quadVertices.push_back(-height/2.0f + height*(j+1)/(float)resolution); // v.z
			quadVertices.push_back((i+1) / (float)resolution); // u
			quadVertices.push_back((j+1) / (float)resolution); // v
		}
	}

	this->texHandle[0] = setupTexture("./image/happy.png"); 
	this->texHandle[1] = setupTexture("./image/mad.png"); 
	this->texHandle[2] = setupTexture("./image/map.png"); 

    // Create and populate the buffer objects
    // GLuint index_buffer;
	// glGenBuffers(1, &index_buffer);

	GLuint vbo;
    glGenBuffers(1, &vbo);
    GLuint positionBufferHandle = vbo;

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), &quadVertices[0], GL_STATIC_DRAW);

	 // Index buffer
	 // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	 // glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*sizeof(GLuint),
	 //					  (void *)&(indexData[0]),GL_STATIC_DRAW);

    // Create and set-up the vertex array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (GLubyte *)NULL );

    glEnableVertexAttribArray(1);  // Texture position
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) (sizeof(float)*3));

	 
	 glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, texHandle[texID]);

	 GLuint texmapPos = glGetUniformLocation(programHandle,"texMap");
		

    
	 glBindVertexArray(0);
	 setCallbacks();
}


void SceneBasic::compileShaderProgram() {
    std::cout << "Compiling shader program" << std::endl;

	//////////////////////////////////////////////////////
	/////////// Vertex shader //////////////////////////
	//////////////////////////////////////////////////////

	// Load contents of file
	std::ifstream inFile("shader/basic.vert.glsl");
	if (!inFile) {
		fprintf(stderr, "Error opening file: shader/basic.vert.glsl\n");
		exit(1);
	}

	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();
	string codeStr(code.str());

	// Create the shader object
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	if (0 == vertShader) {
		std::cerr << "Error creating vertex shader." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load the source code into the shader object
	const GLchar* codeArray[] = { codeStr.c_str() };
	glShaderSource(vertShader, 1, codeArray, NULL);

	// Compile the shader
	glCompileShader(vertShader);

	// Check compilation status
	GLint result;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		std::cerr << "Vertex shader compilation failed!" << std::endl;
		std::cerr << getShaderInfoLog(vertShader) << std::endl;
        exit(EXIT_FAILURE);
	}

	//////////////////////////////////////////////////////
	/////////// Tesselation Control shader ///////////////
	//////////////////////////////////////////////////////

	// Load contents of file
	std::ifstream tcsFile("shader/basic.tcs.glsl");
	if (!tcsFile) {
		fprintf(stderr, "Error opening file: shader/basic.tcs.glsl\n");
		exit(1);
	}

	std::stringstream tcsCode;
	tcsCode << tcsFile.rdbuf();
	tcsFile.close();
	string tcsCodeStr(tcsCode.str());

	// Create the shader object
	GLuint tcsShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	if (0 == tcsShader) {
		std::cerr << "Error creating tesselation control shader." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load the source code into the shader object
	const GLchar* tcsCodeArray[] = { tcsCodeStr.c_str() };
	glShaderSource(tcsShader, 1, tcsCodeArray, NULL);

	// Compile the shader
	glCompileShader(tcsShader);

	// Check compilation status
	glGetShaderiv(tcsShader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		std::cerr << "Tesselation control shader compilation failed!" << std::endl;
		std::cerr << getShaderInfoLog(tcsShader) << std::endl;
        exit(EXIT_FAILURE);
	}

	//////////////////////////////////////////////////////
	/////////// Tesselation Evaluation shader ////////////
	//////////////////////////////////////////////////////

	// Load contents of file
	std::ifstream tesFile("shader/basic.tes.glsl");
	if (!tcsFile) {
		fprintf(stderr, "Error opening file: shader/basic.tes.glsl\n");
		exit(1);
	}

	std::stringstream tesCode;
	tesCode << tesFile.rdbuf();
	tesFile.close();
	string tesCodeStr(tesCode.str());

	// Create the shader object
	GLuint tesShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	if (0 == tesShader) {
		std::cerr << "Error creating tesselation evaluation shader." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load the source code into the shader object
	const GLchar* tesCodeArray[] = { tesCodeStr.c_str() };
	glShaderSource(tesShader, 1, tesCodeArray, NULL);

	// Compile the shader
	glCompileShader(tesShader);

	// Check compilation status
	glGetShaderiv(tesShader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		std::cerr << "Tesselation evaluation shader compilation failed!" << std::endl;
		std::cerr << getShaderInfoLog(tesShader) << std::endl;
        exit(EXIT_FAILURE);
	}

	//////////////////////////////////////////////////////
	/////////// Fragment shader //////////////////////////
	//////////////////////////////////////////////////////

	// Load contents of file into shaderCode here
	std::ifstream fragFile("shader/basic.frag.glsl");
	if (!fragFile) {
		fprintf(stderr, "Error opening file: shader/basic.frag.glsl\n");
		exit(1);
	}

	std::stringstream fragCode;
	fragCode << fragFile.rdbuf();
	fragFile.close();
	codeStr = fragCode.str();

	// Create the shader object
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (0 == fragShader) {
		fprintf(stderr, "Error creating fragment shader.\n");
		exit(1);
	}

	// Load the source code into the shader object
	codeArray[0] = codeStr.c_str();
	glShaderSource(fragShader, 1, codeArray, NULL);

	// Compile the shader
	glCompileShader(fragShader);

	// Check compilation status
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
        std::cerr << "Fragment shader compilation failed!" << std::endl;
        std::cerr << getShaderInfoLog(fragShader) << std::endl;
        exit(EXIT_FAILURE);
	}

	linkMe(vertShader, tcsShader, tesShader, fragShader);
}

void SceneBasic::linkMe(GLint vertShader, GLint tcsShader, GLint tesShader, GLint fragShader)
{
    // Create the program object
    programHandle = glCreateProgram();
    if(0 == programHandle) {
        std::cerr << "Error creating program object." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind index 0 to the shader input variable "VertexPosition"
    glBindAttribLocation(programHandle, 0, "VertexPosition");
    // Bind index 1 to the shader input variable "VertexColor"
    glBindAttribLocation(programHandle, 1, "vTexCoord");

    // Attach the shaders to the program object
    glAttachShader( programHandle, vertShader );
	 glAttachShader( programHandle, tcsShader  );
	 glAttachShader( programHandle, tesShader  );
    glAttachShader( programHandle, fragShader );

    // Link the program
    glLinkProgram( programHandle );

    // Check for successful linking
    GLint status;
    glGetProgramiv( programHandle, GL_LINK_STATUS, &status );
    if (GL_FALSE == status) {
        std::cerr << "Failed to link shader program!" << std::endl;
        std::cerr << getProgramInfoLog(programHandle) << std::endl;
        exit(EXIT_FAILURE);
    }

	// Clean up shader objects
	glDetachShader(programHandle, vertShader);
	glDetachShader(programHandle, tcsShader );
	glDetachShader(programHandle, tesShader );
	glDetachShader(programHandle, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(tcsShader );
	glDeleteShader(tesShader );
	glDeleteShader(fragShader);

    glUseProgram( programHandle );
}


std::string SceneBasic::getShaderInfoLog(GLuint shader) {
    GLint logLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

    std::string log;
    if (logLen > 0) {
        log.resize(logLen, ' ');
        GLsizei written;
        glGetShaderInfoLog(shader, logLen, &written, &log[0]);
    }

    return log;
}

std::string SceneBasic::getProgramInfoLog(GLuint program) {
    GLint logLen;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLen );

    std::string log;
    if (logLen > 0) {
        log.resize(logLen, ' ');
        GLsizei written;
        glGetProgramInfoLog(program, logLen, &written, &log[0]);
    }
    return log;
}
void SceneBasic::handleCursor(glm::vec2 pos) {
   if(_mousePosition.x == -1) {//initilize the mouse so the camera dosn't flip
        _mousePosition = pos;
    }
   if (_leftMouseButtonState == GLFW_PRESS){
      _arcCam->rotate((pos.x - _mousePosition.x) * 0.005f,
                         (_mousePosition.y - pos.y) * 0.005f );
   
   }
   _mousePosition = pos;
	if(zoom == 1){
			  _arcCam->moveForward(0.5);
			  zoom = 0;
	}
	else if(zoom == -1){
			  _arcCam->moveBackward(0.5);
			  zoom = 0;
	}
	if(vert == 1){
			  _arcCam->moveVert(1);
			  vert = 0;
	}
	else if(vert == -1){
			  _arcCam->moveVert(-1);
			  vert = 0;
	}
	if(horz == 1){
			  _arcCam->moveHorz(1);
			  horz = 0;
	}
	else if(horz == -1){
			  _arcCam->moveHorz(-1);
			  horz = 0;
	}
}  

void SceneBasic::update( float t )
{
		  handleCursor(_newMousePos);
}

void SceneBasic::render()
{
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	view = _arcCam->getViewMatrix();


	//Setup the MVP matrix
	projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.3f, 3000.0f);
	model = glm::mat4(1.0f);//glm::rotate(mat4(1.0f), (GLfloat)(-PI/2.0), vec3(0.0f,0.0f,0.0f));
	mat4 mvp = projection * view * model;
	
	//Send the MVP matrix
	GLint mvp_location = glGetUniformLocation(programHandle,"MVP");
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &(mvp)[0][0]);

    glClear(GL_COLOR_BUFFER_BIT);
	 glBindTexture(GL_TEXTURE_2D,texHandle[texID]);
    glBindVertexArray(vaoHandle);
    glDrawArrays(GL_PATCHES, 0, 4*resolution*resolution);
	 glBindVertexArray(0);
}

void SceneBasic::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
}
//--------------------Call Backs-------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
   if (action == GLFW_PRESS || action == GLFW_REPEAT){
      switch (key){
         case GLFW_KEY_1:
				texID = 0;
            break;
         case GLFW_KEY_2:
				texID = 1;
            break;
         case GLFW_KEY_3:
				texID = 2;
            break;
			case GLFW_KEY_W:
				zoom = 1;
				break;
			case GLFW_KEY_S:
				zoom = -1;
				break;
			case GLFW_KEY_UP:
				vert = 1;
				break;
			case GLFW_KEY_DOWN:
				vert = -1;
				break;
			case GLFW_KEY_LEFT:
				horz = 1;
				break;
			case GLFW_KEY_RIGHT:
				horz = -1;
				break;
				
      }
   }

}
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}
void cursor_callback(GLFWwindow *window, double x, double y ) {
    _newMousePos = glm::vec2(x, y);
}
