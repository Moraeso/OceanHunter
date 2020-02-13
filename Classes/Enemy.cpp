#include "Enemy.h"
#include <time.h>
#include"GB2ShapeCache-x.h"
#define StandardSpeed 30000

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

Enemy* Enemy::create(b2World* world, const int kind, GB2ShapeCache* shape1, GB2ShapeCache* shape2, int level)
{
	Enemy * node = new Enemy();
	node->_init(world, kind, shape1, shape2, level);
	node->autorelease();

	return node;	
}

void Enemy::_init(b2World* world, const int kind, GB2ShapeCache* shape1, GB2ShapeCache* shape2, int level)
{
	srand((unsigned)time(NULL));

	winSize = CCDirector::sharedDirector()->getWinSize();

	this->kind = kind;

	char filename[100] = "0";
	if(kind == 2) {  
		sprintf(filename, "Enemy/2/1.png");
	}
	else {
		sprintf(filename, "Enemy/1/1.png");
	}

	CCSprite::initWithFile(filename);

	CCAnimation* animation = CCAnimation::create();
	animation->setDelayPerUnit(0.1);
	animation->setLoops(99999);

	for(int i=1; i<=2; i++)
	{
		char str[100] = "0";
		sprintf(str,"Enemy/%d/%d.png",kind,i);
		animation->addSpriteFrameWithFileName(str);
	}
	CCAnimate *animate = CCAnimate::create(animation);

	this->runAction(animate);

	speed = StandardSpeed;
	int temp_level = 1;
	while(1)
	{
		if(temp_level >= level) break;
		speed = speed * 11.0f/10.0f;
		temp_level++;
	}

	if(level == 10)
	{
		int a = 1;
	}

	moveTimeRand = 0;
	move_timer = 0;

	dead_flag = 0;
	dead_timer = 0;
		
	while(1) { // 초기 좌표 정해주기
		pos.x = rand()%((int)winSize.width+200) - 100;
		pos.y = rand()%((int)winSize.height+200) - 100;
		
		if(! (pos.x > -this->getContentSize().width/2
			&& pos.x < winSize.width + this->getContentSize().width/2
			&& pos.y > -this->getContentSize().height/2
			&& pos.y < winSize.height + this->getContentSize().height/2) )
		{
			if(pos.x > -this->getContentSize().width/2)
				creat_location = 1;
			
			else if(pos.x < winSize.width + this->getContentSize().width/2) 
				creat_location = 2;
			
			else if(pos.y > -this->getContentSize().height/2)
				creat_location = 3;
			
			else if(pos.y < winSize.height + this->getContentSize().height/2)
				creat_location = 4;

			break;
		}
	}

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 물리 현상 적용, b2_staticBody : 물리 현상 무시
	bodyDef.position.Set( this->pos.x/PTM_RATIO, this->pos.y/PTM_RATIO );
	bodyDef.angle = 0; // 각도
	
	body = world->CreateBody(&bodyDef); // 영혼에 몸을 넣는 느낌

	CCSize size = this->boundingBox().size;

	// Define another box shape for our dynamic body.
	//b2PolygonShape dynamicBox;
	//dynamicBox.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);//These are mid points for our 1m box

	b2CircleShape dynamicCircle;
	//dynamicCircle.m_p.Set(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);
	dynamicCircle.m_radius = size.width/2/PTM_RATIO;

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicCircle;
	fixtureDef.density = 1.0f; // 밀도
	fixtureDef.friction = 0.3f; // 마찰력

	fixtureDef.filter.categoryBits = ENEMY;
	fixtureDef.filter.maskBits = BOUNDARY2 | HARPOON;

	body->CreateFixture(&fixtureDef);

	//if(kind == 1)
		//shape1->sharedGB2ShapeCache()->addFixturesToBody(body, "1");
	//else if(kind == 2)
		//shape2->sharedGB2ShapeCache()->addFixturesToBody(body, "1");

	//body->SetLinearDamping(5);
	//body_player->SetLinearVelocity( b2Vec2(5,5) );

	this->setPosition( ccp( body->GetPosition().x*PTM_RATIO,
		body->GetPosition().y*PTM_RATIO ) );

	scoreLabel = CCLabelAtlas::create("0123456789", "Font/font.png", 40, 40, '0');
	scoreLabel->setAnchorPoint( ccp(0,0) );
	scoreLabel->setPosition( ccp(-10,60) );
	this->addChild(scoreLabel);
	scoreLabel->setVisible(false);
	scoreLabel->setScale(0.7);
}

void Enemy::setScoreLabel(int score)
{
	scoreLabel->setString(CCString::createWithFormat("%d", score)->m_sString.c_str());
	scoreLabel->setVisible(true);
}

int Enemy::_update(float dt, CCPoint player_pos, bool speedUp)
{
	move_timer += dt;

	if(move_timer >= moveTimeRand && dead_flag == 0)
	{
		body->SetLinearVelocity( b2Vec2(0,0) );

		setSpeedRand();

		body->ApplyForceToCenter( b2Vec2(0, speedRand[0]/PTM_RATIO/2) ); // UP
		body->ApplyForceToCenter( b2Vec2(0, -speedRand[1]/PTM_RATIO/2) ); // DOWN
		body->ApplyForceToCenter( b2Vec2(-speedRand[2]/PTM_RATIO, 0) ); // LEFT
		body->ApplyForceToCenter( b2Vec2(speedRand[3]/PTM_RATIO, 0) ); // RIGHT

		if(speedRand[2] > speedRand[3])
			this->setFlipX(false);
		else
			this->setFlipX(true);		

		body->SetGravityScale(0);

		setMoveTimeRand(); // 이동이 일어나는 간격 Rand
		move_timer = 0;
	}

	if(dead_flag == 1)
	{
		dead_timer += dt;

		this->setColor( ccc3(100,100,100) );
		this->setFlipY(true);
		if(dead_timer >= 1)
			return 1;
	}
	
	this->setPosition( ccp( body->GetPosition().x*PTM_RATIO,
		body->GetPosition().y*PTM_RATIO ) );
	
	return 0;
}

void Enemy::setMoveTimeRand()
{
	moveTimeRand = rand()%4 + 2;
}

void Enemy::setSpeedRand()
{
	int speedMulty = 1;
	if(kind == 2) speedMulty = 3;
	while(1)
	{
		for(int i=0; i<4; i++)
		{
			speedRand[i] = ((rand()%(int)speed)  + ((int)speed - StandardSpeed) + 20000) * speedMulty;
		}

		if(creat_location == 1) {
			if(speedRand[0] > speedRand[1])
			{
				creat_location = 0;
				break;
			}
		}
		else if(creat_location == 2)
		{
			if(speedRand[0] < speedRand[1])
			{
				creat_location = 0;
				break;
			}
		}
		else if(creat_location == 3)
		{
			if(speedRand[2] < speedRand[3])
			{
				creat_location = 0;
				break;
			}
		}
		else if(creat_location == 4)
		{
			if(speedRand[2] > speedRand[3])
			{
				creat_location = 0;
				break;
			}
		}
		else
			break;
	}
}