#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <string>
#include <iostream>
#include <vector>

#include "Shader.h"

class Player {
private:
	//Transformation Variables
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	float rotX;
	float rotY;
	float rotZ;

	glm::mat4 identity_matrix4;

	//Camera Variables
	glm::mat4 projection;
	glm::vec3 cameraPos;
	glm::vec3 WorldUp;
	glm::vec3 Front;
	glm::mat4 viewMatrix;

public:
	Player(glm::vec3 position);

	void draw(GLuint* shaderProg, GLuint* VAO, std::vector<GLfloat>* fullVertexData);
	void mainDraw(Shader* shaderProg, GLuint* VAO, std::vector<GLfloat>* fullVertexData);
	void rotate(char axis, int direction);
	glm::mat4 mainTrans();
	void translate(glm::vec3 newPos);

	void setCamera(glm::mat4 projection, glm::vec3 cameraPos, glm::vec3 Front, glm::mat4 viewMatrix);
	void setTexture(Shader* shaderProg, GLuint* texture, const std::string& name);
	void setPosition(glm::vec3 newPos);
	void setScale(glm::vec3 newScale);
	void setRotation(float rotX, float rotY, float rotZ);

	glm::vec3 getPosition();
	glm::vec3 getPosition(bool fromMatrix);
};