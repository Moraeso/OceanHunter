#include "Player.h"
#include"GB2ShapeCache-x.h"

USING_NS_CC;

Player::Player()
{
}

Player::~Player()
{
}

Player* Player::create(b2World* world, const char* filename, GB2ShapeCache* shape)
{
	Player * node = new Player();
	node->_init(world, filename, shape);
	node->autorelease();

	return node;
}

void Player::_init(b2World* world, const char* filename, GB2ShapeCache* shape)
{
	CCSprite::initWithFile(filename);

	winSize = CCDirector::sharedDirector()->getWinSize();

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 물리 현상 적용, b2_staticBody : 물리 현상 무시
	bodyDef.position.Set( winSize.width/2/PTM_RATIO, winSize.height/2/PTM_RATIO);
	bodyDef.angle = 0; // 각도

	body = world->CreateBody(&bodyDef); // 영혼에 몸을 넣는 느낌

	CCSize size = this->boundingBox().size;

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);//These are mid points for our 1m box

	//b2CircleShape dynamicCircle;
	//dynamicCircle.m_p.Set(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);
	//dynamicCircle.m_radius = size.width/2/PTM_RATIO;

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f; // 밀도
	fixtureDef.friction = 0.0f; // 마찰력

	fixtureDef.filter.categoryBits = PLAYER;
	fixtureDef.filter.maskBits = BOUNDARY | SHARK;

	body->CreateFixture(&fixtureDef);

	body->SetGravityScale(0);
	body->SetLinearDamping(5);
	//body_player->SetLinearVelocity( b2Vec2(5,5) );

	shape->sharedGB2ShapeCache()->addFixturesToBody(body, "1");
	

	this->setPosition( ccp(body->GetPosition().x*PTM_RATIO, 
		body->GetPosition().y*PTM_RATIO) );

	//this->setScale(1.5);
		
	// 애니메이션
	CCAnimation* animation = CCAnimation::create();
	animation->setDelayPerUnit(0.1);
	animation->setLoops(99999);

	for(int i=1; i<=4; i++)
	{
		char str[100] = "0";
		sprintf(str,"Character/Jack/Normal/%d.png",i);
		animation->addSpriteFrameWithFileName(str);
	}
	CCAnimate *animate = CCAnimate::create(animation);

	this->runAction(animate);
	 
	direction = 2;
}

void Player::update(float dt)
{
	move();

	this->setPosition( ccp(body->GetPosition().x*PTM_RATIO, 
		body->GetPosition().y*PTM_RATIO) );
}

void Player::move()
{
	if(GetKeyState(VK_UP) & 0x8000)
	{
		body->ApplyForceToCenter( b2Vec2(0, SPEED/PTM_RATIO) );
	}

	if(GetKeyState(VK_DOWN) & 0x8000)
	{
		body->ApplyForceToCenter( b2Vec2(0, -SPEED/PTM_RATIO) );
	}

	if(GetKeyState(VK_LEFT) & 0x8000)
	{
		body->ApplyForceToCenter( b2Vec2(-SPEED/PTM_RATIO, 0) );
		this->setFlipX(true);
		direction = 1;
	}

	if(GetKeyState(VK_RIGHT) & 0x8000)
	{
		body->ApplyForceToCenter( b2Vec2(SPEED/PTM_RATIO, 0) );
		this->setFlipX(false);
		direction = 2;
	}
}
