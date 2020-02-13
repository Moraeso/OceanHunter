#pragma once

#ifndef __RESULT_H__
#define __RESULT_H__

#include "cocos2d.h"
#include "Title.h"
#include <Windows.h>

USING_NS_CC;

class Result : public CCLayer
{
public:
	Result();
	~Result();

	static CCScene* scene();
	virtual bool init();

	void update(float dt);

	CREATE_FUNC(Result);

private:
	CCSize winSize;
	
	CCSprite* back;
};
#endif