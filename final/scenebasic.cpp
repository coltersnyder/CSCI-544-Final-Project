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

SceneBasic::SceneBasic() { }

GLuint SceneBasic::setupTexture(){//this could just be the texture library in ingredients
	GLuint textureHandle = 0;
	GLint imageWidth, imageHeight, imageChannels;
	const char * FILENAME = "./image/white.png";

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
    float quadVertices[] = {
             -2.5f, -2.5f,  0.0f, 0.0f, 0.0f, // 0 - BL
              2.5f, -2.5f,  0.0f, 1.0f, 0.0f,// 1 - BR
             -2.5f,  2.5f,  0.0f, 0.0f, 1.0f,// 2 - TL
              2.5f,  2.5f,  0.0f, 1.0f, 1.0f// 3 - TR
    };
	 GLuint indexData[] = {0,1,2,3};
	 this->texHandle = setupTexture(); 

    // Create and populate the buffer objects
    GLuint index_buffer;
	 glGenBuffers(1, &index_buffer);

	 GLuint vbo;
    glGenBuffers(1, &vbo);
    GLuint positionBufferHandle = vbo;

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), quadVertices, GL_STATIC_DRAW);

	 //Index buffer
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	 glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*sizeof(GLuint),
						  (void *)&(indexData[0]),GL_STATIC_DRAW);

    // Create and set-up the vertex array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (GLubyte *)NULL );

    glEnableVertexAttribArray(1);  // Texture position
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) (sizeof(float)*3));

	 
	 glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, texHandle);

	 GLuint texmapPos = glGetUniformLocation(programHandle,"texMap");
		

    
	 glBindVertexArray(0);
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
        std::cerr << getShaderInfoLog(vertShader) << std::endl;
        exit(EXIT_FAILURE);
	}

	linkMe(vertShader, fragShader);
}

void SceneBasic::linkMe(GLint vertShader, GLint fragShader)
{
    // Create the program object
    programHandle = glCreateProgram();
    if(0 == programHandle) {
        std::cerr << "Error creating program object." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind index 0 to the shader input variable "VertexPosition"
    //glBindAttribLocation(programHandle, 0, "VertexPosition");
    // Bind index 1 to the shader input variable "VertexColor"
    //glBindAttribLocation(programHandle, 1, "VertexColor");

    // Attach the shaders to the program object
    glAttachShader( programHandle, vertShader );
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
	glDetachShader(programHandle, fragShader);
	glDeleteShader(vertShader);
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

void SceneBasic::update( float t )
{
		  float deltaT = t - tPrev;
		  if(tPrev == 0.0f) deltaT = 0.0f;
		  tPrev = t;
		  angle += rotSpeed * deltaT;
		  if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void SceneBasic::render()
{
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	vec3 cameraPos = vec3(10.0f * cos(angle), 5.0f, 10.0f * sin(angle));//TODO we can probally remove this part
	view = glm::lookAt(cameraPos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));

	//Setup the MVP matrix
	projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.3f, 100.0f);
	model = glm::rotate(mat4(1.0f), (GLfloat)(-PI/2.0), vec3(1.0f,0.0f,0.0f));
	mat4 mvp = projection * view * model;
	
	//Send the MVP matrix
	GLint mvp_location = glGetUniformLocation(programHandle,"MVP");
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &(mvp)[0][0]);

    glClear(GL_COLOR_BUFFER_BIT);
	 glBindTexture(GL_TEXTURE_2D,texHandle);
    glBindVertexArray(vaoHandle);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT,0);
	 glBindVertexArray(0);
}

void SceneBasic::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
}
