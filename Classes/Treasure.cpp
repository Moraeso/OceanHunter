#include "Treasure.h"
#include <time.h>

Treasure::Treasure()
{
}

Treasure::~Treasure()
{
}

Treasure* Treasure::create(b2World* world, GB2ShapeCache* shape)
{
	Treasure * node = new Treasure();
	node->_init(world, shape);
	node->autorelease();

	return node;
}

void Treasure::_init(b2World* world, GB2ShapeCache* shape)
{
	srand((unsigned)time(NULL));

	winSize = CCDirector::sharedDirector()->getWinSize();

	char filename[100] = "0";
   	sprintf(filename, "Item/1.png");

	CCSprite::initWithFile(filename);

	CCAnimation* animation = CCAnimation::create();
	animation->setDelayPerUnit(0.1);
	animation->setLoops(99999);

	for(int i=1; i<=13; i++)
	{
		char str[100] = "0";
		sprintf(str,"Item/%d.png",i);
		animation->addSpriteFrameWithFileName(str);
	}
	CCAnimate *animate = CCAnimate::create(animation);

	this->runAction(animate);

	pos.x = rand()%(int)winSize.width;
	pos.y = winSize.height - 100;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 물리 현상 적용, b2_staticBody : 물리 현상 무시
	bodyDef.position.Set( pos.x/PTM_RATIO, pos.y/PTM_RATIO );
	bodyDef.angle = 0; // 각도
	//bodyDef.bullet = true;

 	body = world->CreateBody(&bodyDef); // 영혼에 몸을 넣는 느낌

	CCSize size = this->boundingBox().size;

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);
	
	shape->sharedGB2ShapeCache()->addFixturesToBody(body, "2");

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f; // 밀도
	fixtureDef.friction = 0.3f; // 마찰력

	fixtureDef.filter.categoryBits = TREASURE;
	fixtureDef.filter.maskBits = HARPOON;

	body->CreateFixture(&fixtureDef);

	body->SetAngularDamping(100000);

	body->SetLinearVelocity( b2Vec2(0, -2.5) );

	this->setPosition( ccp( body->GetPosition().x*PTM_RATIO,
		body->GetPosition().y*PTM_RATIO ) );

	light = CCSprite::create("Item/light.png");
	light->setPosition(this->getPosition());

	CCRotateTo* lotation = CCRotateTo::create(30,3600);
	light->runAction(lotation);

	dead_timer = 0;
	dead_flag = 0;
}

int Treasure::_update(float dt)
{
	if(dead_flag == 0)
	{
		this->setPosition( ccp( body->GetPosition().x*PTM_RATIO,
			body->GetPosition().y*PTM_RATIO ) );

		light->setPosition(this->getPosition());
	}

	if(dead_flag == 1)
	{
		dead_timer += dt;
		if(dead_timer >= 1)
			dead_flag = 2;

		return 1;
	}

	if(dead_flag == 2)
	{
		return 2;
	}

	return 0;
}

bool Treasure::SetParticle( const std::string& fileName, float scale, float duration, const CCPoint& position )
{
    particle_->setTexture( CCTextureCache::sharedTextureCache()->addImage( fileName.c_str() ) );
 
    if( particle_ )
    {
        particle_->setScale( scale );
 
        // 파티클의 지속시간 조정 : -1 일경우 FOREVER
        particle_->setDuration( duration );
 
        particle_->setPosition( position );

		particle_->autorelease();
 
        return true;
    }
 
    return false;
}
void Treasure::FireParticles()
{
	particle_ = CCParticleFire::create();
    particle_->retain();
	
    SetParticle("Enemy/3/bubble.png", 1.0f, 0.1f, this->getPosition());
}