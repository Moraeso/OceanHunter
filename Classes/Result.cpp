#include "Result.h"
#include "SimpleAudioEngine.h"

extern int score;
extern int level;

CCScene* Result::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		Result *layer = Result::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

Result::Result()
{
}

Result::~Result()
{
}

bool Result::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(! CCLayer::init());
		
		winSize = CCDirector::sharedDirector()->getWinSize();
		this->setTouchEnabled(true);
		this->setKeypadEnabled(true);

		this->scheduleUpdate();

		CocosDenshion::SimpleAudioEngine *m_Sae = NULL;

		m_Sae->preloadBackgroundMusic("Sound/ending_bgm.mp3");

		m_Sae->playBackgroundMusic("Sound/ending_bgm.mp3", true);

		back = CCSprite::create("Result/back_ending.png");
		back->setPosition( ccp(winSize.width/2, winSize.height/2) );
		this->addChild(back, 1);

		float scoreX = 0;
		if(score > 0) scoreX += 40;
		if(score > 10) scoreX += 40;
		if(score > 100) scoreX += 40;
		if(score > 1000) scoreX += 40;
		if(score > 10000) scoreX += 40;

		CCLabelAtlas* scoreLabel = CCLabelAtlas::create("0123456789", "Font/font.png", 40, 40, '0');
		scoreLabel->setPosition( ccp(winSize.width/2 - scoreX, winSize.height-100) );
		scoreLabel->setString(CCString::createWithFormat("%d", score)->m_sString.c_str());
		this->addChild(scoreLabel, 5);
		scoreLabel->setScale(2);
		scoreLabel->setColor( ccc3(90,90,90) );

		float levelX = -40;
		if(level > 0) levelX += 40;
		if(level > 10) levelX += 40;
		if(level > 100) levelX += 40;
		if(level > 1000) levelX += 40;
		if(level > 10000) levelX += 40;

		CCLabelAtlas* levelLabel = CCLabelAtlas::create("0123456789", "Font/font.png", 40, 40, '0');
		levelLabel->setPosition( ccp(winSize.width/2 - levelX, winSize.height-200) );
		levelLabel->setString(CCString::createWithFormat("%d", level)->m_sString.c_str());
		this->addChild(levelLabel, 5);
		levelLabel->setScale(2);
		levelLabel->setColor( ccc3(90,90,90) );

		CCSprite* level_pan = CCSprite::create("GameObj/level.png");
		level_pan->setAnchorPoint(ccp(0,0));
		level_pan->setPosition(ccp(winSize.width/2-levelX-80, -130-40));
		this->addChild(level_pan, 5);
		//level_pan->setScale(2);
		level_pan->setColor( ccc3(90,90,90) );
		
		bRet = true;
	
	} while (0);

	return bRet;
}

void Result::update(float dt)
{
	if(GetKeyState(VK_SPACE) & KF_UP)
	{
		CCScene *pScene = CCTransitionFade::create(1, Title::scene());

		CCDirector::sharedDirector()->replaceScene(pScene);
	}
}