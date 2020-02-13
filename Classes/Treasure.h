#pragma once

#ifndef __TREASURE_H__
#define __TREASURE_H__

#include "cocos2d.h"
#include "Box2D.h"
#include "Category.h"
#include"GB2ShapeCache-x.h"

#define PTM_RATIO 32.f

USING_NS_CC;

class Treasure : public CCSprite
{
public:
	Treasure();
	~Treasure();

	static Treasure * create(b2World* world, GB2ShapeCache* shape);
	void _init(b2World* world, GB2ShapeCache* shape);
	int _update(float dt);

	b2Body* getBody() { return body; }
	void setDeadFlag(int flag) { dead_flag = flag; }
	int getDeadFlag() { return dead_flag; }
	CCSprite* getLight() { return light; }
	CCParticleSystem* getParticle() { return particle_; }

	void FireParticles(); // ¸ð´ÚºÒ È¿°ú
	bool SetParticle( const std::string& fileName, float scale, float duration, const CCPoint& position );

private:
	CCSize winSize;
	CCParticleSystem* particle_;
	CCSprite* light;

	b2Body* body;
	CCPoint pos;

	int dead_flag;
	float dead_timer;
};
#endif