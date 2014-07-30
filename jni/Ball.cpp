#include "Ball.h"

Ball::Ball() {
}

Ball::Ball(const int& width_, const int& height_, const float& lastXPos_) {
	width = width_;
	height = height_;
	xSize = 2.0f / width;
	ySize = 2.0f / height;
	lastXPos = lastXPos_;
	initialize();
	initializeShader();
}

Ball::Ball(const int& width_, const int& height_, const float& lastXPos_, Ball& ball) :
		Ball(width_, height_, lastXPos_){
	this->type = ball.type;
	this->shift = ball.shift;
	this->color = ball.color;
	this->erase = ball.erase;
	this->gameOver = ball.gameOver;
	this->destroy = ball.destroy;
}

Ball::~Ball() {
	delete[] shift;
	delete[] color;
}

void Ball::initialize() {
	xRadius = radius * xSize;
	yRadius = radius * ySize;
	for (int i = 0; i < pointsCount; i++){
		float percent = (i / (float) (pointsCount - 1));
		float rad = percent * 2 * M_PI;
		points[2 * i]      = xRadius* cos(rad);
		points[2 * i + 1]  = yRadius * sin(rad);
	}
	type = rand() % 100 > possibilityWhite ? 0 : 1;  //0 - black, 1 - white
	shift = new float[2];  //start shift
	float rightSide = (1.0f - xRadius - border);
	do{
		shift[0] = rightSide - 2 * rightSide * (rand() % 100)/100;

	} while(abs(shift[0] - lastXPos) < 4 * xRadius);

	shift[1] = 1.0f + yRadius;  //outside

	float fColor = type ? 1.0f : 0.3f;
	color = new float[4];
	color[0] = fColor;
	color[1] = fColor;
	color[2] = fColor;
	color[3] = 1.0f;

	destroy = 0;
	erase = 0;
	gameOver = 0;
}

void Ball::initializeShader() {
	GLenum error;
	gProgram = createProgram(gVertexShader,
			gFragmentShader);
	error = glGetError();
	gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
	error = glGetError();
	gvOffsetHandle = glGetUniformLocation(gProgram, "vOffset");
	error = glGetError();
	gvColorHandle = glGetUniformLocation(gProgram, "vColor");
	error = glGetError();

	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
}

void Ball::update(double elapseTime){
	shift[1] -= velocity * elapseTime;
	if(type == 1){
		if(shift[1] < -1.0f + yRadius)
			gameOver = 1;
	} else {
		if(shift[1] < -1.0f - yRadius)
			erase = 1;
	}
}

void Ball::render() {
	GLenum error;
	glUseProgram(gProgram);
	error = glGetError();
	glUniform4fv(gvColorHandle, 1, color);
	error = glGetError();
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			points);
	error = glGetError();
	glEnableVertexAttribArray(gvPositionHandle);
	error = glGetError();
	glUniform2fv(gvOffsetHandle, 1, shift);
	error = glGetError();
	glEnableVertexAttribArray(gvOffsetHandle);
	error = glGetError();
	glDrawArrays(GL_TRIANGLE_FAN, 0, pointsCount);
	error = glGetError();
}

float Ball::getLastPos(){
	return shift[0];
}

void Ball::crossingAnalysis(int state, int x, int y, int pointerId){
	switch(state){
		case (DOWN):{
			std::vector<bool> vec;
			for(int i = 0; i < 2; ++i){
				vec.push_back(0);
			}
			touchMap.insert(std::pair<int,std::vector<bool>>(pointerId,vec));
			break;
		}
		case (UP):{
			for (std::map<int, std::vector<bool>>::iterator it = touchMap.begin(); it != touchMap.end(); ++it){
				if(it->first == pointerId){
					touchMap.erase(it);
				}
			}
			break;
		}
		case (CANCEL):{
			for (std::map<int, std::vector<bool>>::iterator it = touchMap.begin(); it != touchMap.end(); ++it){
				if(it->first == pointerId){
					touchMap.erase(it);
				}
			}
			break;
		}
		case (MOVE):{
			for (std::map<int, std::vector<bool>>::iterator it = touchMap.begin(); it != touchMap.end(); ++it){
				if(it->first == pointerId){
					if(inside(x, y)){
						if(it->second[0] == 1)  //was first touch outside the cirlce
							it->second[1] = 1;
					} else {
						if(it->second[1] == 1){  //was touch inside the cirlce
							destroy = 1;
						} else
							it->second[0] = 1;
						it->second[1] = 0;
					}
				}
			}
			break;
		}
	};
}

bool Ball::inside(int x_, int y_){
	float x = xSize*(x_ - width/2);
	float y = ySize*(height/2 - y_);
	if(distance(x, y) < currentRadius(x, y))
		return 1;
	return 0;
}

double Ball::distance(float x, float y){
	return (x - shift[0]) * (x - shift[0]) + (y - shift[1]) * (y - shift[1]);
}

double Ball::currentRadius(float x, float y){
	float dx = x - shift[0];
	float dy = y - shift[1];
	double angle = 0.0;
	if (dx != 0.0){
		angle = atan(dy/dx);
		if(x < shift[0]){
			angle += M_PI;
		}
	} else {
		if(y < shift[1])
			angle = -M_PI/2;
		 else
			angle = M_PI/2;
	}
	return distance(xRadius*cos(angle) + shift[0], yRadius*sin(angle) + shift[1]);
}

bool Ball::needToDestroy(){
	if(destroy){
		color[3] -= 0.05f;
		if(color[3] < 0.0f)
			color[3] = 0.0f;
	}
	return (destroy && color[3] == 0.0f) ;
}

bool Ball::getType(){
	return type;
}

bool Ball::needErase(){
	return erase;
}

bool Ball::getGameOver(){
	return gameOver;
}

//shader
GLuint Ball::loadShader(GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	if (shader) {
		glShaderSource(shader, 1, &pSource, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, nullptr, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint Ball::createProgram(const char* pVertexSource,
		const char* pFragmentSource) {
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		return 0;
	}

	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program) {
		GLenum error;
		glAttachShader(program, vertexShader);
		error = glGetError();
		glAttachShader(program, pixelShader);
		error = glGetError();
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, nullptr, buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	glDeleteShader(pixelShader);
	glDeleteShader(vertexShader);
	return program;
}

const char* Ball::gFragmentShader =
		"precision mediump float;\n"
		" uniform vec4 vColor; \n"
		"void main() {\n"
		"  gl_FragColor = vColor;\n"
		"}\n";
const char* Ball::gVertexShader =
		"attribute vec2 vPosition;\n"
		"uniform vec2 vOffset;\n"
		"void main() {\n"
		"  gl_Position = vec4(vPosition + vOffset,0.0,1.0);\n"
		"}\n";
