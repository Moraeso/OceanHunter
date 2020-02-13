#include "Game.h"
#include "SimpleAudioEngine.h"

int score;
int level;

CCScene* Game::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		Game *layer = Game::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

Game::Game()
{
}

Game::~Game()
{
}

// on "init" you need to initialize your instance
bool Game::init()
{
	bool bRet = false;
	do 
	{
 		CC_BREAK_IF(! CCLayer::init());

		winSize = CCDirector::sharedDirector()->getWinSize();
		this->setTouchEnabled(true);
		this->setKeypadEnabled(true);

		m_Sae = NULL;

		m_Sae->preloadBackgroundMusic("Sound/game_bgm.mp3");
		m_Sae->preloadEffect("Sound/harpoon_effect.wav");
		m_Sae->preloadEffect("Sound/dead_effect.wav");
		m_Sae->preloadEffect("Sound/level_up.wav");
		m_Sae->preloadEffect("Sound/alert.wav");
		m_Sae->preloadEffect("Sound/shark.wav");

		m_Sae->playBackgroundMusic("Sound/game_bgm.mp3", true);

		player_shape->sharedGB2ShapeCache()->addShapesWithFile("Character/Jack/Normal/jack.plist");
		enemy1_shape->sharedGB2ShapeCache()->addShapesWithFile("Enemy/1/enemy1.plist");
		enemy2_shape->sharedGB2ShapeCache()->addShapesWithFile("Enemy/2/enemy2.plist");
		shark_shape->sharedGB2ShapeCache()->addShapesWithFile("Enemy/3/shark.plist");
		item_shape->sharedGB2ShapeCache()->addShapesWithFile("Item/item.plist");

		this->scheduleUpdate();

		level_timer = 0;
		level = 1;

		// 배경
		back = CCSprite::create("GameObj/back.png");
		back->setPosition( ccp(winSize.width/2, winSize.height/2) );
		this->addChild(back);

		levelUpLogo = CCSprite::create("GameObj/level_up/1.png");
		levelUpLogo->setPosition(ccp(winSize.width/2, winSize.height/2));
		this->addChild(levelUpLogo,10);
		levelUpLogo->setOpacity(0);

		level_pan = CCSprite::create("GameObj/level.png");
		level_pan->setAnchorPoint(ccp(0,0));
		level_pan->setPosition(ccp(0,-40));
		this->addChild(level_pan);

		// world 초기화
		b2Vec2 gravity;
		gravity.Set(0.0f, -1.0f);
		world = new b2World(gravity);

		world->SetAllowSleeping(true);
		world->SetContinuousPhysics(true);
		world->SetContactListener(this);

		// 플레이어 init
		player = Player::create(world, "Character/Jack/Normal/1.png", player_shape);
		this->addChild(player);

		// 상하좌우 벽 edge
		b2BodyDef bodyDef[8];
		b2Body* body_wall[8];

		for(int i=0; i<8; i++)
		{
			bodyDef[i].type = b2_staticBody; //b2_staticBody는 안떨어짐 DynamicBody는 중력작용 
			bodyDef[i].angle = 0;

			body_wall[i] = world->CreateBody(&bodyDef[i]);

			b2EdgeShape edge;
			b2FixtureDef edgeFixture;

			CCSprite* sprite = CCSprite::create("Enemy/1/1.png");
			float length_width = sprite->getContentSize().width*2;
			float length_height = sprite->getContentSize().height*2;

			switch(i)
			{
			case 0:
				edge.Set(b2Vec2(0, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO));
				break;
			case 1:
				edge.Set(b2Vec2(0, 0), b2Vec2(winSize.width/PTM_RATIO, 0));
				break;
			case 2:
				edge.Set(b2Vec2(0, 0), b2Vec2(0, winSize.height/PTM_RATIO));
				break;
			case 3:
				edge.Set(b2Vec2(winSize.width/PTM_RATIO, 0), b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO));
				break;
			case 4:
				edge.Set(b2Vec2(-length_width/PTM_RATIO, (winSize.height+length_height)/PTM_RATIO),
					b2Vec2((winSize.width+length_width)/PTM_RATIO, (winSize.height+length_height)/PTM_RATIO));
				break;
			case 5:
				edge.Set(b2Vec2(-length_width/PTM_RATIO, -length_height/PTM_RATIO),
					b2Vec2((winSize.width+length_width)/PTM_RATIO, -length_height/PTM_RATIO));
				break;
			case 6:
				edge.Set(b2Vec2(-length_width/PTM_RATIO, -length_height/PTM_RATIO),
					b2Vec2(-length_width/PTM_RATIO, (winSize.height+length_height)/PTM_RATIO));
				break;
			case 7:
				edge.Set(b2Vec2((winSize.width+length_width)/PTM_RATIO, -length_height/PTM_RATIO),
					b2Vec2((winSize.width+length_width)/PTM_RATIO, (winSize.height+length_height)/PTM_RATIO));
				break;
			}

			if(i <= 3)
			{
				edgeFixture.filter.categoryBits = BOUNDARY;
				edgeFixture.filter.maskBits = PLAYER;
			}
			else
			{
				edgeFixture.filter.categoryBits = BOUNDARY2;
				edgeFixture.filter.maskBits = ENEMY;
			}

			edgeFixture.shape = &edge;
			body_wall[i]->CreateFixture(&edgeFixture);
		}

		///////
		m_debugDraw = new GLESDebugDraw( PTM_RATIO ); // PTM_RATIO
		world->SetDebugDraw(m_debugDraw);

		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;  // 이게 모양 그리는거
		flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_aabbBit;
		flags += b2Draw::e_pairBit; 
		flags += b2Draw::e_centerOfMassBit; // 이게아마 질량ㅇ인가 나도 다 안해봐서 모름
		m_debugDraw->SetFlags(flags);
		//////

		ENEMY_LIST = new list<Enemy*>();
		enemy_timer = 0;
		basic_enemy = true;
		enemyNumber = 0;
		enemyLevel = 20;

		HARPOON_LIST = new list<Harpoon*>();
		harpoon_fire = false;
		
		SHARK_LIST = new list<Shark*>();
		shark_time = rand()%(int)(100.0f/((float)level+19)) + 30.0f/((float)level+5);
		shark_timer = 0;

		TREASURE_LIST = new list<Treasure*>();
		treasure_time = 3;//rand()%10 + 10;
		treasure_timer = 0;

		score = 0;

		harpoon_timer = 0;

		scoreLabel = CCLabelAtlas::create("0123456789", "Font/font.png", 40, 40, '0');
		scoreLabel->setPosition( ccp(winSize.width-60, winSize.height-55) );
		scoreLabel->setString(CCString::createWithFormat("%d", score)->m_sString.c_str());
		this->addChild(scoreLabel, 5);

		levelLabel = CCLabelAtlas::create("0123456789", "Font/font.png", 40, 40, '0');
		levelLabel->setPosition( ccp(80, winSize.height-60) );
		levelLabel->setString(CCString::createWithFormat("%d", level)->m_sString.c_str());
		this->addChild(levelLabel, 5);

		bRet = true;

	} while (0);

	return bRet;
}

