#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include "glm/glm.hpp"
#include "mass.h"
#include "rope.h"
#include "spring.h"
#include <Eigen3/Eigen/Dense>
#include <Eigen3/Eigen/Sparse>
#include <chrono>
#include <string>
#include <omp.h>

using namespace std;
using namespace Eigen;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
VectorXi reserved_sizes;
std::unordered_map<int, std::unordered_map<int, double*>> nnz_pointers;
SparseMatrix<double> Jx;
vector<Triplet<double>> Jx_triplets;
SparseMatrix<double> Newton_A;
SimplicialLLT<SparseMatrix<double>> Newton_solver;

//SparseMatrix<double> M;
SparseMatrix<double> M(1200, 1200);
vector<Eigen::Triplet<double>> M_triplets;
SparseMatrix<double> L(1200, 1200);
vector<Eigen::Triplet<double>> L_triplets;
SparseMatrix<double> J;
vector<Eigen::Triplet<double>> J_triplets;
int pinned_nodes_size = 0;
bool init = true;
Vector3d sphere_center(0.75, 0.0, 0.0);
glm::mat4 mass_model(1.0);
int row_num = 0, column_num = 0;

//Bunny parameters
SparseMatrix<double> obj_M;
vector<Eigen::Triplet<double>> obj_M_triplets;
SparseMatrix<double> obj_L;
vector<Eigen::Triplet<double>> obj_L_triplets;
SparseMatrix<double> obj_J;
vector<Eigen::Triplet<double>> obj_J_triplets;
int obj_mass_size;
int obj_spring_size;
int fixed_mass_size;
int flexible_mass_size;
Rope::Rope(glm::vec3 start, glm::vec3 end, glm::vec2 num_nodes, float node_mass, float k, std::vector<int> pinned_nodes, std::vector<int>& obj_indices)
{
	row_num = num_nodes.x;
	column_num = num_nodes.y;
	if (glm::length(num_nodes) == 0 || glm::length(num_nodes) == 1)
		return;
	int ID = 0;
	glm::vec3 CurrentPosition = start;
	Mass* p = nullptr;
	Mass* l = nullptr;
	Spring* presentSpring = nullptr;
	float row_interval = (end.x - start.x) / (num_nodes.x - 1);
	float column_interval = (end.z - start.z) / (num_nodes.y - 1);
	for (int i = 0; i <= num_nodes.x - 1; i++)
	{
		for (int j = 0; j <= num_nodes.y - 1; j++)
		{
			if (i == (num_nodes.x - 1) && j == (num_nodes.y - 1))
			{
				CurrentPosition = end;
			}
			else
			{
				CurrentPosition = start + glm::vec3(i * row_interval, 0, j * column_interval); //先列后行
			}
			l = new Mass(CurrentPosition, node_mass, false, ID);
			ID++;
			masses.push_back(l);
		}
	}

	//先竖直方向后水平方向
	for (int i = 0; i < num_nodes.x; i++)
	{
		p = masses[i * num_nodes.y];
		for (int j = 1; j < num_nodes.y; j++)
		{
			l = p;
			p = masses[i * num_nodes.y + j];
			presentSpring = new Spring(p, l, k);
			springs.push_back(presentSpring); //每列的弹簧
		}
	}

	for (int i = 0; i < num_nodes.x - 1; i++)
	{
		for (int j = 0; j < num_nodes.y; j++)
		{
			l = masses[i * num_nodes.y + j];
			p = masses[(i + 1) * num_nodes.y + j];
			presentSpring = new Spring(l, p, k); //每行的弹簧
			springs.push_back(presentSpring);
		}
	}

	//render indices
	for (int i = 0; i < num_nodes.x - 1; i++)
	{
		for (int j = 0; j < num_nodes.y - 1; j++)
		{
			obj_indices.push_back(i * num_nodes.x + j);
			obj_indices.push_back(i * num_nodes.x + j + 1);
			obj_indices.push_back((i + 1) * num_nodes.x + j);

			obj_indices.push_back(i * num_nodes.x + j + 1);
			obj_indices.push_back((i + 1) * num_nodes.x + j);
			obj_indices.push_back((i + 1) * num_nodes.x + j + 1);
		}
	}

	
	for (auto& i : pinned_nodes) {
		masses[i]->pinned = true;
	}

	////存储每个mass在矩阵中block的位置
	//int index = 0;
	//for (auto& m : masses)
	//{
	//	if (!m->pinned)
	//	{
	//		hash.insert(pair<int, int>(m->id, m->id - index));
	//	}
	//	else
	//	{
	//		hash.insert(pair<int, int>(m->id, -1));
	//		index++;
	//	}
	//}

	////记录Jacobian矩阵中非零元素的数量
	//for (auto& s : springs)
	//{
	//	if (!s->m1->pinned && !s->m2->pinned)
	//	{
	//		for (int i = 0; i < 3; i++)
	//		{
	//			for (int j = 0; j < 3; j++)
	//			{
	//				NZ_indices[3 * hash[s->m1->id] + j].insert(3 * hash[s->m1->id] + i);
	//				NZ_indices[3 * hash[s->m2->id] + j].insert(3 * hash[s->m1->id] + i);
	//				NZ_indices[3 * hash[s->m1->id] + j].insert(3 * hash[s->m2->id] + i);
	//				NZ_indices[3 * hash[s->m2->id] + j].insert(3 * hash[s->m2->id] + i);
	//			}
	//		}
	//	}

	//	else if (s->m1->pinned && !s->m2->pinned)
	//	{
	//		for (int i = 0; i < 3; i++)
	//		{
	//			for (int j = 0; j < 3; j++)
	//			{
	//				NZ_indices[3 * hash[s->m2->id] + j].insert(3 * hash[s->m2->id] + i);
	//			}
	//		}
	//	}

	//	else if (!s->m1->pinned && s->m2->pinned)
	//	{
	//		for (int i = 0; i < 3; i++)
	//		{
	//			for (int j = 0; j < 3; j++)
	//			{
	//				NZ_indices[3 * hash[s->m1->id] + j].insert(3 * hash[s->m1->id] + i);
	//			}
	//		}
	//	}
	//}
	//for (int i = 0; i < 21; i++)
	//{
	//	sizes[i] = NZ_indices[i].size();
	//}

	//set up mass matrix
	for (auto& m : masses)
	{
		M_triplets.push_back(Triplet<double>(3 * m->id, 3 * m->id, m->mass));
		M_triplets.push_back(Triplet<double>(3 * m->id + 1, 3 * m->id + 1, m->mass));
		M_triplets.push_back(Triplet<double>(3 * m->id + 2, 3 * m->id + 2, m->mass));
	}
	M.setFromTriplets(M_triplets.begin(), M_triplets.end());

	//set up L matrix
	SparseMatrix<double> kAA(400, 400);
	for (auto& s : springs)
	{
		SparseVector<double> A(400);
		A.reserve(2);
		A.insert(s->m1->id) = 1;
		A.insert(s->m2->id) = -1;
		kAA += s->k * A * A.transpose();
	}
	for (int k = 0; k < kAA.outerSize(); ++k)
		for (SparseMatrix<double>::InnerIterator it(kAA, k); it; ++it)
		{
			L_triplets.push_back(Triplet<double>(3 * it.row(), 3 * it.col(), it.value()));
			L_triplets.push_back(Triplet<double>(3 * it.row() + 1, 3 * it.col() + 1, it.value()));
			L_triplets.push_back(Triplet<double>(3 * it.row() + 2, 3 * it.col() + 2, it.value()));
		}
	L.setFromTriplets(L_triplets.begin(), L_triplets.end());

	//set up J matrix
	int spring_size = springs.size();
	J = SparseMatrix<double>(1200, 3 * spring_size);
	SparseMatrix<double> kAS(400, spring_size);
	int spring_index = 0;
	for (auto& s : springs)
	{
		SparseVector<double> A(400);
		A.reserve(2);
		A.insert(s->m1->id) = 1;
		A.insert(s->m2->id) = -1;
		SparseVector<double> S(spring_size);
		S.reserve(1);
		S.insert(spring_index) = 1;
		kAS += s->k * A * S.transpose();
		spring_index++;
	}
	for (int k = 0; k < kAS.outerSize(); ++k)
		for (SparseMatrix<double>::InnerIterator it(kAS, k); it; ++it)
		{
			J_triplets.push_back(Triplet<double>(3 * it.row(), 3 * it.col(), it.value()));
			J_triplets.push_back(Triplet<double>(3 * it.row() + 1, 3 * it.col() + 1, it.value()));
			J_triplets.push_back(Triplet<double>(3 * it.row() + 2, 3 * it.col() + 2, it.value()));
		}
	J.setFromTriplets(J_triplets.begin(), J_triplets.end());
}

