#include "Balls.h"

Balls::Balls() {
	gameOver = 0;
	lastXPos = 0.0;
	lifeCount = beginLifeCount;
}

Balls::~Balls() {
}

void Balls::initialize(int width_, int height_){
	srand (time(0));
	secCount = 0.0;
	width = width_;
	height = height_;
	lastXPos = 0.0f;
	lifesList.clear();
	int i = 0;
	while (i < lifeCount ){
		Life* life = new Life(width, height, i);
		lifesList.push_back(life);
		i++;
	}
}

void Balls::touchAnalysis(int state, int x, int y, int pointerId){
	for (std::list<Ball*>::iterator it = ballsList.begin(); it != ballsList.end(); ++it){
		(*it)->crossingAnalysis(state, x, y, pointerId);
	}
}

void Balls::render(){
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	for (std::list<Ball*>::iterator it = ballsList.begin(); it != ballsList.end(); ++it){
		(*it)->render();
	}
	for (std::list<Life*>::iterator it2 = lifesList.begin(); it2 != lifesList.end(); ++it2){
		(*it2)->render();
	}
}

void Balls::addBall(){
	Ball* ball = new Ball(width, height, lastXPos);
	ballsList.push_back(ball);
	lastXPos = ball->getLastPos();
}

void Balls::update(double elapseTime){
	secCount += elapseTime;
	if(secCount > 0.5){
		addBall();
		secCount = 0.0;
	}
	for (std::list<Ball*>::iterator it = ballsList.begin(); it != ballsList.end(); ++it){
		if((*it)->getGameOver()){
			gameOver = 1;
			break;
		}
		if((*it)->needToDestroy()){
			if(!lifesList.empty() && (*it)->getType() == 0){
				lifesList.pop_back();
				lifeCount--;
			}
			ballsList.erase(it++);
		} else {
			if((*it)->needErase())
				ballsList.erase(it++);
			(*it)->update(elapseTime);
		}
	}
	if(lifesList.empty() && !gameOver)
		gameOver = 1;
}

bool Balls::getGameOver(){
	return gameOver;
}

std::list<Ball*> Balls::getBallsList(){
	return ballsList;
}

void Balls::setBallsList(std::list<Ball*> list){
	ballsList.clear();
	for (std::list<Ball*>::iterator it = list.begin(); it != list.end(); ++it){
		Ball* ball = new Ball(width, height, lastXPos, *(*it));
		ballsList.push_back(ball);
	}
}

void Balls::setNewGame(){
	ballsList.clear();
	lifesList.clear();
	secCount = 0.0;
	lastXPos = 0.0;
	lifeCount = beginLifeCount;
	int i = 0;
	while (i < lifeCount ){
		Life* life = new Life(width, height, i);
		lifesList.push_back(life);
		i++;
	}
	gameOver = 0;
}
