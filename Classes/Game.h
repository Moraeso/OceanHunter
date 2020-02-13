#pragma once

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "SimpleAudioEngine.h"
#include <Windows.h>
#include <list>
#include "Enemy.h"
#include "Harpoon.h"
#include "Player.h"
#include "Shark.h"
#include "Treasure.h"
#include "Category.h"
#include "GLES-Render.h"
#include "Result.h"
#include"GB2ShapeCache-x.h"

#define PTM_RATIO 32.f

USING_NS_CC;

class Game : public cocos2d::CCLayer, public b2ContactListener
{
public:
	Game();
	~Game();

	virtual bool init();  
	GLESDebugDraw* m_debugDraw;
	CocosDenshion::SimpleAudioEngine* m_Sae;
	static cocos2d::CCScene* scene();
	
	void update(float dt);
	void levelUp();

	virtual void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, const b2ContactImpulse* inpulse);
	
	//virtual void keyClicked(char);
	void draw();

	void updateLabel();
	void font();

	b2World* world;

    CREATE_FUNC(Game);

private:
	CCSize winSize;
	
	CCSprite *back;

	CCSprite* levelUpLogo;
	CCLabelAtlas* levelLabel;
	CCSprite* level_pan;

	// 플레이어
	Player* player;
	GB2ShapeCache* player_shape;

	float harpoon_timer;
	
	list<Enemy*> *ENEMY_LIST;
	float enemy_timer;
	bool basic_enemy;
	int enemyNumber;
	int enemyLevel;
	float level_timer;
	GB2ShapeCache* enemy1_shape;
	GB2ShapeCache* enemy2_shape;

	list<Harpoon*> *HARPOON_LIST;
	bool harpoon_fire;

	list<Shark*> *SHARK_LIST;
	GB2ShapeCache* shark_shape;
	float shark_timer;
	float shark_time;

	list<Treasure*> *TREASURE_LIST;
	GB2ShapeCache* item_shape;
	float treasure_timer;
	float treasure_time;

	CCLabelAtlas* scoreLabel;
	//int score;
};

#endif  // __GAME_SCENE_H__