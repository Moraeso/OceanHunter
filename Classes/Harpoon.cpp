#include "Harpoon.h"

Harpoon::Harpoon()
{
}

Harpoon::~Harpoon()
{
}

Harpoon* Harpoon::create(b2World* world, const int itemNumber, CCPoint p_pos, int p_look)
{
	Harpoon * node = new Harpoon();
	node->_init(world, itemNumber, p_pos, p_look);
	node->autorelease();

	return node;
}

void Harpoon::_init(b2World* world, const int itemNumber, CCPoint p_pos, int p_look)
{
	winSize = CCDirector::sharedDirector()->getWinSize();

	char filename[100] = "0";
	if(itemNumber == 1)
	{
   		sprintf(filename, "Character/Jack/Normal/harpoon.png");
	}

	CCSprite::initWithFile(filename);

	if(p_look == 1) { this->setFlipX(true); }

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 물리 현상 적용, b2_staticBody : 물리 현상 무시
	bodyDef.position.Set( p_pos.x/PTM_RATIO, p_pos.y/PTM_RATIO );
	bodyDef.angle = 0; // 각도
	//bodyDef.bullet = true;

	body = world->CreateBody(&bodyDef); // 영혼에 몸을 넣는 느낌

	CCSize size = this->boundingBox().size;

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f; // 밀도
	fixtureDef.friction = 0.3f; // 마찰력

	fixtureDef.filter.categoryBits = HARPOON;
	fixtureDef.filter.maskBits = ENEMY | SHARK | TREASURE;

	body->CreateFixture(&fixtureDef);

	body->SetAngularDamping(100000);

	if(p_look == 1) {
		body->SetLinearVelocity( b2Vec2(-30,0) );
		direction = 1;
	}
	else if(p_look == 2) {
		body->SetLinearVelocity( b2Vec2(30,0) );
		direction = 2;
	}

	this->setPosition( ccp( body->GetPosition().x*PTM_RATIO,
		body->GetPosition().y*PTM_RATIO ) );

	dead_flag = 0;
	dead_timer = 0;
	fly_timer = 0;
	power = true;
}

int Harpoon::_update(float dt)
{
	if(dead_flag == 0)
	{
		this->setPosition( ccp( body->GetPosition().x*PTM_RATIO,
			body->GetPosition().y*PTM_RATIO ) );

		this->setRotation( CC_RADIANS_TO_DEGREES(body->GetAngle()) * -1);
		fly_timer += dt;
	}

	if(dead_flag == 1)
	{
 		dead_timer += dt;
		if(dead_timer >= 1)
			dead_flag = 2;		
	}

	if( dead_flag == 2
		|| this->getPositionX() < -this->getContentSize().width/2
		|| this->getPositionX() > winSize.width + this->getContentSize().width/2
		|| this->getPositionY() < -this->getContentSize().width/2
		|| this->getPositionY() > winSize.height + this->getContentSize().width/2 )
	{
		return 1;
	}

	return 0;
}