#include <GLES2/gl2.h>
#include <cmath>

class Life {
public:
	Life(const int& width_, const int& height_, const int& number_);
	~Life();
	void render();

	//shader
private:
	static constexpr float border = 0.03f;
	static constexpr float lineLength = 20.0f;
	static constexpr int pointsCount = 20;
	void initializeShader();
	void initialize();
	void initializePoints();
	//size params
	double xSize;
	double ySize;
	int number;
	int width;
	int height;
	double xRadius;
	double yRadius;
	float* color;
	float* shift;
	float points[2*pointsCount];
	//shader
	GLuint gProgram;
	GLuint gvPositionHandle;
	GLuint gvOffsetHandle;
	GLuint gvColorHandle;
	static const char* gVertexShader;
	static const char* gFragmentShader;
	static GLuint loadShader(GLenum shaderType, const char* pSource);
	static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
};
