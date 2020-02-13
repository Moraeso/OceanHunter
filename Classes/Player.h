#pragma once

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include <Windows.h>
#include "Category.h"
#include"GB2ShapeCache-x.h"

#define PTM_RATIO 32.f
#define SPEED 20000

USING_NS_CC;

class Player : public cocos2d::CCSprite
{
public:
	Player();
	~Player();

	b2Body *getBody() { return body; }

	static Player* create(b2World* world, const char* filename, GB2ShapeCache* shape);
	void _init(b2World* world, const char* filename, GB2ShapeCache* shape);

	void update(float dt);
	
	void move();

	int getDirection() { return direction; }

private:
	CCSize winSize;

	b2Body* body;
	int direction;
};

#endif