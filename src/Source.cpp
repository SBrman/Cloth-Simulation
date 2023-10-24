// Source.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include "Vec3.hpp"
#include "raylib.h"
#include "Node.h"
#include "Link.h"
#include "Parameters.h"
#include "Source.h"
#include "rlgl.h"


const int WIDTH = 1500;
const int HEIGHT = 1000;
const float SCALER = 20.0f;
const float SCENE_SCALE = WIDTH / SCALER;
const float COR = 0.1;

const float link_length = 0.2;
Vec3 gravity{ 0, -10, 0 };
Vec3 base_pos{ 5, 5, 0 };

// Physics Parameters
const int relaxation_steps = 0;

int sub_steps = 100;

const int ROWS = 25;
const int COLUMNS = 25;



struct Sphere {
	
	Vec3 position;
	float radius;

	Sphere(Vec3 p, float r) : position(p), radius(r) {};

	void draw() {
		DrawSphere( { (float)position.x, (float)position.y, (float)position.z }, radius, BLUE);
	}

};




void tearingUpdate(Color& c, std::vector<std::shared_ptr<Link>>& links, Camera3D& camera)
{
	// User interaction and tearing
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		c = BLUE;

		for (auto link : links) {
			Node* n1 = link->first;
			Node* n2 = link->second;

			float n1x = n1->position.x - camera.target.x;
			float n1y = n1->position.y - camera.target.y;
			float n1z = n1->position.z - camera.target.z;
			// float dist_to_node1 = std::sqrt((n1x * n1x) + (n1y * n1y) + (n1z * n1z));
			float dist_to_node1 = std::sqrt((n1x * n1x) + (n1y * n1y));

			float n2x = n2->position.x - camera.target.x;
			float n2y = n2->position.y - camera.target.y;
			float n2z = n2->position.z - camera.target.z;
			// float dist_to_node2 = std::sqrt((n2x * n2x) + (n2y * n2y) + (n2z * n2z));
			float dist_to_node2 = std::sqrt((n2x * n2x) + (n2y * n2y));

			if (dist_to_node1 < link->rest_length && dist_to_node2 < link->rest_length) {
				// std::cout << n1->position.toString() << " " << n2->position.toString() << std::endl;
				link->ripped = true;
			}
		}
	}
}

void createConnections(std::vector<std::vector<std::shared_ptr<Node>>>& all_nodes, std::vector<std::shared_ptr<Link>>& links)
{
	for (int j = 0; j < ROWS; j++)
	{
		for (int i = 0; i < COLUMNS; i++)
		{
			Node* n = all_nodes[j][i].get();
			if (j < ROWS - 1) {
				// Vertical 
				Node* nv = all_nodes[j + 1][i].get();
				links.push_back(std::make_shared<Link>(Link{ n, nv, link_length }));

				if (j < ROWS - 2) {
					Node* nv1 = all_nodes[j + 2][i].get();
					links.push_back(std::make_shared<Link>(Link{ n, nv1, 2 * link_length }));
				}

				if (i < COLUMNS - 1) {
					Node* nd_lb = all_nodes[j + 1][i + 1].get();
					links.push_back(std::make_shared<Link>(Link{ n, nd_lb, std::sqrt(2 * link_length * link_length) }));
				}
			}

			if (i < COLUMNS - 1)
			{
				Node* nh = all_nodes[j][i + 1].get();
				links.push_back(std::make_shared<Link>(Link{ n, nh, link_length }));

				if (i < COLUMNS - 2) {
					Node* nh1 = all_nodes[j][i + 2].get();
					links.push_back(std::make_shared<Link>(Link{ n, nh1, 2 * link_length }));
				}

				if (j > 0 && j < ROWS) {
					Node* nd_rb = all_nodes[j - 1][i + 1].get();
					links.push_back(std::make_shared<Link>(Link{ n, nd_rb, std::sqrt(2 * link_length * link_length) }));
				}
			}
		}
	}
}



#include "raymath.h"
#include "rlights.h"


