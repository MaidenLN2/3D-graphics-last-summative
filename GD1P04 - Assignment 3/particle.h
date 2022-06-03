#pragma once
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include "camera.h"
class particle
{
public:
	particle(glm::vec3 origin, camera* _camera, std::string texFileName);
	~particle();
	void Render(float dt);
	std::vector<particle> particles;
	std::vector<glm::vec3> vPosition;
private:
	camera* camera;
	GLuint VAO, VBO, texture, program;
	float nParticles;

};