Rope::Rope(float node_mass, float k, std::vector<glm::vec3> obj_vertices, std::map<int, std::set<int>> connection, std::vector<int> pinned_nodes)
{
	glm::vec3 CurrentPosition;
	Mass* p = nullptr;
	Mass* l = nullptr;
	Spring* presentSpring = nullptr;
	obj_mass_size = obj_vertices.size();
	//load masses
	for (int i = 0; i < obj_mass_size; i++)
	{
		CurrentPosition = obj_vertices[i];
		l = new Mass(CurrentPosition, node_mass, false, i);
		masses.push_back(l);
	}

	//load springs
	for (auto outer_it = connection.begin(); outer_it != connection.end(); ++outer_it)
	{
		l = masses[outer_it->first];
		for (auto inner_it = outer_it->second.begin(); inner_it != outer_it->second.end(); ++inner_it)
		{
			p = masses[*inner_it];
			presentSpring = new Spring(l, p, k);
			springs.push_back(presentSpring);
		}
	}


	for (auto& i : pinned_nodes) {
		masses[i]->pinned = true;
	}

	//set up mass matrix
	obj_M = SparseMatrix<double>(3 * obj_mass_size, 3 * obj_mass_size);
	for (auto& m : masses)
	{
		obj_M_triplets.push_back(Triplet<double>(3 * m->id, 3 * m->id, m->mass));
		obj_M_triplets.push_back(Triplet<double>(3 * m->id + 1, 3 * m->id + 1, m->mass));
		obj_M_triplets.push_back(Triplet<double>(3 * m->id + 2, 3 * m->id + 2, m->mass));
	}
	obj_M.setFromTriplets(obj_M_triplets.begin(), obj_M_triplets.end());

	//set up L matrix
	SparseMatrix<double> kAA(obj_mass_size, obj_mass_size);
	obj_L = SparseMatrix<double>(3 * obj_mass_size, 3 * obj_mass_size);
	vector<Triplet<double>> kAA_triplets;
	int index = 0;

	for (auto& s : springs)
	{
		obj_L_triplets.push_back(Triplet<double>(3 * s->m1->id, 3 * s->m1->id, k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m1->id + 1, 3 * s->m1->id + 1, k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m1->id + 2, 3 * s->m1->id + 2, k));

		obj_L_triplets.push_back(Triplet<double>(3 * s->m1->id, 3 * s->m2->id, -k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m1->id + 1, 3 * s->m2->id + 1, -k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m1->id + 2, 3 * s->m2->id + 2, -k));

		obj_L_triplets.push_back(Triplet<double>(3 * s->m2->id, 3 * s->m1->id, -k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m2->id + 1, 3 * s->m1->id + 1, -k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m2->id + 2, 3 * s->m1->id + 2, -k));

		obj_L_triplets.push_back(Triplet<double>(3 * s->m2->id, 3 * s->m2->id, k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m2->id + 1, 3 * s->m2->id + 1, k));
		obj_L_triplets.push_back(Triplet<double>(3 * s->m2->id + 2, 3 * s->m2->id + 2, k));
	}
	obj_L.setFromTriplets(obj_L_triplets.begin(), obj_L_triplets.end());

	//set up J matrix
	obj_spring_size = springs.size();
	obj_J = SparseMatrix<double>(3 * obj_mass_size, 3 * obj_spring_size);
	SparseMatrix<double> kAS(obj_mass_size, obj_spring_size);
	vector<Triplet<double>> kAS_triplets;
	int spring_index = 0;

	for (auto& s : springs)
	{
		obj_J_triplets.push_back(Triplet<double>(3 * s->m1->id, 3 * spring_index, k));
		obj_J_triplets.push_back(Triplet<double>(3 * s->m1->id + 1, 3 * spring_index + 1, k));
		obj_J_triplets.push_back(Triplet<double>(3 * s->m1->id + 2, 3 * spring_index + 2, k));

		obj_J_triplets.push_back(Triplet<double>(3 * s->m2->id, 3 * spring_index, -k));
		obj_J_triplets.push_back(Triplet<double>(3 * s->m2->id + 1, 3 * spring_index + 1, -k));
		obj_J_triplets.push_back(Triplet<double>(3 * s->m2->id + 2, 3 * spring_index + 2, -k));

		spring_index++;
	}
	obj_J.setFromTriplets(obj_J_triplets.begin(), obj_J_triplets.end());

}

