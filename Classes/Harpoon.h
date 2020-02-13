#pragma once

#ifndef __HARPOON_H__
#define __HARPOON_H__

#include "cocos2d.h"
#include "Box2D.h"
#include "Category.h"

#define PTM_RATIO 32.f

USING_NS_CC;

class Harpoon : public CCSprite
{
public:
	Harpoon();
	~Harpoon();

	static Harpoon * create(b2World* world, const int itemNumber, CCPoint p_pos, int p_look);
	void _init(b2World* world, const int itemNumber, CCPoint p_pos, int p_look);
	int _update(float dt);

	b2Body* getBody() { return body; }
	float getDeadTimer() { return dead_timer; }
	float getFlyTimer() { return fly_timer; }
	int getDirection() { return direction; }

	void setDeadFlag(int pInt) { dead_flag = pInt; } 
	int getDeadFlag() { return dead_flag; }

	bool getPower() { return power; }
	void setPower(bool value) { power = value; }
	
private:
	CCSize winSize;

	b2Body* body;
	CCPoint pos;
		
	int direction;
	int dead_flag;
	float dead_timer;
	float fly_timer;
	bool power;
};
#endif