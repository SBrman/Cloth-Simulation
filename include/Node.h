#pragma once
#include "Vec3.hpp"
#include "raylib.h"

struct Node
{
	float mass;
	Vec3 position;
	Vec3 velocity;
	Vec3 last_position;
	Vec3 fixed_position;
	
	Color node_color;

	bool fixed;
	float radius;

	Node(): mass(1.0f), radius(0.0f), fixed(false) {}

	Node(Vec3, bool);

	void setPosition(Vec3);
	void setPosition(float x, float y, float z);
	
	void update_physics(float);
	void apply_force(float dt);
	void update_physicsVerlet(float dt);
	void draw();

	void bounce_floor();
	
};

