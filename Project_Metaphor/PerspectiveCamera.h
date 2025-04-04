#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera {
private:
	glm::mat4 projection;
	glm::vec3 Offset;
public:
	PerspectiveCamera(glm::vec3 pos,float height, float width);
	void updateCameraPosition(const glm::vec3& carPosition, bool stateCam);
	glm::mat4 getProjection();
};