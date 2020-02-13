#pragma once

#ifndef __ENEMY_H__
#define __ENEMY_H__

//#include "GameScene.h" // 여기서 오류 추가 O or X
#include "cocos2d.h"
#include "Box2D.h"
#include "Category.h"
#include"GB2ShapeCache-x.h"

#define PTM_RATIO 32.f

USING_NS_CC;

class Enemy : public CCSprite
{
public:
	Enemy();
	~Enemy();

	static Enemy * create(b2World* world, const int kind, GB2ShapeCache* shape1, GB2ShapeCache* shape2, int level);
	void _init(b2World* world, const int kind, GB2ShapeCache* shape1, GB2ShapeCache* shape2, int level);

	int _update(float dt, CCPoint player_pos, bool speedUp);
	void move();

	CCPoint getPos() { return pos; }
	b2Body *getBody() { return body; };

	float getMoveTimer() { return move_timer; }
	int getMoveTimeRand() { return moveTimeRand; }
	int* getSpeedRand() { return speedRand; }
	//bool getMoveTransform() { return moveTransform; }
	int getDeadFlag() { return dead_flag; }
	float getDeadTimer() { return dead_timer; }
	int getKind() { return kind; }

	void setMoveTimeRand();
	//void setMoveTransform(bool logic) { moveTransform = logic; }
	void setSpeedRand();
	void setDeadFlag(int pInt) { dead_flag = pInt; }
	void doDeadTimer(float dt) { dead_timer += dt; }

	void setMoveTimer(float time) { move_timer = time; }
	
	void setScoreLabel(int score);

private:
	CCSize winSize;

	b2Body* body;
	CCPoint pos;

	int kind;
	float speed;

	int creat_location; // UP:1, DOWN:2, LEFT:3, RIGHT:4
	
	float moveTimeRand; // 이동이 일어나는 간격
	float move_timer;

	int speedRand[4]; // 스피드
	int dead_flag;
	float dead_timer;

	CCLabelAtlas* scoreLabel;

	//bool moveTransform;
};
#endif