Rope::Rope(float node_mass, float k, std::vector<glm::vec3> obj_vertices, std::map<int, std::set<int>> connection, std::vector<int> pinned_nodes, bool Newton)
{
	obj_M_triplets.clear();
	obj_mass_size = obj_vertices.size();
	fixed_mass_size = pinned_nodes.size();
	flexible_mass_size = obj_mass_size - fixed_mass_size;
	obj_M = SparseMatrix<double>(3 * flexible_mass_size, 3 * flexible_mass_size);
	reserved_sizes = VectorXi(3 * flexible_mass_size);

	glm::vec3 CurrentPosition;
	Mass* p = nullptr;
	Mass* l = nullptr;
	Spring* presentSpring = nullptr;
	obj_mass_size = obj_vertices.size();
	//load masses
	for (int i = 0; i < obj_mass_size; i++)
	{
		CurrentPosition = obj_vertices[i];
		l = new Mass(CurrentPosition, node_mass, false, i);
		masses.push_back(l);
	}

	//load springs
	for (auto outer_it = connection.begin(); outer_it != connection.end(); ++outer_it)
	{
		l = masses[outer_it->first];
		for (auto inner_it = outer_it->second.begin(); inner_it != outer_it->second.end(); ++inner_it)
		{
			p = masses[*inner_it];
			presentSpring = new Spring(l, p, k);
			springs.push_back(presentSpring);
		}
	}


	for (auto& i : pinned_nodes) {
		masses[i]->pinned = true;
	}


	int index = 0;
	for (auto& m : masses)
	{
		if (!m->pinned)
			hash.insert(pair<int, int>(m->id, m->id - index));
		else
			index++;
	}

	for (auto& m : masses)
	{
		if (!m->pinned)
		{
			obj_M_triplets.push_back(Triplet<double>(3 * hash[m->id], 3 * hash[m->id], m->mass));
			obj_M_triplets.push_back(Triplet<double>(3 * hash[m->id] + 1, 3 * hash[m->id] + 1, m->mass));
			obj_M_triplets.push_back(Triplet<double>(3 * hash[m->id] + 2, 3 * hash[m->id] + 2, m->mass));
		}
	}
	obj_M.setFromTriplets(obj_M_triplets.begin(), obj_M_triplets.end());

	Jx = SparseMatrix<double>(3 * flexible_mass_size, 3 * flexible_mass_size);
	for (auto& s : springs)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				if (!s->m1->pinned && !s->m2->pinned)
				{
					Jx_triplets.push_back(Triplet<double>(3 * hash[s->m1->id] + i, 3 * hash[s->m1->id] + j, 1));
					Jx_triplets.push_back(Triplet<double>(3 * hash[s->m1->id] + i, 3 * hash[s->m2->id] + j, -1));
					Jx_triplets.push_back(Triplet<double>(3 * hash[s->m2->id] + i, 3 * hash[s->m1->id] + j, -1));
					Jx_triplets.push_back(Triplet<double>(3 * hash[s->m2->id] + i, 3 * hash[s->m2->id] + j, 1));
				}

				else if (s->m1->pinned && !s->m2->pinned)
					Jx_triplets.push_back(Triplet<double>(3 * hash[s->m2->id] + i, 3 * hash[s->m2->id] + j, 1));

				else if (!s->m1->pinned && s->m2->pinned)
					Jx_triplets.push_back(Triplet<double>(3 * hash[s->m1->id] + i, 3 * hash[s->m1->id] + j, 1));
			}
	}
	Jx.setFromTriplets(Jx_triplets.begin(), Jx_triplets.end());

	for (int k = 0; k < Jx.outerSize(); ++k)
	{
		std::unordered_map<int, double*> inner_map;
		for (SparseMatrix<double>::InnerIterator it(Jx, k); it; ++it)
		{
			inner_map.insert(std::pair<int, double*>(it.row(), &it.valueRef()));
		}
		nnz_pointers.insert(std::pair<int, std::unordered_map<int, double*>>(k, inner_map));
	}

	Newton_A = SparseMatrix<double>(3 * flexible_mass_size, 3 * flexible_mass_size);
	Newton_A = obj_M - Jx;
	Newton_solver.analyzePattern(Newton_A);
}

