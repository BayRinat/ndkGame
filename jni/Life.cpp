#include "Life.h"

Life::Life(const int& width_, const int& height_,  const int& number_) {
	width = width_;
	height = height_;
	number = number_;
	xSize = 2.0f / width;
	ySize = 2.0f / height;
	initialize();
	initializeShader();
}


Life::~Life() {
	delete[] color;
	delete[] shift;
}

void Life::initialize(){
	xRadius = lineLength * xSize;
	yRadius = lineLength * ySize;
	for (int i = 0; i < pointsCount; i++){
		float percent = (i / (float) (pointsCount - 1));
		float rad = percent * 2 * M_PI;
		points[2 * i]      = xRadius * cos(rad);
		points[2 * i + 1]  = yRadius * sin(rad);
	}

	shift = new float[2];  //start shift
	shift[0] = -1.0f + xRadius + border;
	shift[0] += (2 * xRadius + border) * number;
	shift[1] = 1.0f - yRadius - border;
	color = new float[4];
	color[0] = 1.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 0.5f;
}


void Life::initializeShader() {
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

void Life::render() {
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

//shader
GLuint Life::loadShader(GLenum shaderType, const char* pSource) {
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

GLuint Life::createProgram(const char* pVertexSource,
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

const char* Life::gFragmentShader =
		"precision mediump float;\n"
		" uniform vec4 vColor; \n"
		"void main() {\n"
		"  gl_FragColor = vColor;\n"
		"}\n";
const char* Life::gVertexShader =
		"attribute vec2 vPosition;\n"
		"uniform vec2 vOffset;\n"
		"void main() {\n"
		"  gl_Position = vec4(vPosition + vOffset,0.0,1.0);\n"
		"}\n";