void Game::font()
{
	scoreLabel->setString(CCString::createWithFormat("%d", score)->m_sString.c_str());
	levelLabel->setString(CCString::createWithFormat("%d", level)->m_sString.c_str());
}

void Game::draw()
{
	CCLayer::draw();
	/*
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);  
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// Draw physics world debuging.
	glEnableClientState(GL_VERTEX_ARRAY);*/

	world->DrawDebugData();                   // 너의 월드

	/*glDisableClientState(GL_VERTEX_ARRAY);   // <-------- You need GL_VERTEX_ARRAY disabled          

	// Enable different states.          
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);  
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);*/
}

void Game::update(float dt)
{
	int velocityIterations = 8;
	int positionIterations = 1;

	world->Step(dt, velocityIterations, positionIterations);

	level_timer += dt;
	int speedUp = false;
	if(level_timer >= 8)
	{
		levelUp();
		speedUp = true;

		level_timer = 0;
	}

	shark_timer += dt;
	if(shark_timer > shark_time)
	{
		Shark* s = Shark::create(world, shark_shape, level, player->getPosition());
		this->addChild(s);
		this->addChild(s->getLine());
		this->addChild(s->getWarning());
		SHARK_LIST->push_front(s);

		m_Sae->playEffect("Sound/alert.wav");

		shark_time = rand()%(int)(100.0f/((float)level+19)) + 30.0f/((float)level+5);
		shark_timer = 0;
	}	
	
	for(list<Shark*>::iterator kt = SHARK_LIST->begin() ; kt != SHARK_LIST->end() ; kt++)
	{
		int updateInt = (*kt)->_update(dt, level);
		
		if(updateInt == 2)
		{
			(*kt)->FireParticles();
			this->addChild((*kt)->getParticle());
			this->removeChild((*kt)->getLine(),true);
			this->removeChild((*kt)->getWarning(),true);
			m_Sae->playEffect("Sound/shark.wav");
		}
		else if(updateInt == 1)
		{
			if(SHARK_LIST->empty())
				break;

			if((*kt) != NULL) {		
				world->DestroyBody((*kt)->getBody());
				this->removeChild((*kt),true);
				this->removeChild((*kt)->getParticle(),true);
				kt = SHARK_LIST->erase(kt);
			}

			break;
		}
	}

	/*
	treasure_timer += dt;
	if(treasure_timer > treasure_time)
	{
		Treasure* t = Treasure::create(world, item_shape);
		this->addChild(t);
		this->addChild(t->getLight());
		TREASURE_LIST->push_front(t);

		treasure_time = 2;//rand()%10 + 10;
		treasure_timer = 0;
	}

	for(list<Treasure*>::iterator lt = TREASURE_LIST->begin() ; lt != TREASURE_LIST->end() ; lt++)
	{
		int updateInt = (*lt)->_update(dt);

		if(updateInt == 1)
		{
			(*lt)->setVisible(false);
			(*lt)->getLight()->setVisible(false);
			(*lt)->FireParticles();
			this->addChild((*lt)->getParticle());
		}

		if(updateInt == 2)
		{
			this->removeChild((*lt)->getLight(), true);
			this->removeChild((*lt)->getParticle(), true);
			this->removeChild((*lt), true);

			if(TREASURE_LIST->empty())
				break;

			if((*lt) != NULL) {		
				world->DestroyBody((*lt)->getBody());
				//delete (*it);
				lt = TREASURE_LIST->erase(lt);
			}

			break;
		}
	}
	*/

	if(player)
	{
		player->update(dt);
	}

	enemy_timer += dt;
	int enemy_time = 1 - (float)level/20;
	if(enemy_time < 0.1)
		enemy_time = 0.1;
	if(enemy_timer >= 1 || (basic_enemy && enemy_timer >= 0.5))
	{
		int enemyCreatRand = rand()%100;

		int kind;
		if(enemyLevel > enemyCreatRand && !basic_enemy)
		{
			kind = 2;
		}
		else
			kind = 1;

		Enemy* enemy = Enemy::create(world, kind, enemy1_shape, enemy2_shape, level);

		this->addChild(enemy,2);

		ENEMY_LIST->push_front(enemy);

		enemy_timer = 0;

		enemyNumber++;
		if(enemyNumber >= 10)
		{
			basic_enemy = false;
		}
	}

	for(list<Enemy*>::iterator it = ENEMY_LIST->begin() ; it != ENEMY_LIST->end() ; it++)
	{
		//if(speedUp)
		//	(*it)->setMoveTimer(100);
		int updateInt = (*it)->_update(dt, player->getPosition(), speedUp);

		CCRect player_rect = CCRect::CCRectMake(
				player->getPositionX() - player->getContentSize().width/2 + 15,
				player->getPositionY() - player->getContentSize().height/2 + 15,
				player->getContentSize().width - 30,
				player->getContentSize().height - 35);

		CCRect enemy_rect = CCRect::CCRectMake(
				(*it)->getPositionX() - (*it)->getContentSize().width/2 + 10,
				(*it)->getPositionY() - (*it)->getContentSize().height/2 + 10,
				(*it)->getContentSize().width - 20,
				(*it)->getContentSize().height - 20);

		if( enemy_rect.intersectsRect(player_rect) && (*it)->getDeadFlag() == 0 )
		{
			CCScene *pScene = CCTransitionFade::create(1, Result::scene());

			CCDirector::sharedDirector()->replaceScene(pScene);
		}
		
		if(updateInt == 1)
		{
			if(ENEMY_LIST->empty())
				break;

			if((*it) != NULL) {		
				world->DestroyBody((*it)->getBody());
				this->removeChild((*it),true);
				//delete (*it);
				it = ENEMY_LIST->erase(it);
			}

			break;
		}
	}

	if(harpoon_fire == false)
	{
		harpoon_timer += dt;
		if(harpoon_timer >= 0.3)
		{
			harpoon_fire = true;
			harpoon_timer = 0;
		}
	}
	if(GetKeyState(VK_SPACE) & KF_UP && harpoon_fire == true)
	{
		Harpoon* e = Harpoon::create(world, 1, player->getPosition(), player->getDirection());

		HARPOON_LIST->push_front(e);

		list<Harpoon*>::iterator it = HARPOON_LIST->begin();

		this->addChild((*it), 1);

		m_Sae->playEffect("Sound/harpoon_effect.wav ", false);

		harpoon_fire = false;
	}

	
	for(list<Harpoon*>::iterator it = HARPOON_LIST->begin() ; it != HARPOON_LIST->end() ; it++)
	{
		int updateInt = (*it)->_update(dt);
		
		for(list<Enemy*>::iterator jt = ENEMY_LIST->begin() ; jt != ENEMY_LIST->end() ; jt++)
		{
			if( (*it)->getDeadTimer() != 0 && (*jt)->getDeadTimer() != 0)
			{
				if( (*it)->getDeadTimer() == (*jt)->getDeadTimer() )
				{
					float changeX = (*it)->boundingBox().size.width/4;

					if((*it)->getDirection() == 2) changeX *= -1; 

					(*it)->setPosition( ccp( (*jt)->getBody()->GetPosition().x*PTM_RATIO + changeX,
						(*jt)->getBody()->GetPosition().y*PTM_RATIO ) );
				}
			}
		}

   		if(updateInt == 1)
		{
			if(HARPOON_LIST->empty())
				break;

			if((*it) != NULL) {		
				world->DestroyBody((*it)->getBody()); 
				this->removeChild((*it), true);
				//delete (*it);
				it = HARPOON_LIST->erase(it);
			}

			break;
		}
	}
	font();
}