void Rope::implicitEuler(float delta_t, glm::vec3 gravity, 
	std::vector<glm::vec3>& mass_pos, std::vector<glm::vec3>& spring_pos, std::vector<glm::vec3>& normals, 
	std::vector<int>& clicked_mass_indices, std::vector <float>& clicked_mass_alpha, std::vector<glm::vec3> &clicked_mass_color, glm::vec2 clicked_pos, 
	Vector3d f_user, glm::mat4 view, glm::mat4 projection)
{
	VectorXd d(3 * springs.size());
	VectorXd new_pos(1200);
	VectorXd y(1200);
	for (auto& m : masses)
	{
		y(3 * m->id) = m->position.x + delta_t * m->velocity.x;
		y(3 * m->id + 1) = m->position.y + delta_t * m->velocity.y;
		y(3 * m->id + 2) = m->position.z + delta_t * m->velocity.z;
	}
	VectorXd f(1200);
	if (f_user(0) != 0 || f_user(1) != 0 || f_user(2) != 0)
	{
		clicked_mass_indices.clear();
		clicked_mass_alpha.clear();
		clicked_mass_color.clear();
		for (auto& m : masses)
		{
			f(3 * m->id) = m->mass * gravity.x;
			f(3 * m->id + 1) = m->mass * gravity.y;
			f(3 * m->id + 2) = m->mass * gravity.z;
			glm::vec4 m_temp_pos = projection * view * mass_model * glm::vec4(m->position, 1.0);
			glm::vec3 mass_pos = glm::vec3(m_temp_pos.x / m_temp_pos.w, m_temp_pos.y / m_temp_pos.w, m_temp_pos.z / m_temp_pos.w);
			if (pow((mass_pos.x - clicked_pos.x), 2) + pow((mass_pos.y - clicked_pos.y), 2) < 0.0025)
			{
				f(3 * m->id) += f_user(0) * 200;
				f(3 * m->id + 1) += f_user(1) * 200;
				f(3 * m->id + 2) += f_user(2) * 200;
				clicked_mass_indices.push_back(m->id);
				clicked_mass_color.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
				clicked_mass_alpha.push_back(0.8f);
			}
			else
			{
				clicked_mass_color.push_back(glm::vec3(1.0f, 0.5f, 0.31f));
				clicked_mass_alpha.push_back(1.0f);
			}
		}
	}
	else
	{
		for (auto& m : masses)
		{
			f(3 * m->id) = m->mass * gravity.x;
			f(3 * m->id + 1) = m->mass * gravity.y;
			f(3 * m->id + 2) = m->mass * gravity.z;
		}
		if (clicked_mass_indices.size() == 0)
		{
			clicked_mass_alpha.clear();
			clicked_mass_color.clear();
			for (auto& m : masses)
			{
				clicked_mass_color.push_back(glm::vec3(1.0f, 0.5f, 0.31f));
				clicked_mass_alpha.push_back(1.0f);
			}
		}
	}


	//local step
	int spring_id = 0;
	for (auto& s : springs)
	{
		Vector3d spring_v;
		//spring_v(0) = y(3 * s->m1->id) - y(3 * s->m2->id);
		//spring_v(1) = y(3 * s->m1->id + 1) - y(3 * s->m2->id + 1);
		//spring_v(2) = y(3 * s->m1->id + 2) - y(3 * s->m2->id + 2);
		spring_v[0] = s->m1->position.x - s->m2->position.x;
		spring_v[1] = s->m1->position.y - s->m2->position.y;
		spring_v[2] = s->m1->position.z - s->m2->position.z;
		spring_v.normalize();
		d(3 * spring_id) = spring_v(0) * s->rest_length;
		d(3 * spring_id + 1) = spring_v(1) * s->rest_length;
		d(3 * spring_id + 2) = spring_v(2) * s->rest_length;
		spring_id++;
	}

	//global step
	SparseMatrix<double> A(1200, 1200);
	A = M + delta_t * delta_t * L;
	VectorXd b(1200);
	b = M * y + delta_t * delta_t * (J * d + f);
	SimplicialLLT<SparseMatrix<double>> solver;
	solver.compute(A);
	new_pos = solver.solve(b);

	int k_collision = 1000;
	bool collision = false;
	for (auto& m : masses)
	{
		if (!m->pinned)
		{
			//collision detection
			Vector3d new_position(new_pos(3 * m->id), new_pos(3 * m->id + 1), new_pos(3 * m->id + 2));
			float distance = (new_position - sphere_center).norm();
			if (distance < 1)
			{
				Vector3d normal = (new_position - sphere_center).normalized();
				float length = 1 - distance;
				Vector3d F_collision(normal(0) * length * k_collision, normal(1) * length * k_collision, normal(2) * length * k_collision);
				f(3 * m->id) += F_collision(0);
				f(3 * m->id + 1) += F_collision(1);
				f(3 * m->id + 2) += F_collision(2);
				collision = true;
			}
		}
	}

	if (collision == true)
	{
		b = M * y + delta_t * delta_t * (J * d + f);
		solver.compute(A);
		new_pos = solver.solve(b);
	}
	for (auto& m : masses)
	{
		if (!m->pinned)
		{
			m->last_one_position = m->position;
			m->position = glm::vec3(new_pos(3 * m->id), new_pos(3 * m->id + 1), new_pos(3 * m->id + 2));
			m->velocity = (m->position - m->last_one_position) / delta_t;
			mass_pos.push_back(m->position);
		}
		else
		{
			mass_pos.push_back(m->position);
		}
	}

	for (auto& s : springs)
	{
		spring_pos.push_back(s->m1->position);
		spring_pos.push_back(s->m2->position);
	}

	std::vector<glm::vec3> temp_normals;
	for (int i = 0; i < column_num - 1; i++)
	{
		for (int j = 0; j < row_num - 1; j++)
		{
			glm::vec3 p1 = mass_pos[i * row_num + j];
			glm::vec3 p2 = mass_pos[i * row_num + j + 1];
			glm::vec3 p3 = mass_pos[(i + 1) * row_num + j];
			glm::vec3 normal1 = glm::normalize(glm::cross(glm::normalize(p2 - p1), glm::normalize(p3 - p1)));

			p1 = mass_pos[i * row_num + j + 1];
			p2 = mass_pos[(i + 1) * row_num + j];
			p3 = mass_pos[(i + 1) * row_num + j + 1];
			glm::vec3 normal2 = glm::normalize(glm::cross(glm::normalize(p2 - p3), glm::normalize(p1 - p3)));

			glm::vec3 normal = glm::normalize(normal1 + normal2);
			temp_normals.push_back(normal);
		}
	}

	int rectangle_indice = 0;
	for (auto& m : masses)
	{
		if (m->id < row_num)
		{
			if (m->id != row_num - 1)
				normals.push_back(temp_normals[m->id]);
			else if (m->id == row_num - 1)
				normals.push_back(temp_normals[m->id - 1]);
		}

		else
		{
			if ((m->id % row_num) != (row_num - 1))
			{
				normals.push_back(temp_normals[rectangle_indice]);
				rectangle_indice++;
			}
			else
				normals.push_back(temp_normals[rectangle_indice - 1]);
		}
	}

	//vector<Eigen::Triplet<double>> M_triplets;
	//SparseMatrix<double> M(1194 , 1194);
	//VectorXd Vc(1194);
	//VectorXd F(1194);


	//for (auto& m : masses)
	//{
	//	if (!m->pinned)
	//	{
	//		M_triplets.push_back(Triplet<double>(3 * hash[m->id], 3 * hash[m->id], m->mass));
	//		M_triplets.push_back(Triplet<double>(3 * hash[m->id] + 1, 3 * hash[m->id] + 1, m->mass));
	//		M_triplets.push_back(Triplet<double>(3 * hash[m->id] + 2, 3 * hash[m->id] + 2, m->mass));

	//		Vc(3 * hash[m->id]) = m->velocity.x;
	//		Vc(3 * hash[m->id] + 1) = m->velocity.y;
	//		Vc(3 * hash[m->id] + 2) = m->velocity.z;

	//		F(3 * hash[m->id]) = (m->mass * gravity).x;
	//		F(3 * hash[m->id] + 1) = (m->mass * gravity).y;
	//		F(3 * hash[m->id] + 2) = (m->mass * gravity).z;
	//	}
	//}

	//M.setFromTriplets(M_triplets.begin(), M_triplets.end());
	//SparseMatrix<double> Jx(1194, 1194);
	//SparseMatrix<double> Jv(1194, 1194);
	//Jx.reserve(sizes);
	//Jv.reserve(sizes);
	//float damping = 3000;
	//Matrix3d I;
	//I.setIdentity();

	////auto t1 = high_resolution_clock::now();
	//for (auto& s : springs)
	//{
	//	//Jacobian Matrix
	//	Vector3d Xi(s->m1->position.x, s->m1->position.y, s->m1->position.z);
	//	Vector3d Xj(s->m2->position.x, s->m2->position.y, s->m2->position.z);
	//	Vector3d x_dif = Xi - Xj;
	//	double l = x_dif.norm();

	//	Matrix3d Jsx_ii;
	//	Jsx_ii = I - (x_dif / l) * (x_dif / l).transpose();
	//	Jsx_ii = Jsx_ii / l;
	//	Jsx_ii = (l - s->rest_length) * Jsx_ii;
	//	Jsx_ii = Jsx_ii + (x_dif / l) * (x_dif / l).transpose();
	//	Jsx_ii = -s->k * Jsx_ii;

	//	Vector3d Vi(s->m1->velocity.x, s->m1->velocity.y, s->m1->velocity.z);
	//	Vector3d Vj(s->m2->velocity.x, s->m2->velocity.y, s->m2->velocity.z);
	//	Vector3d v_dif = Vi - Vj;
	//	Matrix3d Jdx_ii;
	//	Jdx_ii = v_dif.transpose() * (x_dif / l) * I + (x_dif / l) * v_dif.transpose();
	//	Matrix3d temp;
	//	temp = I - (x_dif / l) * (x_dif / l).transpose();
	//	temp = temp / l;
	//	Jdx_ii = Jdx_ii * temp;
	//	Jdx_ii = -damping * Jdx_ii;

	//	Matrix3d Jdv_ii;
	//	Jdv_ii = -damping * (x_dif / l) * (x_dif / l).transpose();
	//	
	//	for (int i = 0; i < 3; i++)
	//	{
	//		for (int j = 0; j < 3; j++)
	//		{
	//			if (!s->m1->pinned && !s->m2->pinned)
	//			{
	//				Jx.coeffRef(3 * hash[s->m1->id] + i, 3 * hash[s->m1->id] + j) += Jsx_ii(i, j) + Jdx_ii(i, j);
	//				Jx.coeffRef(3 * hash[s->m1->id] + i, 3 * hash[s->m2->id] + j) -= Jsx_ii(i, j) + Jdx_ii(i, j);
	//				Jx.coeffRef(3 * hash[s->m2->id] + i, 3 * hash[s->m1->id] + j) -= Jsx_ii(i, j) + Jdx_ii(i, j);
	//				Jx.coeffRef(3 * hash[s->m2->id] + i, 3 * hash[s->m2->id] + j) += Jsx_ii(i, j) + Jdx_ii(i, j);

	//				Jv.coeffRef(3 * hash[s->m1->id] + i, 3 * hash[s->m1->id] + j) += Jdv_ii(i, j);
	//				Jv.coeffRef(3 * hash[s->m1->id] + i, 3 * hash[s->m2->id] + j) -= Jdv_ii(i, j);
	//				Jv.coeffRef(3 * hash[s->m2->id] + i, 3 * hash[s->m1->id] + j) -= Jdv_ii(i, j);
	//				Jv.coeffRef(3 * hash[s->m2->id] + i, 3 * hash[s->m2->id] + j) += Jdv_ii(i, j);
	//			}
	//			else if (s->m1->pinned && !s->m2->pinned)
	//			{
	//				Jx.coeffRef(3 * hash[s->m2->id] + i, 3 * hash[s->m2->id] + j) += Jsx_ii(i, j) + Jdx_ii(i, j);

	//				Jv.coeffRef(3 * hash[s->m2->id] + i, 3 * hash[s->m2->id] + j) += Jdv_ii(i, j);
	//			}

	//			else if (!s->m1->pinned && s->m2->pinned)
	//			{
	//				Jx.coeffRef(3 * hash[s->m1->id] + i, 3 * hash[s->m1->id] + j) += Jsx_ii(i, j) + Jdx_ii(i, j);

	//				Jv.coeffRef(3 * hash[s->m1->id] + i, 3 * hash[s->m1->id] + j) += Jdv_ii(i, j);
	//			}
	//		}
	//	}
	//	//Spring Forces
	//	Vector3d fi = -s->k * (x_dif / l) * (l - s->rest_length);
	//	if (!s->m1->pinned)
	//	{
	//		F(3 * hash[s->m1->id]) += fi(0);
	//		F(3 * hash[s->m1->id] + 1) += fi(1);
	//		F(3 * hash[s->m1->id] + 2) += fi(2);
	//	}
	//	if (!s->m2->pinned)
	//	{
	//		F(3 * hash[s->m2->id]) -= fi(0);
	//		F(3 * hash[s->m2->id] + 1) -= fi(1);
	//		F(3 * hash[s->m2->id] + 2) -= fi(2);
	//	}

	//	//damping force
	//	float d = (x_dif / l).transpose() * v_dif;
	//	Vector3d fd = -damping * (x_dif / l) * d;
	//	if (!s->m1->pinned)
	//	{
	//		F(3 * hash[s->m1->id]) += fd(0);
	//		F(3 * hash[s->m1->id] + 1) += fd(1);
	//		F(3 * hash[s->m1->id] + 2) += fd(2);
	//	}
	//	if (!s->m2->pinned)
	//	{
	//		F(3 * hash[s->m2->id]) -= fd(0);
	//		F(3 * hash[s->m2->id] + 1) -= fd(1);
	//		F(3 * hash[s->m2->id] + 2) -= fd(2);
	//	}
	//}
	////auto t2 = high_resolution_clock::now();
	////duration <double, milli> time = t2 - t1;
	////std::cout << time.count() << "ms\n";

	//SparseMatrix<double> A(1194, 1194);
	//VectorXd b(1194);
	//A = M - delta_t * delta_t * Jx - delta_t * Jv;
	//b = delta_t * (F + delta_t * Jx * Vc);
	//VectorXd dV(1194);


	//SimplicialLLT<SparseMatrix<double>> solver;
	//solver.compute(A);
	//dV = solver.solve(b);

	//for (auto& m : masses)
	//{
	//	if (!m->pinned)
	//	{
	//		glm::vec3 v(dV(3 * hash[m->id]), dV(3 * hash[m->id] + 1), dV(3 * hash[m->id] + 2));
	//		m->velocity = m->velocity + v;
	//		m->position = m->position + delta_t * m->velocity;
	//		mass_pos.push_back(m->position);
	//	}
	//	else
	//	{
	//		mass_pos.push_back(m->position);
	//	}
	//}

	//for (auto& s : springs)
	//{
	//	spring_pos.push_back(s->m1->position);
	//	spring_pos.push_back(s->m2->position);
	//}


}