int main()
{
	float dt = 1 / 30.0f;

	InitWindow(WIDTH, HEIGHT, "Cloth Simulation [CSCI 5611]");
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetConfigFlags(FLAG_INTERLACED_HINT);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	SetConfigFlags(FLAG_WINDOW_TOPMOST);
	//SetConfigFlags(FLAG_WINDOW_MOUSE_PASSTHROUGH);

	SetTargetFPS(60);

	// Creating all the nodes
	std::vector<std::vector<std::shared_ptr<Node>>> all_nodes;
	for (int r = 0; r < ROWS; r++) 
	{
		std::vector<std::shared_ptr<Node>> nodes;
		for (int c = 0; c < COLUMNS; c++) 
		{
			// Vec3 position{ 5 + (double) r * link_length + (double) c * link_length, 5, 0};
			// Vec3 position{ (double)r + 5, 5, 0 };

			// Grid
			Vec3 position{(double) c * link_length, 5 - (double) r * link_length, 0 };

			std::shared_ptr<Node> n = std::make_shared<Node>(position, 0);
			nodes.push_back(n);
		}
		all_nodes.push_back(nodes);
	}

	// for (int j = 0; j < ROWS; j++)
		// all_nodes[0][j].get()->fixed = 1;

	// Fixing the first and the last node slightly closer to each other to get a ripple like effect
	all_nodes[0][0].get()->position = Vec3 { 0, 5, 0 };
	all_nodes[0][0].get()->fixed = 1;

	all_nodes[0][ROWS - 1].get()->position.x *= 0.95;
	all_nodes[0][ROWS-1].get()->fixed = 1;


	// Creating the links
	std::vector<std::shared_ptr<Link>> links;
	createConnections(all_nodes, links);


	// std::vector<std::shared_ptr<Node>> nodes;
	// for (auto ns : all_nodes)
	// 	for (auto n : ns)
	// 		nodes.push_back(n);

	// Sphere to collide with
	Sphere sphere{ Vec3(5, 5, 1), 0.3};


	
	// Camera stuff
	Camera3D camera = { 0 };
	camera.position = { 8.0f, 5.0f, 8.0f };
	camera.target = { 3.0f, 5.0f, 3.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	
	// Taken from raylib examples
	// ----------------------------------------------------------------------------------------
	// Load plane model from a generated mesh
	Model model = LoadModelFromMesh(GenMeshPlane(30.0f, 30.0f, 3, 3));
	// Load basic lighting shader
	Shader shader = LoadShader("lighting.vs", "lighting.fs");
	// Get some required shader locations
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
	// Ambient light level (some basic lighting)
	int ambientLoc = GetShaderLocation(shader, "ambient");
	float ff[4] { 0.1f, 0.1f, 0.1f, 1.0f };
	SetShaderValue(shader, ambientLoc, ff, SHADER_UNIFORM_VEC4);
	// Assign out lighting shader to model
	model.materials[0].shader = shader;

	// Create lights
	Light lights[MAX_LIGHTS] = { 0 };
	lights[0] = CreateLight(LIGHT_POINT, Vector3 { -2, 1, -2 }, Vector3Zero(), WHITE, shader);
	lights[1] = CreateLight(LIGHT_POINT, Vector3 { -2, 1, -2 }, Vector3Zero(), RED, shader);
	//--------------------------------------------------------------------------------------


	// Texture2D texture = LoadTexture("texture/c1.jpg");
	
	Image image = GenImageChecked(10, 10, 1, 1, RED, GREEN);
	Texture2D texture = LoadTextureFromImage(image);


	bool ripped = false;
	bool tearMode = false;
	bool wireMode = true;

	bool airLow = true;


	// Main game loop
	while (!WindowShouldClose())        // Detect window close button or ESC key
	{

		Vec3 mp3d{ camera.target.x, camera.target.y, camera.target.z };
		sphere.position = mp3d;
		Color c = WHITE;

		if (IsKeyPressed(KeyboardKey::KEY_SPACE)) { tearMode = !tearMode; }
		if (IsKeyPressed(KeyboardKey::KEY_ENTER)) { wireMode = !wireMode; }
		if (IsKeyPressed(KeyboardKey::KEY_L)) { airLow = !airLow; }


		if (tearMode) {
			c = GREEN;
			tearingUpdate(c, links, camera);
		}

		float physics = true;

		// Update Physics
		if (physics) {
			for (int i = 0; i < sub_steps; i++) {

				// Fallback: the following works
				// update_physics(dt / sub_steps, nodes, links);

				std::vector<int> ripped_links;

				// Apply spring and damping forces
				// for (auto link : links) {
				float drag = airLow ? 10.0f : 30.0f;
				for (int i = 0; i < links.size(); i++) {
					links[i]->updateNodesSDF(dt / sub_steps, drag);

					if (links[i]->ripped) 
						ripped_links.push_back(i);
				}

				for (int i : ripped_links) {
					if (i < links.size()) {
						links.erase(links.begin() + i);
						ripped = true;
					}
				}

				// std::cout << sphere.position.toString() << std::endl;
				bool self_collision = false;

				// Update nodes physics
				for (auto row_nodes : all_nodes)
					for (auto node : row_nodes) {
						
						if (self_collision && !tearMode) {
							for (auto rns : all_nodes) {
								for (auto n : rns) {
									if (&node != &n) {

										Vec3 dir = n->position - node->position;
										Vec3 normal = dir.normalized();

										// std::cout << normal.toString() << normal.length() << " " << dir.toString() << std::endl;
											
										if (dir.length() < 0.1) {
											Vec3 v0 = (normal * dot(node->velocity, normal) * 2) * COR;
											Vec3 v1 = (normal * dot(n->velocity, normal) * 2) * COR;
											node->velocity.x -= std::min(v0.x, 0.1);
											node->velocity.y -= std::min(v0.y, 0.1);
											node->velocity.z -= std::min(v0.z, 0.1);
											n->velocity.x -= std::min(v1.x, 0.1);
											n->velocity.y -= std::min(v1.y, 0.1);
											n->velocity.z -= std::min(v1.z, 0.1);

											node->position -= normal * (n->position.distanceTo(node->position) / 2);
											n->position += normal * (n->position.distanceTo(node->position) / 2);
										}


									}
								}
							}
						}

						if (!tearMode){
							float distance = sphere.position.distanceTo(node->position);
							// if (distance < sphere.radius + node->radius) {
							if (distance < sphere.radius + 0.05) {

								Vec3 normal = node->position - sphere.position;
								normal.normalize();

								node->velocity -= (normal * dot(node->velocity, normal) * 2) * COR;
								node->position += normal * (0.05 + sphere.radius - distance);
								node->node_color = GREEN;
							}
						}

						node->update_physics(dt / sub_steps);
						// Node update physics Verlet does not work [Fix later]
						// node->update_physicsVerlet(dt/sub_steps);
					}


				// Update link physics (constraints). 
				// PBD is done right after constraints solver solves, to update node velocities.
				for (auto link : links) {
					link->updatePhysics(dt / sub_steps);
					// link->updatePhysicsVerlet(dt/sub_steps);
				}
			}
		}

		UpdateCamera(&camera, CAMERA_ORTHOGRAPHIC);


		// Taken from raylib examples
		// ----------------------------------------------------------------------------------------
		// Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
		// Check key inputs to enable/disable lights
		if (IsKeyPressed(KEY_L)) lights[1].enabled = !lights[1].enabled; 
		// Update light values (actually, only enable/disable them)
		for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);
		//----------------------------------------------------------------------------------

		lights[1].color = (!tearMode)? BLUE : GREEN;

		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode3D(camera);
		{
			// Taken from raylib examples
			// ----------------------------------------------------------------------------------------
			DrawModel(model, Vector3Zero(), 1.0f, WHITE);
			//----------------------------------------------------------------------------------

			// Draw spheres to show where the lights are
			if (lights[0].enabled) DrawSphereEx(lights[0].position, 0.2f, 8, 8, lights[0].color);
			
			if (tearMode) {
				DrawSphere( { (float)mp3d.x, (float)mp3d.y, (float) mp3d.z }, 0.0005*SCENE_SCALE, c);
			}
			else {
				// sphere.draw();
				lights[1].position.x = sphere.position.x;
				lights[1].position.y = sphere.position.y;
				lights[1].position.z = sphere.position.z;
				DrawSphereEx(Vector3{ (float)mp3d.x, (float)mp3d.y, (float) mp3d.z }, sphere.radius, 8, 8, lights[1].color);
				DrawSphereWires(lights[1].position, sphere.radius, 8, 8, ColorAlpha(BLACK, 0.3f));
			}
			
			if (wireMode) {
				for (auto link : links) {
					link->draw();
				}
			}
			
			if (tearMode) {
				for (auto nodes : all_nodes) {
					for (auto node : nodes) {
						node->draw();
					}
				}

				// DrawTextureEx(texture, { 0.0, 0.0 }, 0, 1, WHITE);
			}
			// else if (!ripped) {
			else if (true) {
				
				rlSetTexture(texture.id);
				rlColor4ub(BLUE.r, BLUE.g, BLUE.b, 220);
				for (int i = 0; i < COLUMNS-1; i++) 
				{
					for (int j = 0; j < ROWS-1; j++) 
					{	
						Vec3 np1 = all_nodes[i][j]->position;
						Vec3 np2 = all_nodes[i][j+1]->position;
						Vec3 np3 = all_nodes[i+1][j+1]->position;

						rlBegin(RL_TRIANGLES);
						{
							rlColor4ub(GREEN.r, GREEN.g, GREEN.b, 220);
							rlVertex3f(np1.x, np1.y, np1.z);
							rlColor4ub(RED.r, RED.g, RED.b, 220);
							rlVertex3f(np2.x, np2.y, np2.z);
							rlColor4ub(BLUE.r, BLUE.g, BLUE.b, 220);
							rlVertex3f(np3.x, np3.y, np3.z);
						}
						rlEnd();

						rlBegin(RL_TRIANGLES);
						{
							rlColor4ub(BLUE.r, BLUE.g, BLUE.b, 220);
							rlTexCoord2f(0, 0);
							rlVertex3f(np1.x, np1.y, np1.z);
							rlColor4ub(RED.r, RED.g, RED.b, 220);
							rlTexCoord2f(0, 100);
							rlVertex3f(np3.x, np3.y, np3.z);
							rlColor4ub(GREEN.r, GREEN.g, GREEN.b, 220);
							rlTexCoord2f(100, 100);
							rlVertex3f(np2.x, np2.y, np2.z);
						}
						rlEnd();

						Vec3 np4 = all_nodes[i+1][j]->position;
						rlBegin(RL_TRIANGLES);
						{
							rlColor4ub(GREEN.r, GREEN.g, GREEN.b, 220);
							rlVertex3f(np3.x, np3.y, np3.z);
							rlColor4ub(RED.r, RED.g, RED.b, 220);
							rlVertex3f(np4.x, np4.y, np4.z);
							rlColor4ub(BLUE.r, BLUE.g, BLUE.b, 220);
							rlVertex3f(np1.x, np1.y, np1.z);
						}
						rlEnd();
						rlBegin(RL_TRIANGLES);
						{
							rlColor4ub(BLUE.r, BLUE.g, BLUE.b, 220);
							rlVertex3f(np3.x, np3.y, np3.z);
							rlColor4ub(RED.r, RED.g, RED.b, 220);
							rlVertex3f(np1.x, np1.y, np1.z);
							rlColor4ub(GREEN.r, GREEN.g, GREEN.b, 220);
							rlVertex3f(np4.x, np4.y, np4.z);
						}
						rlEnd();
					}
				}


				rlSetTexture(0);
				// rlSetShader(rlGetShaderIdDefault(), rlGetShaderLocsDefault());
			}
			
			else {
				for (auto link : links) {
					link->draw();
				}
				for (auto nodes : all_nodes) {
					for (auto node : nodes) {
						node->draw();
					}
				}
			
			}
			DrawGrid(30, 1.0f);
		}
		EndMode3D();

		// DrawFPS(10, 10);
		if (tearMode) 
			DrawText("Tear [SPACE: Collision mode]", 10, 10, 20, DARKGRAY);
		else
			DrawText("Collision [SPACE: Tear mode]", 10, 10, 20, DARKGRAY);

		if (wireMode) 
			DrawText("Hide wires: ENTER", WIDTH-200, 10, 20, DARKGRAY);
		else
			DrawText("Show wires: ENTER", WIDTH-200, 10, 20, DARKGRAY);

		if (airLow) 
			DrawText("High air drag [Press L to toggle low air drag]", WIDTH-1000, 30, 20, DARKGRAY);
		else
			DrawText("Low air drag [Press L to toggle high air drag]", WIDTH-1000, 30, 20, DARKGRAY);

		// DrawText("Use keys [Y][R][G][B] to toggle lights", 10, 40, 20, DARKGRAY);
		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	// De-Initialization
	//--------------------------------------------------------------------------------------
	UnloadModel(model);     // Unload the model
	UnloadShader(shader);   // Unload shader
	UnloadTexture(texture);

	CloseWindow();          // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}