void Game::BeginContact(b2Contact* contact)
{
	b2Body* body1 = contact->GetFixtureA()->GetBody();
	b2Body* body2 = contact->GetFixtureB()->GetBody();

	int body_correct = 0;

	// 작살과 보물 충돌
	{
		for(list<Harpoon*>::iterator it = HARPOON_LIST->begin() ; it != HARPOON_LIST->end() ; it++)
		{
			if((*it)->getBody() == body1)
			{
  				body_correct++;
			}
		}
		for(list<Treasure*>::iterator jt = TREASURE_LIST->begin() ; jt != TREASURE_LIST->end() ; jt++)
		{
			if( (*jt)->getBody() == body2 )
			{
				body_correct++;
			}
		}		

		if(body_correct == 2)
		{
			for(list<Enemy*>::iterator kt = ENEMY_LIST->begin() ; kt != ENEMY_LIST->end() ; kt++)
			{
				if( 0 < (*kt)->getPositionX() && (*kt)->getPositionX() < winSize.width &&
					0 < (*kt)->getPositionY() && (*kt)->getPositionY() < winSize.height )
				{
					(*kt)->setDeadFlag(1);
					/*
					if(ENEMY_LIST->empty())
						break;

					if((*kt) != NULL) {		
						world->DestroyBody((*kt)->getBody()); 
						this->removeChild((*kt), true);
						//delete (*it);
						kt = ENEMY_LIST->erase(kt);
						}*/
				}
			}

			for(list<Treasure*>::iterator jt = TREASURE_LIST->begin() ; jt != TREASURE_LIST->end() ; jt++)
			{
				if( (*jt)->getBody() == body2 )
				{
					(*jt)->setDeadFlag(1);
				}
			}	
		}
		body_correct = 0;
	}

	// 캐릭터와 적 충돌
	/*{
	CCRect player_rect;
	CCRect enemy_rect;
	bool rect = false;

	if(player->getBody() == body1)
		{
			body_correct++;

			player_rect = CCRect::CCRectMake(
				player->getPositionX() - player->getContentSize().width/2,
				player->getPositionY() - player->getContentSize().height/2 + 10,
				player->getContentSize().width,
				player->getContentSize().height - 20);
		}

		for(list<Enemy*>::iterator it = ENEMY_LIST->begin() ; it != ENEMY_LIST->end() ; it++)
		{		
			if( (*it)->getBody() == body2 )
			{
				body_correct++;
				if( (*it)->getDeadFlag() != 0 )
					body_correct--;

				enemy_rect = CCRect::CCRectMake(
					(*it)->getPositionX() - (*it)->getContentSize().width/2,
					(*it)->getPositionY() - (*it)->getContentSize().height/2,
					(*it)->getContentSize().width,
					(*it)->getContentSize().height);
			}
		}

		if(player_rect.intersectsRect(enemy_rect))
		{
			rect = true;
		}
		

		if(body_correct == 2 && rect)
		{
			CCScene *pScene = CCTransitionFade::create(1, Result::scene());

			CCDirector::sharedDirector()->replaceScene(pScene);
		}
		body_correct = 0;
	}*/

	// 작살과 적 충돌
	{
		for(list<Harpoon*>::iterator it = HARPOON_LIST->begin() ; it != HARPOON_LIST->end() ; it++)
			if( (*it)->getBody() == body1 && (*it)->getDeadFlag() == 0 && (*it)->getPower())
				body_correct++;

		for(list<Enemy*>::iterator it = ENEMY_LIST->begin() ; it != ENEMY_LIST->end() ; it++)
			if( (*it)->getBody() == body2 && (*it)->getDeadFlag() == 0 )
				body_correct++;

		if(body_correct == 2)
		{
			float harpoon_y = body1->GetPosition().y*PTM_RATIO;
			float enemy_y = body2->GetPosition().y*PTM_RATIO;

			m_Sae->playEffect("Sound/dead_effect.wav", false);

			float multifly = 0;
			for(list<Harpoon*>::iterator it = HARPOON_LIST->begin() ; it != HARPOON_LIST->end() ; it++)
			{
				if( (*it)->getBody() == body1 )
				{
					(*it)->setDeadFlag(1);

					if((*it)->getFlyTimer() < 0.2)
						multifly = 0;
					else
						multifly = ((*it)->getFlyTimer() - 0.2) * 3;

					break;
				}
			}

			for(list<Enemy*>::iterator it = ENEMY_LIST->begin() ; it != ENEMY_LIST->end() ; it++)
			{
				if( (*it)->getBody() == body2 )
				{
					(*it)->setDeadFlag(1);

					int thisScore = 0;
					if( (*it)->getKind() == 1) {
						thisScore += 100;
						thisScore += (int)(100*multifly);
					}
					else if( (*it)->getKind() == 2 ) {
						thisScore += 500;
						thisScore += (int)(500*multifly);
					}

					(*it)->setScoreLabel(thisScore);

					score += thisScore;

					int zari = 1;
					int tempScore = score;
					while(1)
					{
						if(tempScore < 10)
							break;
						else
							zari++;

						tempScore /= 10;
					}

					scoreLabel->setPosition( ccp(winSize.width-15-zari*40, winSize.height-55) );

					break;
				}


				else
				{
					for(list<Harpoon*>::iterator it = HARPOON_LIST->begin() ; it != HARPOON_LIST->end() ; it++)
					{
						if( (*it)->getBody() == body1 )
						{
							(*it)->setPower(false);

							break;
						}
					}
				}
			}
		}
		body_correct = 0;
	}

	// 상어와 적 충돌
	{
		CCRect player_rect;
		CCRect shark_rect;
		bool rect = false;

		if(player->getBody() == body1)
		{
			body_correct++;
		}

		for(list<Shark*>::iterator kt = SHARK_LIST->begin() ; kt != SHARK_LIST->end() ; kt++)
		{
			if( (*kt)->getBody() == body2 )
			{
				body_correct++;
			}
		}

		if(body_correct == 2)
		{
			CCScene *pScene = CCTransitionFade::create(1, Result::scene());

			CCDirector::sharedDirector()->replaceScene(pScene);
		}
		body_correct = 0;
	}
}

void Game::EndContact(b2Contact* contact)
{
}

void Game::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

void Game::PostSolve(b2Contact* contact, const b2ContactImpulse* inpulse)
{
}

void Game::levelUp()
{
	level++;

	enemyLevel += 5;
	if(enemyLevel > 100)
		enemyLevel = 100;

	CCAnimation* animation = CCAnimation::create();
	animation->setDelayPerUnit(0.05);
	for(int i=1; i<=5; i++)
	{
		char str[100] = "0";
		sprintf(str,"GameObj/level_up/%d.png",i);
		animation->addSpriteFrameWithFileName(str);
	}
	CCAnimate *animate = CCAnimate::create(animation);
	levelUpLogo->runAction(animate);

	levelUpLogo->setPosition(ccp(winSize.width/2, winSize.height/2));
	levelUpLogo->setOpacity(255);

	CCFadeTo* fadeTo = CCFadeTo::create(1,0);
	levelUpLogo->runAction(fadeTo);

	m_Sae->playEffect("Sound/level_up.wav");
}