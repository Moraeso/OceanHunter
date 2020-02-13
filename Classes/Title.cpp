#include "Title.h"
#include "Game.h"
#include "SimpleAudioEngine.h"

CCScene* Title::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		Title *layer = Title::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

Title::Title()
{
}

Title::~Title()
{
}

bool Title::init()
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

		m_Sae->preloadBackgroundMusic("Sound/title_bgm.mp3");

		m_Sae->playBackgroundMusic("Sound/title_bgm.mp3", true);

		back = CCSprite::create("Title/back.png");
		back->setPosition( ccp(winSize.width/2, winSize.height/2) );
		this->addChild(back, 1);

		logo = CCSprite::create("Title/logo.png");
		logo->setPosition( ccp(winSize.width/2 - 100, winSize.height/2) );
		this->addChild(logo, 1);

		clickKey = CCSprite::create("Title/clickKey.png");
		clickKey->setPosition( ccp(winSize.width/2, winSize.height/2) );
		this->addChild(clickKey, 1);
		
		bRet = true;
	
	} while (0);

	return bRet;
}

void Title::update(float dt)
{
	if(GetKeyState(VK_SPACE) & KF_UP)
	{
		CCScene *pScene = CCTransitionFadeBL::create(1, Game::scene());

		CCDirector::sharedDirector()->replaceScene(pScene);
	}
}
