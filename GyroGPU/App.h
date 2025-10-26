#pragma once
#include <memory>
#include <vector>

#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>

#include "Shapes/Polygon.h"
#include "Shapes/Circle.h"
#include "Components/CollisionComponent.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"
#include "Rendering/Texture/TextureManager.h"
#include "Systems/RenderSystem.h"
#include "GameObject.h"
#include "TickTimer.h"
#include "Window.h"
#include "Math/Gyrotile.h"
#include <iostream>
#include <list>
#include <ostream>
#include <cstdlib>
#include "Components/Textured/RegularTexturedPolygon.h"
#include "glm/ext/vector_float2.hpp"
#include "Systems/PhysicsSystem.h"
#include "Components/PositionComponent.h"
#include "Rendering/Camera.h"

class App {
	int width{ 1800 }, height{ 1800 };

	Window window;
	RenderSystem renderSystem;
	TextureManager textureManager;
	PhysicsSystem physicsSystem;


public:
	App() : window{ width, height }, renderSystem{ width, height } {
		int actual_width{ 0 }, actual_height{ 0 };
		window.GetDrawDimensions(&actual_width, &actual_height);
		renderSystem.Resize(actual_width, actual_height);
	}

	void Run() {
		bool running = true;
		TickTimer timer{ 60 }; // 60fps
		std::vector<std::unique_ptr<GameObject>> scene;
		MobiusTransformation gyroCamera;
		Camera worldCamera{ width, height };

		// start sandbox region
		scene.emplace_back(std::make_unique<GameObject>());
		scene.emplace_back(std::make_unique<GameObject>());

		auto ship = [&]() -> auto& { return scene[0]; };
		//scene[0]->addComponent(std::make_unique<SpriteRenderComponent>(0.4, 0.4, textureManager.get("Resources/ship.png")));
		ship()->addComponent<CollisionComponent>(collision::Polygon{{
			{-0.4, -0.4 },
			{0.4, -0.4},
			{0.4, 0.4},
			{-0.4, 0.4}
			} } );
		//auto sprite = [&]() -> auto& { return scene[1]; };
		//sprite()->addComponent<SpriteRenderComponent>(0.2, 0.2, textureManager.get("Resources/sprite.png"), 1.0f);
		//sprite()->addComponent<CollisionComponent>(collision::Polygon{{
		//	{-0.2, -0.2 },
		//	{0.2, -0.2},
		//	{0.2, 0.2},
		//	{-0.2, 0.2}
		//	} });
		//sprite()->addComponent<PositionComponent>(MobiusGyrovector{ 0.3, 0.0 });
		//sprite()->addComponent<VelocityComponent>(MobiusGyrovector{ -0.01, 0.0 });
		//sprite()->Transform({ MobiusGyrovector(0.3, 0) });

		GameObject bounding_box;
		bounding_box.addComponent<CollisionComponent>(collision::Circle{ MobiusGyrovector::Zero, 3 });
		
		Gyrogrid grid{ 4, 5, 2.0 };
		std::list<Gyrotile*> tiles = grid.getAll();
		std::cout << "tiles.size: " << tiles.size() << std::endl;
		for (Gyrotile* tile : tiles) {
			scene.emplace_back(std::make_unique<GameObject>());
			auto& obj = scene.back();
			double rand_x = (double)(std::rand() % 7) / 7.0;
			double rand_y = (double)((std::rand() % 6) + 1) / 7.0;
			std::vector<glm::vec2> uvs{
				{0.0       + rand_x, 0.0       + rand_y},
				{1.0 / 7.0 + rand_x, 0.0       + rand_y},
				{1.0 / 7.0 + rand_x, 1.0 / 7.0 + rand_y},
				{0.0       + rand_x, 1.0 / 7.0 + rand_y}
			};
			obj->addComponent<TexturedPolygon>(grid.getBasePoints(), uvs, textureManager.get("Resources/betrayal.png"), -1.0f);
			//obj->addComponent<RegularTexturedPolygon>(grid.getBasePoints(), textureManager.get("Resources/gradient.png"), -1.0f);
			obj->addComponent<PositionComponent>(tile->pos);
		}
		// end sandbox region

		//TODO: App should not know about SDL(?)
		SDL_Event windowEvent;
		while (running) {
			
			//just like pygame
			while (SDL_PollEvent(&windowEvent)) {
				switch (windowEvent.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_MOUSEWHEEL:
					worldCamera.zoom(windowEvent.wheel.y);
					break;
				case SDL_KEYDOWN:
					MobiusGyrovector translate = MobiusGyrovector::Zero;
					switch (windowEvent.key.keysym.sym) {
					case SDLK_UP:
						translate = MobiusGyrovector(0.0, -0.05);
						break;
					case SDLK_DOWN:
						translate = MobiusGyrovector(0.0, 0.05);
						break;
					case SDLK_LEFT:
						translate = MobiusGyrovector(0.05, 0.0);
						break;
					case SDLK_RIGHT:
						translate = MobiusGyrovector(-0.05, 0.0);
						break;
					case SDLK_w:
						worldCamera.move(0.0f, 0.05f, 0.0f);
						worldCamera.point(0.0f, 0.05f, 0.0f);
						break;
					case SDLK_a:
						worldCamera.move(-0.05f, 0.0f, 0.0f);
						worldCamera.point(-0.05f, 0.0f, 0.0f);
						break;
					case SDLK_s:
						worldCamera.move(0.0f, -0.05f, 0.0f);
						worldCamera.point(0.0f, -0.05f, 0.0f);
						break;
					case SDLK_d:
						worldCamera.move(0.05f, 0.0f, 0.0f);
						worldCamera.point(0.05f, 0.0f, 0.0f);
						break;
					}
					gyroCamera.Translate(translate);
					break;
				}
			}

			timer.update();
			while (timer.isTickReady()) {
				// update game logic

				physicsSystem.Update(scene);

				// start sandbox region
				//if (ship()->getComponent<COLLISION>()->Intersects(sprite()->getComponent<COLLISION>())) {
				//	std::cout << "YO";
				//}
				//if (!sprite()->getComponent<COLLISION>()->Intersects(bounding_box.getComponent<COLLISION>())) {
				//	static_cast<ProjectilePhysicsComponent*>(sprite()->getComponent<PHYSICS>())->SetVelocity(MobiusGyrovector::Zero);
				//}
				// end sandbox region

				// tell the timer that a game tick was executed
				timer.decrement();
			}

			renderSystem.RenderScene(scene, gyroCamera, worldCamera);
			window.SwapBuffers();

		}
	};


};