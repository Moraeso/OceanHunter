#include "Shark.h"
#include <time.h>

#define DEGREE 57.29577

Shark::Shark()
{
}

Shark::~Shark()
{
}

Shark* Shark::create(b2World* world, GB2ShapeCache* shape, int level, CCPoint p_pos)
{
	Shark * node = new Shark();
	node->_init(world, shape, level, p_pos);
	node->autorelease();

	return node;	
}

void Shark::_init(b2World* world, GB2ShapeCache* shape, int level, CCPoint p_pos)
{
	srand((unsigned)time(NULL));

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	char filename[100] = "0";
	sprintf(filename, "Enemy/3/1.png");

	CCSprite::initWithFile(filename);

	CCAnimation* animation = CCAnimation::create();
	animation->setDelayPerUnit(0.1);
	animation->setLoops(99999);

	for(int i=1; i<=3; i++)
	{
		char str[100] = "0";
		sprintf(str,"Enemy/3/%d.png",i);
		animation->addSpriteFrameWithFileName(str);
	}
	CCAnimate *animate = CCAnimate::create(animation);

	this->runAction(animate);

	sharkAngle = 400;

	create_flag = rand()%2 + 1;

	float sharkY_min;
	float sharkY_max;

	float s = -300;
	while(1)
	{
		if(20 > atan((p_pos.y-s)/p_pos.x) * DEGREE)
		{
			sharkY_min = s;
			break;
		}
		s++;
	}
	while(1)
	{
		if(-20 > atan((p_pos.y-s)/p_pos.x) * DEGREE)
		{
			sharkY_max = s;
			break;
		}
		s++;
	}
	
	int upDown = rand()%(int)(sharkY_max-sharkY_min) + sharkY_min;

	float angle = atan((p_pos.y-upDown)/p_pos.x) * DEGREE;
	
	float sharkY_end;
	float ss = -300;
	while(1)
	{
		if(-angle > atan((p_pos.y-ss)/p_pos.x) * DEGREE)
		{
			sharkY_end = ss;
			break;
		}
		ss++;
	}

	int upDown2 = sharkY_end;

	CCPoint shark_pos;
	if(create_flag == 1)
	{
		shark_pos = ccp(-125,upDown);
		this->setFlipX(true);
	}
	else if(create_flag == 2)
		shark_pos = ccp(winSize.width+125,upDown2);

	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 물리 현상 적용, b2_staticBody : 물리 현상 무시
	bodyDef.position.Set( shark_pos.x/PTM_RATIO, shark_pos.y/PTM_RATIO );
	bodyDef.angle = 0; // 각도
	
	body = world->CreateBody(&bodyDef); // 영혼에 몸을 넣는 느낌

	CCSize size = this->boundingBox().size;

	// Define another box shape for our dynamic body.
	//b2PolygonShape dynamicBox;
	//dynamicBox.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);//These are mid points for our 1m box

	b2CircleShape dynamicCircle;
	//dynamicCircle.m_p.Set(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);
	dynamicCircle.m_radius = size.width/2/PTM_RATIO;

	shape->sharedGB2ShapeCache()->addFixturesToBody(body, "1");

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicCircle;
	fixtureDef.density = 1.0f; // 밀도
	fixtureDef.friction = 0.3f; // 마찰력

	fixtureDef.filter.categoryBits = SHARK;
	fixtureDef.filter.maskBits = PLAYER;

	body->SetAngularDamping(100000);
	body->SetGravityScale(0);

  	arrivalY = upDown - upDown2;
   	line = CCSprite::create("Enemy/3/sharkLine.png");
	warning = CCSprite::create("Enemy/3/warning.png");
	line->setRotation(atan(((float)arrivalY)/winSize.width) * DEGREE);
	warning->setRotation(atan(((float)arrivalY)/winSize.width) * DEGREE);
	line->setAnchorPoint(ccp(0,0));
	warning->setAnchorPoint(ccp(0,0));
	line->setPosition(ccp(0,upDown));

	if(create_flag == 1) {
		warning->setPosition( ccp(50, upDown + 30) );
		warning->setFlipX(true);
	}
	else if(create_flag == 2)
		warning->setPosition( ccp(winSize.width-150, upDown2 + 30) );
	
	appear_timer = 0;
	//body->SetLinearDamping(5);
	//body_player->( b2Vec2(5,5) );
}

int Shark::_update(float dt, int level)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	appear_timer += dt;

	appear_time = 1 - ((float)level/25.0f);
	if(appear_time < 0.3) appear_time = 0.3;
	if(appear_timer >= appear_time)
	{
		appear_timer = -1000;

		if(create_flag == 1)
		{
			body->SetLinearVelocity( b2Vec2(50,-(arrivalY/20)) );
		}
		else
		{
			body->SetLinearVelocity( b2Vec2(-50,arrivalY/20) );
			line->setFlipX(true);
		}

		return 2;
	}

	this->setPosition( ccp( body->GetPosition().x*PTM_RATIO,
		body->GetPosition().y*PTM_RATIO ) );

	if(appear_timer < 0)
		particle_->setPosition(this->getPosition());

	if(this->getPositionX() < -250 && create_flag == 2 ||
		this->getPositionX() > winSize.width+250 && create_flag == 1)
	{
		return 1;
	}
	return 0;
}

bool Shark::SetParticle( const std::string& fileName, float scale, float duration, const CCPoint& position )
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
void Shark::FireParticles()
{
	particle_ = CCParticleFire::create();
    particle_->retain();
	
    SetParticle("Enemy/3/bubble.png", 1.0f, 5.0f, ccp( 0, 0 ) );
}