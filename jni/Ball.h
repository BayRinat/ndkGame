#include <GLES2/gl2.h>
#include <cmath>
#include <map>
#include <vector>

class Ball {
	enum {DOWN, UP, MOVE, CANCEL};  //for crossing analysis
public:
	Ball();
	Ball(const int& width_, const int& height_, const float& lastXPos);
	Ball(const int& width_, const int& height_, const float& lastXPos, Ball&);
	~Ball();
	void initialize();
	void render();
	float getLastPos();
	void update(double elapseTime);
	void crossingAnalysis(int state, int x, int y, int pointerId);
	bool needToDestroy();
	bool getType();
	bool needErase();
	bool getGameOver();

private:
	static constexpr int pointsCount = 50;
	static constexpr float border = 0.03f;
	static constexpr double radius = 70.0;
	static constexpr int possibilityWhite = 70;
	static constexpr double velocity = 0.15f;

	bool erase;
	bool gameOver;
	float* color;
	float* shift;
	float points[pointsCount*2];
	float lastXPos;
	bool destroy;
	bool type;
	std::map<int, std::vector<bool>> touchMap;

	bool inside(int x, int y);
	double distance(float x, float y);
	double currentRadius(float x, float y);

	//size params
	double xSize;
	double ySize;
	int width;
	int height;
	double xRadius;
	double yRadius;

	//shader
	void initializeShader();
	GLuint gProgram;
	GLuint gvPositionHandle;
	GLuint gvOffsetHandle;
	GLuint gvColorHandle;
	static const char* gVertexShader;
	static const char* gFragmentShader;
	static GLuint loadShader(GLenum shaderType, const char* pSource);
	static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
};
