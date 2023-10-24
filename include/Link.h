#pragma once
#include <string>

struct Link {
	Node* first = nullptr;
	Node* second = nullptr;
	
	float rest_length;
	float spring_k;
	float damp_k;

	bool ripped;

	Link() {}
	Link(Node* first, Node* second, float);

	void constrain();

	void updateNodesSDF(float dt, float air);

	void updateNodesSDF(float dt);


	void draw();
	void updatePhysics(float);

	void updatePhysicsVerlet(float dt);

	std::string toString() { 
		return first->position.toString() + ", " + second->position.toString(); 
	}
	
};

