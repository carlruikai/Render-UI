#ifndef SPRING_H
#define SPRING_H
#include "mass.h"
#include <iostream>

struct Spring
{
	Spring(Mass* a, Mass* b, float k)
		: m1(a), m2(b), k(k), rest_length(glm::length(a->position - b->position)) {}

	float k;
	double rest_length;

	Mass* m1;
	Mass* m2;
};
#endif // !SPRING_H
