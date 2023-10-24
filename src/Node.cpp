#pragma once

#include "Node.h"
#include "Parameters.h"
#include <raylib.h>

Node::Node(Vec3 position, bool fixed = false) {

	mass = 1.0f;
	this->position = position;
	velocity = Vec3{ 0, 0, 0.01 };
	last_position = position;

	this->fixed = fixed;
	this->fixed_position = position;

	node_color = RED;

	radius = 0.0002;
}

void Node::setPosition(Vec3 position) {
	if (!fixed) {
		this->position = position;
	}
}

void Node::setPosition(float x, float y, float z) {
	if (!fixed) {
		position = Vec3{ x, y, z };
	}

	bounce_floor();
}

void Node::draw() {
	// DrawCircle(position.x * SCENE_SCALE, position.y * SCENE_SCALE, 5, RED);
	DrawSphere( { (float)position.x, (float)position.y, (float)position.z }, radius*SCENE_SCALE, node_color);
	node_color = RED;
}

void Node::bounce_floor() {
	if (position.y + radius > HEIGHT) {
		velocity.y *= -0.9;
		position.y = (double) HEIGHT - radius;
	}
	if (position.y + radius < 0) {
		velocity.y *= -0.9;
		position.y = radius;
	}
}

void Node::update_physics(float dt){

	last_position = position;
	velocity += gravity * dt;
	setPosition(position + velocity * dt);
	
}


// Does not work
void Node::apply_force(float dt) {
	setPosition(position + velocity * dt);
}

void Node::update_physicsVerlet(float dt){
	apply_force(dt);

	Vec3 temp = position;
	setPosition((position * 2 - last_position) + gravity * dt * dt);
	last_position = temp;
	
}
