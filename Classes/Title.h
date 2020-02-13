#pragma once

#ifndef __TITLE_H__
#define __TITLE_H__

#include "cocos2d.h"
#include <Windows.h>

USING_NS_CC;

class Title : public CCLayer
{
public:
	Title();
	~Title();

	static CCScene* scene();
	virtual bool init();

	void update(float dt);

	CREATE_FUNC(Title);

private:
	CCSize winSize;

	CCSprite* back;
	CCSprite* logo;
	CCSprite* clickKey;

};
#endif