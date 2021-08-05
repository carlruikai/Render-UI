#ifndef MASS_H
#define MASS_H
#include <glm/glm.hpp>
#include <iostream>
struct Mass
{
	Mass(glm::vec3 position, float mass, bool pinned, int ID)
		: position(position), last_one_position(position), initial_position(position),
		mass(mass), pinned(pinned), velocity(glm::vec3(0.0)), forces(glm::vec3(0.0)), id(ID){}

	float mass;
	bool pinned;
	float alpha;
	
	int id;
	
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 forces;

	glm::vec3 initial_position;
	glm::vec3 last_one_position;
};
#endif // !MASS_H
