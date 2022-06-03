#include "particle.h"
#include <random>

static float RandomFloat() 
{
	float r = (float)rand() / (double)RAND_MAX;
	return r;
}

particle::particle(glm::vec3 origin, camera* _camera, std::string texFileName)
{
	nParticles = 4000;

	for (int i = 0; i < nParticles; i++) 
	{
		vPosition.push_back(glm::vec3(0.0)); //initialize position vector
		particle p = particle(
			origin, // pos
			glm::vec3(0.25 * cos(i * .0167) + 0.25f * RandomFloat() - 0.125f, // vel
				2.0f + 0.25f * RandomFloat() - 0.125f,
				0.25 * sin(i * .0167) + 0.25f * RandomFloat() - 0.125f),
			RandomFloat() + 0.125 // elapsed time
		);
		particles.push_back(p); // add 

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vPosition.size(), &vPosition[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

}

particle::~particle()
{
}

void particle::Render(float dt)
{
	for (int i = 0; i < nParticles; i++) 
	{
		particles[i].update(.0167);
		vPosition[i] = particles[i].getPosition();
	}
}




ResetToInitialValues() 
{
	this->position = this->origin;
	this->velocity = glm::vec3(	0.25 * cos(this->id * .0167) + 0.25f * RandomFloat() - 0.125f, 1.5f + 0.25f * RandomFloat() - 0.125f, 0.25 * sin(this->id * .0167) + 0.25f * RandomFloat() - 0.125f);
	this->elapsedTime = RandomFloat() + 0.125;
}

void Update(float dt) 
{
	this->velocity.y += -0.2 * .0167f;
	this->position += velocity;
	this->elapsedTime -= .000167;

	if (this->elapsedTime <= 0.0f) 
	{
		ResetToInitialValues();
	}
}

