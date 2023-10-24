#pragma once

void update_physics(float dt, Node& base, Node& node1, Node& node2) {
	// Semi-implicit Integration
	node1.last_position = node1.position;
	node1.velocity = node1.velocity + gravity * dt;
	node1.setPosition(node1.position + node1.velocity * dt);

	// TODO: Simulate node 2's position & velocity
	node2.last_position = node2.position;
	node2.velocity = node2.velocity + gravity * dt;
	node2.setPosition(node2.position + node2.velocity * dt);

	// Constrain the distance between nodes to the link length
	for (int i = 0; i < relaxation_steps; i++) {
		Vec3 delta = node1.position - base.position;
		float delta_len = delta.length();
		float correction = delta_len - link_length;
		Vec3 delta_normalized = delta.normalized();
		node1.setPosition(node1.position - delta_normalized * (correction / 2));
		base.setPosition(base.position + delta_normalized * (correction / 2));

		// TODO: Constrain distance between node2 and node1
		//       Make sure you update the position of both notes!
		Vec3 delta1 = node2.position - node1.position;
		float delta1_len = delta1.length();
		float correction1 = delta1_len - link_length;
		Vec3 delta1_normalized = delta1.normalized();
		node2.setPosition(node2.position - delta1_normalized * (correction1 / 2));
		node1.setPosition(node1.position + delta1_normalized * (correction1 / 2));

		base.setPosition(base_pos); // Fix the base node in place
	}

	// Update the velocities (PBD)
	base.velocity = (base.position - base.last_position) / dt;
	node1.velocity = (node1.position - node1.last_position) / dt;
	node2.velocity = (node2.position - node2.last_position) / dt;
}

void update_physics(float dt, std::vector<std::shared_ptr<Node>>& nodes, std::vector<std::shared_ptr<Link>>& links) {
	for (const auto& nodePtr : nodes) {
		Node* node = nodePtr.get();
		node->last_position = node->position;
		node->velocity = node->velocity + gravity * dt;
		node->setPosition(node->position + node->velocity * dt);
	}


	// Constrain the distance between nodes to the link length
	for (int k = 0; k < relaxation_steps; k++) {
		for (auto link : links){
		// for (int i = 0; i < nodes.size() - 1; i++) {
			// Node* node1 = nodes[i].get();
			// Node* node2 = nodes[i + 1].get();
			Node* node1 = link.get()->first;
			Node* node2 = link.get()->second;

			Vec3 delta = node2->position - node1->position;
			float delta_len = delta.length();
			float correction = delta_len - link_length;
			Vec3 delta_normalized = delta.normalized();

			node2->setPosition(node2->position - delta_normalized * (correction / 2));
			node1->setPosition(node1->position + delta_normalized * (correction / 2));
		}
		
		for (auto n : nodes) {
			if (n.get()->fixed)
				n->setPosition(n->fixed_position); // Fix the base node in place
		}
	}

	// Update the velocities (PBD)
	for (const auto& nodePtr : nodes) {
		Node* node = nodePtr.get();
		node->velocity = (node->position - node->last_position) / dt;
	}

}

void tearingUpdate(Color& c, std::vector<std::shared_ptr<Link>>& links, Camera3D& camera);

void createConnections(std::vector<std::vector<std::shared_ptr<Node>>>& all_nodes, std::vector<std::shared_ptr<Link>>& links);
