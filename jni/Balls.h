#include <list>
#include <Ball.h>
#include "Life.h"
#include <time.h>

class Balls {
public:
	Balls();
	~Balls();
	void initialize(int width_, int height_);
	void render();
	void update(double elapseTime);
	void touchAnalysis(int state, int x, int y, int pointerId);
	void addBall();
	bool getGameOver();
	void setNewGame();
	std::list<Ball*> getBallsList();
	void setBallsList(std::list<Ball*> list);

private:
	static constexpr int beginCount = 3;
	static constexpr int beginLifeCount = 3;
	static constexpr int countPerSecond = 9;
	int lifeCount;

	std::list<Ball*> ballsList;
	std::list<Life*> lifesList;
	int width;
	int height;
	float lastXPos;  // to prevent overlap
	double secCount;
	bool gameOver;
};
