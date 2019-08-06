/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include<string>
#include "cGame.h"
#include<fstream>


typedef std::chrono::high_resolution_clock Clock;
cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	int i = 1;
	/* Let the computer pick a random number */
	random_device rd;    // non-deterministic engine 
	mt19937 gen{ rd() }; // deterministic engine. For most common uses, std::mersenne_twister_engine, fast and high-quality.
	uniform_int_distribution<> AsteroidDis{ 1, 8};
	uniform_int_distribution<> AsteroidTextDis{ 0, 4 };

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();
	theScore = 0;
	int random1;


	// Store the textures
	textureName = { "asteroid1", "asteroid2", "asteroid3", "asteroid4", "Enemy", "photon","theRocket","theBackground", "explosion", "explosion1"};
	texturesToUse = { "Images\\Sprites\\asteroid1.png", "Images\\Sprites\\asteroid2.png", "Images\\Sprites\\asteroid3.png", "Images\\Sprites\\asteroid4.png","Images\\Sprites\\mainEnemy1.png", "Images\\Sprites\\Photon64x32.png", "Images\\Sprites\\main1.png", "Images\\Bkg\\cyberpunk-street.png", "Images\\Sprites\\explosion.png", "Images\\Sprites\\explosion.png" };
	for (int tCount = 0; tCount < (int)textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge","central" };
	fontsToUse = { "Fonts/RANDOM_DM.ttf", "Fonts/space age.ttf" };
	theFontMgr->addFont("digital","Fonts/RANDOM_DM.ttf", 22 );
	theFontMgr->addFont("spaceAge", "Fonts/space age.ttf", 30);
	theFontMgr->addFont("central", "Fonts/RANDOM_DM.ttf", 18);
	gameTextList = {  "SpaceAssault", "Score : ", "Instructions: use up and down to move","Space/Assault:press E to start" };
	strScore = gameTextList[1];
	strScore += to_string(theScore).c_str();
	
	
	theTextureMgr->addTexture("Title", theFontMgr->getFont("digital")->createTextTexture(theRenderer, gameTextList[0], textType::blended, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theScore", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theInstructions", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, gameTextList[2], textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("TheMenu", theFontMgr->getFont("central")->createTextTexture(theRenderer, gameTextList[3], textType::blended, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	// Load game sounds
	soundList = { "theme", "shot", "explosion" };
	soundTypes = { soundType::music, soundType::sfx, soundType::sfx };
	soundsToUse = { "Audio/who10Edit.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };
	for (int sounds = 0; sounds < (int)soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	theRocket.setSpritePos({ 200, 600 });
	theRocket.setTexture(theTextureMgr->getTexture("theRocket"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("theRocket")->getTWidth(), theTextureMgr->getTexture("theRocket")->getTHeight());
	theRocket.setRocketVelocity(100);
	theRocket.setSpriteTranslation({ 50,0 });
	theRocket.setBoundingRect({ theRocket.getSpritePos().x, theRocket.getSpritePos().y, theRocket.getSpritePos().w, theRocket.getSpritePos().h });


	// Create vector array of textures

	for (int astro = 0; astro < 10; astro++)
	{
			theAsteroids.push_back(new cAsteroid);
			random1 = rand() % 2;
			if (random1 == 0) {
				theAsteroids[astro]->setSpritePos({ (50 * (astro + 1) + (50 * astro)), -30 * (astro * 10) });
				theAsteroids[astro]->setSpriteTranslation({ 200, -150 });
				int randAsteroid = AsteroidTextDis(gen);
				theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
				theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
				theAsteroids[astro]->getSpriteScale();
				theAsteroids[astro]->scaleSprite();
				theAsteroids[astro]->setAsteroidVelocity(250);
				theAsteroids[astro]->setActive(true);
			}
			else if(random1 == 1) {
				theAsteroids[astro]->setSpritePos({ (50 * (astro + 1) + (50 * astro)),  -30 * (astro * 3) });
				theAsteroids[astro]->setSpriteTranslation({ 250, -100 });
				int randAsteroid = AsteroidTextDis(gen);
				theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
				theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
				theAsteroids[astro]->getSpriteScale();
				theAsteroids[astro]->scaleSprite();
				theAsteroids[astro]->setAsteroidVelocity(200);
				theAsteroids[astro]->setActive(true);
			}
			else if (random1==2) {
				theAsteroids[astro]->setSpritePos({ (350 * (astro + 1) + (50 * astro)),  -30 * (astro * 3) });
				theAsteroids[astro]->setSpriteTranslation({ 200, -100 });
				int randAsteroid = AsteroidTextDis(gen);
				theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
				theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
				theAsteroids[astro]->getSpriteScale();
				theAsteroids[astro]->scaleSprite();
				theAsteroids[astro]->setAsteroidVelocity(200);
				theAsteroids[astro]->setActive(true);
			}
			


		
	}

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	
	bool loop = true;
	
	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
		if (theScore == 100) {/*end statement*/
		loop = false;
	}
	}
	
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	// Render each asteroid in the vector array


	// Render the Title
	cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
	SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	FPoint scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	
	/*tempTextTexture = theTextureMgr->getTexture("TheMenu");
	pos = { 10, 350, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	*/


	// Render updated score value
	if (updateScore) {
		theTextureMgr->deleteTexture("theScore");
		theTextureMgr->addTexture("theScore", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		updateScore = false;

	}
	
	tempTextTexture = theTextureMgr->getTexture("theScore");
	pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	tempTextTexture = theTextureMgr->getTexture("theInstructions");
	pos = { 10, 700, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	
	
	
	for (int draw = 0; draw < (int)theAsteroids.size(); draw++)
	{
		theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
	}
	// Render each bullet in the vector array
	for (int draw = 0; draw < (int)theBullets.size(); draw++)
	{
		theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle() + 90, &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
	}
	// Render each explosion in the vector array
	for (int draw = 0; draw < (int)theExplosions.size(); draw++)
	{
		theExplosions[draw]->render(theRenderer, &theExplosions[draw]->getSourceRect(), &theExplosions[draw]->getSpritePos(), theExplosions[draw]->getSpriteScale());
	}

	// render the rocket
	theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

	
}

void cGame::update(double deltaTime)
	
{	
	
	
	if (theRocket.getSpritePos().x <= 0 || theRocket.getSpritePos().x > (WINDOW_WIDTH - theRocket.getSpriteDimensions().w)) {
		theRocket.setSpritePos({ theRocket.getSpritePos().x, theRocket.getSpritePos().y });
		theRocket.setRocketMove(theRocket.getRocketMove()*(-1));
	}
	
	/*the fallowing lines were removed because the ship moves only along X */
	/*else if (theRocket.getSpritePos().y <= 0 || theRocket.getSpritePos().y > (WINDOW_HEIGHT - theRocket.getSpriteDimensions().h)) {
		theRocket.setSpritePos({ theRocket.getSpritePos().x, theRocket.getSpritePos().y });
			theRocket.setRocketMove(theRocket.getRocketMove()*(-1));
	}*/
	
	// Update the visibility and position of each asteriod
	vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();

	while (asteroidIterator != theAsteroids.end())
	{
		bool InScreen = false;
		if ((*asteroidIterator)->isActive() == false)
		{
			asteroidIterator = theAsteroids.erase(asteroidIterator);
		}
		/*Asteroids and Enemyships bouncing off walls and remaining in the screen*/
		else
		{
			bool InScreen = false;
			if ((*asteroidIterator)->getSpritePos().y > (0)) {
				InScreen = true;
			}
			
			if ((*asteroidIterator)->getSpritePos().x >= (WINDOW_WIDTH - (*asteroidIterator)->getSpriteDimensions().w)) {
				(*asteroidIterator)->setSpriteTranslation({ ((*asteroidIterator)->getSpriteTranslation().x)*(-1) ,
					(*asteroidIterator)->getSpriteTranslation().y });
			}
			else if ((*asteroidIterator)->getSpritePos().x <= 0) {
				(*asteroidIterator)->setSpriteTranslation({ ((*asteroidIterator)->getSpriteTranslation().x)*(-1) ,
					(*asteroidIterator)->getSpriteTranslation().y });
			}
			if ((*asteroidIterator)->getSpritePos().y > (WINDOW_HEIGHT - (*asteroidIterator)->getSpriteDimensions().h)) {
				(*asteroidIterator)->setSpriteTranslation({ ((*asteroidIterator)->getSpriteTranslation().x) ,
					(*asteroidIterator)->getSpriteTranslation().y *(-1) });
				}
			if ((*asteroidIterator)->getSpritePos().y <= (0)){
				(*asteroidIterator)->setSpriteTranslation({ ((*asteroidIterator)->getSpriteTranslation().x) ,
					-150 });
			}
			(*asteroidIterator)->update(deltaTime);
			
			++asteroidIterator;
			
			
		}
	}
	
	// Update the visibility and position of each bullet
	vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
	while (bulletIterartor != theBullets.end())
	{
		if ((*bulletIterartor)->isActive() == false)
		{
			bulletIterartor = theBullets.erase(bulletIterartor);
		}
		else
		{
			
			(*bulletIterartor)->update(deltaTime);
			++bulletIterartor;
		}
	}
	
	// Update the visibility and position of each explosion
	vector<cSprite*>::iterator expIterartor = theExplosions.begin();
	while (expIterartor != theExplosions.end())
	{
		if ((*expIterartor)->isActive() == false)
		{
			expIterartor = theExplosions.erase(expIterartor);
		}
		else
		{
			(*expIterartor)->animate(deltaTime);
			++expIterartor;
		}
	}

	/*
	==============================================================
	| Check for collisions
	==============================================================
	*/
	for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
	{
		//(*bulletIterartor)->update(deltaTime);
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
			{
				// if a collision set the bullet and asteroid to false
				(*asteroidIterator)->setActive(false);
				(*bulletIterartor)->setActive(false);
				theExplosions.push_back(new cSprite);
				int index = theExplosions.size()-1;
				theExplosions[index]->setSpriteTranslation({ 0, 0 });
				theExplosions[index]->setActive(true);
				theExplosions[index]->setNoFrames(16);
				theExplosions[index]->setTexture(theTextureMgr->getTexture("explosion1"));
				theExplosions[index]->setSpriteDimensions(theTextureMgr->getTexture("explosion1")->getTWidth()/ theExplosions[index]->getNoFrames(), theTextureMgr->getTexture("explosion1")->getTHeight());
				theExplosions[index]->setSpritePos({ (*asteroidIterator)->getSpritePos().x + (int)((*asteroidIterator)->getSpritePos().w/2), (*asteroidIterator)->getSpritePos().y + (int)((*asteroidIterator)->getSpritePos().h / 2) });
				
				theSoundMgr->getSnd("explosion")->play(0);
				theScore += 10;
				strScore = gameTextList[1];
				strScore = std::to_string(theScore);
				updateScore = true;

				
				
				
			}
		
			/*else if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(theRocket.getSpritePos()) ))
			{
				(*asteroidIterator)->setActive(false);
				theExplosions.push_back(new cSprite);
				int index = theExplosions.size() - 1;
				theExplosions[index]->setSpriteTranslation({ 0, 0 });
				theExplosions[index]->setActive(true);
				theExplosions[index]->setNoFrames(16);
				theExplosions[index]->setTexture(theTextureMgr->getTexture("explosion1"));
				theExplosions[index]->setSpriteDimensions(theTextureMgr->getTexture("explosion1")->getTWidth() / theExplosions[index]->getNoFrames(), theTextureMgr->getTexture("explosion1")->getTHeight());
				theExplosions[index]->setSpritePos({ (*asteroidIterator)->getSpritePos().x + (int)((*asteroidIterator)->getSpritePos().w / 2), (*asteroidIterator)->getSpritePos().y + (int)((*asteroidIterator)->getSpritePos().h / 2) });

				theSoundMgr->getSnd("explosion")->play(0);
			}*/
		}
	}


	// Update the Rockets position
	theRocket.update(deltaTime);
	
	

}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_DOWN:
				{
					theRocket.setRocketMove(2);
				}
				break;

				case SDLK_UP:
				{
					theRocket.setRocketMove(-2);
				}
				break;
				case SDLK_RIGHT:
				{
					//theRocket.setSpriteRotAngle(theRocket.getSpriteRotAngle() +5);
					theRocket.setRocketMove(2);
					
				}
				break;

				case SDLK_LEFT:
				{
					//theRocket.setSpriteRotAngle(theRocket.getSpriteRotAngle() - 5);
					theRocket.setRocketMove(-2);
					
				}
				break;
				case SDLK_SPACE:
				{
					
						theBullets.push_back(new cBullet);
						int numBullets = theBullets.size() - 1;

						theBullets[numBullets]->setSpritePos({ theRocket.getBoundingRect().x + theRocket.getSpriteCentre().x, theRocket.getBoundingRect().y + theRocket.getSpriteCentre().y });
						theBullets[numBullets]->setSpriteTranslation({ 50, 50 });
						theBullets[numBullets]->setTexture(theTextureMgr->getTexture("photon"));
						theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("photon")->getTWidth(), theTextureMgr->getTexture("photon")->getTHeight());
						theBullets[numBullets]->setBulletVelocity(50);
						theBullets[numBullets]->setSpriteRotAngle(theRocket.getSpriteRotAngle() - 90);
						theBullets[numBullets]->setActive(true);
						cout << "Bullet added to Vector at position - x: " << theRocket.getBoundingRect().x << " y: " << theRocket.getBoundingRect().y << endl;
						
					
					
				}
				theSoundMgr->getSnd("shot")->play(0);
				
				break;
				default:
					break;
				}

			default:
				break;
		}

	}/*ofstream copy the score amount into a .txt file*/
	ofstream myFile;
	myFile.open("SCORE.txt");
	myFile << "the Score is : " + std::to_string(theScore);
	myFile.close();
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	//Quit FONT system
	TTF_Quit();

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

