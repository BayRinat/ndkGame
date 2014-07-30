#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include "Balls.h"

class Engine {
	enum {DOWN, UP, MOVE, CANCEL};
public:
	Engine();
	virtual ~Engine();
	bool isRunning();
	struct android_app* app;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	void onStart();
	void onResume();
	void onPause();
	int engine_init_display();
	void engine_draw_frame();
	void engine_term_display();
	static int32_t engine_handle_input(struct android_app* app, AInputEvent* event);
	static void engine_handle_cmd(struct android_app* app, int32_t cmd);
private:
	bool running;
	double elapseTime;
	double lastTime;
	double betweenTap; //for restart
	double now();
	int32_t width;
	int32_t height;
	Balls* balls;
	std::list<Ball*> ballsList;
};
