//
//  Engine.cpp
//  HeroMustSavePrincess
//
//  Created by Daniel Breves on 7/11/12.
//  Copyright (c) 2012 Daniel Breves. All rights reserved.
//

#include "Engine.h"
#include "Animation.h"
#include "MenuState.h"
#include "GameMessageState.h"
#include "StateManager.h"
#include "ResourcePath.hpp"
#include <sstream>
#include <math.h>

Engine::Engine()
{
    current = 0;
}

Engine::Engine(int width, int height) {
    camera = Camera(sf::IntRect(0,0,width,height));
    
    CreateLevels();
}

Engine::~Engine()
{    
//    vector<Level*>::iterator i;
//    for ( i = levels.begin() ; i < levels.end(); i++ ) {
//        delete (*i);
//    }
}

void Engine::Init(StateManager* manager)
{
    sf::Vector2i videoSize = (sf::Vector2i)manager->GetWindow()->getSize();
    
    levels[current].LoadMap();
    
    int tileSize = levels[current].GetTileSize();
    
    camera.Move((levels[current].GetWidth() * tileSize - videoSize.x)/2,(levels[current].GetHeight() * tileSize - videoSize.y)/2);
    
    sf::Texture* hero = new sf::Texture;
    hero->loadFromFile(resourcePath() + "hero_full.png");
    player = new Player(*hero, levels[current].GetPlayerPosition(), 32, 32, 3);
}

void Engine::CreateLevels() {
    Level level0 = Level(resourcePath() + "tiled_test6.tmx");
    levels.push_back(level0);
    
    Level level1 = Level(resourcePath() + "level_momo.tmx");
    levels.push_back(level1);
}

void Engine::HandleEvents(StateManager* manager)
{
	sf::Event evt;

	while(manager->GetWindow()->pollEvent(evt))
	{
		if(evt.type == sf::Event::Closed) {
			manager->GetWindow()->close();
		}
        if(evt.type == sf::Event::KeyPressed && (evt.key.code == sf::Keyboard::Escape)) {
			manager->ChangeState(new MenuState);
		}
	}
    
    player->SetAction(Player::STAND);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        player->SetAction(Player::NORTH);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        player->SetAction(Player::SOUTH);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player->SetAction(Player::WEST);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player->SetAction(Player::EAST);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        player->SetAction(Player::ATTACK);
    }
}

void Engine::Update(StateManager* manager)
{
    if (!player->GetHealth() || levels[current].GetStatus() == Level::LOST) {
        sf::sleep(sf::seconds(1.2f));
        manager->ChangeState(new GameMessageState("Game Over", 60, new MenuState));
    }
    else if (levels[current].GetStatus() == Level::COMPLETE) {
        if (current == levels.size() - 1) {
            sf::sleep(sf::seconds(1.2f));
            manager->ChangeState(new GameMessageState("You won!", 60, new MenuState));
        } else {
            ++current;
            std::stringstream ss;
            ss << "Level " << current + 1;
            sf::sleep(sf::seconds(1.0f));
            manager->ChangeState(new GameMessageState(ss.str(), 60, this));
        }
    }
    else {
        player->Update(levels[current].GetSprites(), &levels[current]);
        camera.MoveCenter(&levels[current], player->GetPosition().x, player->GetPosition().y);
        levels[current].Update(&camera);
    }
}

void Engine::Render(StateManager* manager)
{
    levels[current].Draw(manager->GetWindow(), &camera);
    
    player->Draw(manager->GetWindow(), &camera);
}
