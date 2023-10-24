#pragma once

#include "Node.h"
#include "Link.h"
#include "Parameters.h"
#include <raylib.h>
#include <iostream>


Link::Link(Node* first, Node* second, float rest_length) {
	this->first = first;
	this->second = second;
	this->rest_length = rest_length;
	spring_k = 100000;
	damp_k = 1000;

	ripped = false;
}


void Link::constrain() {
	// Constrain the distance between nodes to the link length
	for (int i = 0; i < relaxation_steps; i++) {

		Vec3 delta = second->position - first->position;
		float correction = delta.length() - rest_length;
		Vec3 delta_normalized = delta.normalized();
		second->setPosition(second->position - delta_normalized * (correction / 2));
		first->setPosition(first->position + delta_normalized * (correction / 2));
	}
}

// Updates the nodes to add spring and damping forces on them
void Link::updateNodesSDF(float dt, float air) 
{
	// e*
	Vec3 string_dir = second->position - first->position;
	// l
	float stringLength = second->position.distanceTo(first->position);

	if (stringLength > rest_length * 2) 
		ripped = true;

	// e = e* / l
	string_dir.normalize();

	float stringForce = - spring_k * (rest_length - stringLength);
	float dampForce = - (dot(string_dir, first->velocity) - dot(string_dir, second->velocity)) * damp_k;

	first->velocity += string_dir * stringForce * dt;
	first->velocity += string_dir * dampForce * dt;

	second->velocity -= string_dir * dampForce * dt;
	second->velocity -= string_dir * stringForce * dt;

	float fricForce = - (dot(string_dir, first->velocity)) * (air);
	first->velocity += string_dir * fricForce * dt;
	float fricForce2 = (dot(string_dir, second->velocity)) * (air);
	second->velocity += string_dir * fricForce * dt;

	// std::cout << std::to_string(stringLength) << std::endl;
	// std::cout << std::to_string(stringForce) << std::endl;
	// std::cout << std::to_string(dampForce) << std::endl;
}

// Eular Integration
void Link::updatePhysics(float dt)
{
	constrain();

	// Update the velocities (PBD)
	first->velocity = (first->position - first->last_position) / dt;
	second->velocity = (second->position - second->last_position) / dt;
}

void Link::updatePhysicsVerlet(float dt) {
	constrain();
}

void Link::draw() {
	DrawLine3D(
		{ (float)first->position.x, (float)first->position.y, (float)first->position.z },
		{ (float)second->position.x, (float)second->position.y, (float)second->position.z },
		BLACK
	);
}
