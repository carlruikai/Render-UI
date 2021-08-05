#ifndef ROPE_H
#define ROPE_H
#include "mass.h"
#include "spring.h"
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <Eigen3/Eigen/Dense>
using namespace Eigen;
class Rope
{
public:
	Rope(glm::vec3 start, glm::vec3 end, glm::vec2 num_nodes, float node_mass, float k, std::vector<int> pinned_nodes, std::vector<int>& obj_indices);

	Rope(float node_mass, float k, std::vector<glm::vec3> obj_vertices, std::map<int, std::set<int>> connection, std::vector<int> pinned_nodes);

	Rope(float node_mass, float k, std::vector<glm::vec3> obj_vertices, std::map<int, std::set<int>> connection, std::vector<int> pinned_nodes, bool Newton);

	void implicitEuler(float delta_t, glm::vec3 gravity, 
		std::vector<glm::vec3>& mass_pos, std::vector<glm::vec3>& spring_pos, std::vector<glm::vec3>& normals, 
		std::vector<int> &clicked_mass_indices, std::vector <float>& clicked_mass_alpha, std::vector<glm::vec3> & clicked_mass_color, glm::vec2 clicked_pos, 
		Vector3d f_user, glm::mat4 view, glm::mat4 projection);

	void obj_animation(float delta_t, glm::vec3 gravity, std::vector<glm::vec3>& obj_spring_pos, std::vector<glm::vec3>& obj_vertices, std::vector<glm::vec3>& obj_normals, std::map<int, std::set<int>> connection);

	void obj_Newton_animation(float delta_t, glm::vec3 gravity, std::vector<glm::vec3>& obj_spring_pos, std::map<int, std::set<int>> connection);

	std::vector<Mass*> masses;
	std::vector<Spring*> springs;
	std::map <int, int> hash;
	std::map <int, std::set<int>> NZ_indices;
};
#endif // !ROPE_H