void Rope::obj_animation(float delta_t, glm::vec3 gravity, std::vector<glm::vec3>& obj_spring_pos, std::vector<glm::vec3>& obj_vertices, std::vector<glm::vec3>& obj_normals, std::map<int, std::set<int>> connection)
{
	obj_spring_pos.clear();
	VectorXd d(3 * obj_spring_size);
	VectorXd y(3 * obj_mass_size);
	VectorXd f(3 * obj_mass_size);
	VectorXd new_pos(3 * obj_mass_size);
	VectorXd last_pos(3 * obj_mass_size);

	VectorXd stiffness(obj_spring_size);
	VectorXd restlength(obj_spring_size);
	int a = 0;
	for (auto& s : springs)
	{
		stiffness(a) = s->k;
		restlength(a) = s->rest_length;
		a++;
	}

	for (auto& m : masses)
	{
		y(3 * m->id) = 2 * m->position.x - m->last_one_position.x;
		y(3 * m->id + 1) = 2 * m->position.y - m->last_one_position.y;
		y(3 * m->id + 2) = 2 * m->position.z - m->last_one_position.z;
		f(3 * m->id) = m->mass * gravity.x;
		f(3 * m->id + 1) = m->mass * gravity.y;
		f(3 * m->id + 2) = m->mass * gravity.z;
		new_pos(3 * m->id) = m->position.x;
		new_pos(3 * m->id + 1) = m->position.y;
		new_pos(3 * m->id + 2) = m->position.z;
	}

	int spring_id = 0;
	SparseMatrix<double> A(3 * obj_mass_size, 3 * obj_mass_size);
	A = obj_M + delta_t * delta_t * obj_L;
	VectorXd b(3 * obj_mass_size);
	SimplicialLLT<SparseMatrix<double>> solver;
	solver.compute(A);


	VectorXd residual(3 * obj_mass_size);
	int j = 0;
	double last_residual = 0.0;
	double current_residual = 0.0;
	

	//global steps and local steps
	while(1)
	{
		spring_id = 0;
		for (auto& s : springs)
		{
			Vector3d spring_v;
			spring_v(0) = new_pos(3 * s->m1->id) - new_pos(3 * s->m2->id);
			spring_v(1) = new_pos(3 * s->m1->id + 1) - new_pos(3 * s->m2->id + 1);
			spring_v(2) = new_pos(3 * s->m1->id + 2) - new_pos(3 * s->m2->id + 2);
			spring_v.normalize();
			d(3 * spring_id) = spring_v(0) * s->rest_length;
			d(3 * spring_id + 1) = spring_v(1) * s->rest_length;
			d(3 * spring_id + 2) = spring_v(2) * s->rest_length;
			spring_id++;
		}
		b = obj_M * y + delta_t * delta_t * (obj_J * d + f);
		last_pos = new_pos;
		new_pos = solver.solve(b);

		residual.setZero();
		j = 0;
		for (auto outer_it = connection.begin(); outer_it != connection.end(); ++outer_it)
		{
			Mass* first_node = masses[outer_it->first];
			Mass* second_node = NULL;
			for (auto inner_it = outer_it->second.begin(); inner_it != outer_it->second.end(); ++inner_it)
			{
				second_node = masses[*inner_it];
				Vector3f first_pos(new_pos(3 * first_node->id), new_pos(3 * first_node->id + 1), new_pos(3 * first_node->id + 2));
				Vector3f second_pos(new_pos(3 * second_node->id), new_pos(3 * second_node->id + 1), new_pos(3 * second_node->id + 2));
				Vector3f pos21 = (second_pos - first_pos).normalized();
				float length = (second_pos - first_pos).norm();
				Vector3f force12 = stiffness(j) * (length - restlength(j)) * pos21;
				residual(3 * first_node->id) += force12(0);
				residual(3 * first_node->id + 1) += force12(1);
				residual(3 * first_node->id + 2) += force12(2);
				residual(3 * second_node->id) -= force12(0);
				residual(3 * second_node->id + 1) -= force12(1);
				residual(3 * second_node->id + 2) -= force12(2);
				j++;
			}
		}
		for (auto& m : masses)
		{
			if (!m->pinned)
			{
				residual(3 * m->id + 1) += gravity.y * m->mass;
			}
			else
			{
				residual(3 * m->id) = 0;
				residual(3 * m->id + 1) = 0;
				residual(3 * m->id + 2) = 0;
			}
		}
		
		last_residual = current_residual;
		current_residual = residual.norm();
		
		if (abs(last_residual - current_residual) < 0.001)
			break;
	}

	
	
	
	for (auto& m : masses)
	{
		if (!m->pinned)
		{
			m->last_one_position = m->position;
			m->position = glm::vec3(new_pos(3 * m->id), new_pos(3 * m->id + 1), new_pos(3 * m->id + 2));
			m->velocity = glm::vec3(m->position.x - m->last_one_position.x, m->position.y - m->last_one_position.y, m->position.z - m->last_one_position.z) / delta_t;
		}
	}




	for (auto& s : springs)
	{
		obj_spring_pos.push_back(s->m1->position);
		obj_spring_pos.push_back(s->m2->position);
	}

}

