#pragma once

#ifndef __SHARK_H__
#define __SHARK_H__

#include "cocos2d.h"
#include "Box2D.h"
#include "Category.h"
#include"GB2ShapeCache-x.h"

#define PTM_RATIO 32.f

USING_NS_CC;

class Shark : public CCSprite
{
public:
	Shark();
	~Shark();

	static Shark * create(b2World* world, GB2ShapeCache* shape, int level, CCPoint p_pos);
	void _init(b2World* world, GB2ShapeCache* shape, int leve, CCPoint p_posl);

	int _update(float dt, int level);

	b2Body *getBody() { return body; };
	CCSprite* getLine() { return line; }
	CCSprite* getWarning() { return warning; }
	int getCreateFlag() { return create_flag; }
	CCParticleSystem* getParticle() { return particle_; }

	void FireParticles(); // ��ں� ȿ��
	bool SetParticle( const std::string& fileName, float scale, float duration, const CCPoint& position );

private:
	b2Body* body;
	CCParticleSystem* particle_;

	CCSprite *line;
	CCSprite *warning;
	int create_flag; // ���� ������
	int UpDownSpeed; // ���Ʒ� ���ǵ�
	int sharkAngle; // ���� ���̵� �ö󰥼��� ������

	float appear_timer;
	float appear_time;
	int arrivalY; // ����~���� Y�� ����
};
#endif