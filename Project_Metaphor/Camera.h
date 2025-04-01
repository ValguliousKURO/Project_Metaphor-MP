#pragma once 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
protected:
	
	//glm::mat4 projection;
	glm::vec3 cameraPos;
	glm::vec3 WorldUp;
	glm::vec3 Front;
	glm::mat4 viewMatrix;

public:
	Camera(glm::vec3 pos);
public:

	void setCameraPos(glm::vec3 camPos);
	void setFront(glm::vec3 Front);
	glm::vec3 getFront();
	glm::vec3 getWorldUp();
	glm::mat4 getViewMat();
	glm::vec3 getCameraPos();

};