void Rope::obj_Newton_animation(float delta_t, glm::vec3 gravity, std::vector<glm::vec3>& obj_spring_pos, std::map<int, std::set<int>> connection)
{

	obj_spring_pos.clear();
	VectorXd Vc(3 * flexible_mass_size);
	VectorXd F(3 * flexible_mass_size);
	VectorXd temp_pos(3 * flexible_mass_size);
	VectorXd residual(3 * flexible_mass_size);
	double last_residual = 0.0;
	double current_residual = 0.0;

	for (auto& m : masses)
	{
		if (!m->pinned)
		{
			Vc(3 * hash[m->id]) = m->velocity.x;
			Vc(3 * hash[m->id] + 1) = m->velocity.y;
			Vc(3 * hash[m->id] + 2) = m->velocity.z;

			F(3 * hash[m->id] + 1) = m->mass * gravity.y;

			temp_pos(3 * hash[m->id]) = m->position.x;
			temp_pos(3 * hash[m->id] + 1) = m->position.y;
			temp_pos(3 * hash[m->id] + 2) = m->position.z;
		}
	}

	Matrix3d I;
	I.setIdentity();
	VectorXd b(3 * flexible_mass_size);
	VectorXd Vn(3 * flexible_mass_size);

	for(int i = 0; i < 10; i++)
	{
		for (int k = 0; k < Jx.outerSize(); ++k)
			for (SparseMatrix<double>::InnerIterator it(Jx, k); it; ++it)
			{
				it.valueRef() = 0;
			}
		Vector3d Xi, Xj;
		for (auto& s : springs)
		{
			if (!s->m1->pinned)
				Xi = Vector3d(temp_pos(3 * hash[s->m1->id]), temp_pos(3 * hash[s->m1->id] + 1), temp_pos(3 * hash[s->m1->id] + 2));
			else
				Xi = Vector3d(s->m1->position.x, s->m1->position.y, s->m1->position.z);
			
			if (!s->m2->pinned)
				Xj = Vector3d(temp_pos(3 * hash[s->m2->id]), temp_pos(3 * hash[s->m2->id] + 1), temp_pos(3 * hash[s->m2->id] + 2));
			else
				Xj = Vector3d(s->m2->position.x, s->m2->position.y, s->m2->position.z);

			Vector3d x_dif = Xi - Xj;
			Vector3d x_dif_uv = x_dif.normalized();
			double l = x_dif.norm();

			Matrix3d Jsx_ii;
			Jsx_ii = I - x_dif_uv * x_dif_uv.transpose();
			Jsx_ii = Jsx_ii / l;
			Jsx_ii = (l - s->rest_length) * Jsx_ii;
			Jsx_ii = Jsx_ii + x_dif_uv * x_dif_uv.transpose();
			Jsx_ii = -s->k * Jsx_ii;

			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
				{
					if (!s->m1->pinned && !s->m2->pinned)
					{
						*nnz_pointers[3 * hash[s->m1->id] + j][3 * hash[s->m1->id] + i] += Jsx_ii(i, j);
						*nnz_pointers[3 * hash[s->m2->id] + j][3 * hash[s->m1->id] + i] -= Jsx_ii(i, j);
						*nnz_pointers[3 * hash[s->m1->id] + j][3 * hash[s->m2->id] + i] -= Jsx_ii(i, j);
						*nnz_pointers[3 * hash[s->m2->id] + j][3 * hash[s->m2->id] + i] += Jsx_ii(i, j);
					}

					else if (s->m1->pinned && !s->m2->pinned)
						*nnz_pointers[3 * hash[s->m2->id] + j][3 * hash[s->m2->id] + i] += Jsx_ii(i, j);


					else if (!s->m1->pinned && s->m2->pinned)
						*nnz_pointers[3 * hash[s->m1->id] + j][3 * hash[s->m1->id] + i] += Jsx_ii(i, j);
				}

			if (i == 0)
			{
				Vector3d fi = -s->k * x_dif_uv * (l - s->rest_length);
				if (!s->m1->pinned)
				{
					F(3 * hash[s->m1->id]) += fi(0);
					F(3 * hash[s->m1->id] + 1) += fi(1);
					F(3 * hash[s->m1->id] + 2) += fi(2);
				}
				if (!s->m2->pinned)
				{
					F(3 * hash[s->m2->id]) -= fi(0);
					F(3 * hash[s->m2->id] + 1) -= fi(1);
					F(3 * hash[s->m2->id] + 2) -= fi(2);
				}
			}
		}
		Jx.makeCompressed();
		Newton_A = obj_M - delta_t * delta_t * Jx;
		b = obj_M * Vc + delta_t * F;
		Newton_solver.factorize(Newton_A);
		Vn = Newton_solver.solve(b);

		for (auto& m : masses)
		{
			if (!m->pinned)
			{
				temp_pos(3 * hash[m->id]) += Vn(3 * hash[m->id]) * delta_t;
				temp_pos(3 * hash[m->id] + 1) += Vn(3 * hash[m->id] + 1) * delta_t;
				temp_pos(3 * hash[m->id] + 2) += Vn(3 * hash[m->id] + 2) * delta_t;
			}
		}
	}


	for (auto& m : masses)
	{
		if (!m->pinned)
		{
			m->velocity = glm::vec3(Vn(3 * hash[m->id]), Vn(3 * hash[m->id] + 1), Vn(3 * hash[m->id] + 2));
			m->position = m->position + delta_t * m->velocity;
		}
	}

	for (auto& s : springs)
	{
		obj_spring_pos.push_back(s->m1->position);
		obj_spring_pos.push_back(s->m2->position);
	}



	//obj_spring_pos.clear();
	//VectorXd Vc(3 * flexible_mass_size);
	//VectorXd F(3 * flexible_mass_size);
	//VectorXd temp_pos(3 * flexible_mass_size);
	//VectorXd residual(3 * flexible_mass_size);
	//double last_residual = 0.0;
	//double current_residual = 0.0;

	//for (auto& m : masses)
	//{
	//	if (!m->pinned)
	//	{
	//		Vc(3 * hash[m->id]) = m->velocity.x;
	//		Vc(3 * hash[m->id] + 1) = m->velocity.y;
	//		Vc(3 * hash[m->id] + 2) = m->velocity.z;

	//		F(3 * hash[m->id] + 1) = m->mass * gravity.y;

	//		temp_pos(3 * hash[m->id]) = m->position.x;
	//		temp_pos(3 * hash[m->id] + 1) = m->position.y;
	//		temp_pos(3 * hash[m->id] + 2) = m->position.z;
	//	}
	//}
	//
	//Matrix3d I;
	//I.setIdentity();
	//VectorXd b(3 * flexible_mass_size);
	//VectorXd Vn(3 * flexible_mass_size);


	//
	//for (int k = 0; k < Jx.outerSize(); ++k)
	//	for (SparseMatrix<double>::InnerIterator it(Jx, k); it; ++it)
	//	{
	//		it.valueRef() = 0;
	//	}

	//for (auto& s : springs)
	//{
	//	Vector3d Xi(temp_pos(3 * hash[s->m1->id]), temp_pos(3 * hash[s->m1->id] + 1), temp_pos(3 * hash[s->m1->id] + 2));
	//	Vector3d Xj(temp_pos(3 * hash[s->m2->id]), temp_pos(3 * hash[s->m2->id] + 1), temp_pos(3 * hash[s->m2->id] + 2));
	//	Vector3d x_dif = Xi - Xj;
	//	double l = x_dif.norm();

	//	Matrix3d Jsx_ii;
	//	Jsx_ii = I - (x_dif / l) * (x_dif / l).transpose();
	//	Jsx_ii = Jsx_ii / l;
	//	Jsx_ii = (l - s->rest_length) * Jsx_ii;
	//	Jsx_ii = Jsx_ii + (x_dif / l) * (x_dif / l).transpose();
	//	Jsx_ii = -s->k * Jsx_ii;

	//	for (int i = 0; i < 3; i++)
	//		for (int j = 0; j < 3; j++)
	//		{
	//			if (!s->m1->pinned && !s->m2->pinned)
	//			{

	//				*nnz_pointers[3 * hash[s->m1->id] + j][3 * hash[s->m1->id] + i] += Jsx_ii(i, j);
	//				*nnz_pointers[3 * hash[s->m2->id] + j][3 * hash[s->m1->id] + i] -= Jsx_ii(i, j);
	//				*nnz_pointers[3 * hash[s->m1->id] + j][3 * hash[s->m2->id] + i] -= Jsx_ii(i, j);
	//				*nnz_pointers[3 * hash[s->m2->id] + j][3 * hash[s->m2->id] + i] += Jsx_ii(i, j);
	//				
	//			}

	//			else if (s->m1->pinned && !s->m2->pinned)
	//				*nnz_pointers[3 * hash[s->m2->id] + j][3 * hash[s->m2->id] + i] += Jsx_ii(i, j);


	//			else if (!s->m1->pinned && s->m2->pinned)
	//				*nnz_pointers[3 * hash[s->m1->id] + j][3 * hash[s->m1->id] + i] += Jsx_ii(i, j);
	//		}

	//	Vector3d fi = -s->k * (x_dif / l) * (l - s->rest_length);
	//	if (!s->m1->pinned)
	//	{
	//		F(3 * hash[s->m1->id]) += fi(0);
	//		F(3 * hash[s->m1->id] + 1) += fi(1);
	//		F(3 * hash[s->m1->id] + 2) += fi(2);
	//	}
	//	if (!s->m2->pinned)
	//	{
	//		F(3 * hash[s->m2->id]) -= fi(0);
	//		F(3 * hash[s->m2->id] + 1) -= fi(1);
	//		F(3 * hash[s->m2->id] + 2) -= fi(2);
	//	}
	//}
	//Jx.makeCompressed();
	//Newton_A = obj_M - delta_t * delta_t * Jx;
	//b = obj_M * Vc + delta_t * F;
	//Newton_solver.factorize(Newton_A);
	//Vn = Newton_solver.solve(b);


	//for (auto& m : masses)
	//{
	//	if (!m->pinned)
	//	{
	//		m->velocity = glm::vec3 (Vn(3 * hash[m->id]), Vn(3 * hash[m->id] + 1), Vn(3 * hash[m->id] + 2));
	//		m->position = m->position + delta_t * m->velocity;
	//	}
	//}

	//for (auto& s : springs)
	//{
	//	obj_spring_pos.push_back(s->m1->position);
	//	obj_spring_pos.push_back(s->m2->position);
	//}

}