#include "Engine.h"

Engine::Engine() {
	display = nullptr;
	surface = nullptr;
	context = nullptr;
	height = 0;
	width = 0;
	app = nullptr;
	running = 0;
	elapseTime = 0.0;
	lastTime = 0.0;
	betweenTap = 0.0;
	balls = nullptr;
}

Engine::~Engine() {
	delete balls;
}

void Engine::onStart() {
	running = 1;
	elapseTime = 0.0f;
	lastTime = now();
}

void Engine::onResume() {
	running = 1;
	double currentTime = now();
	elapseTime = (currentTime - lastTime);
	lastTime = currentTime;
	if(balls != nullptr)
		ballsList = balls->getBallsList();
}

void Engine::onPause() {
	running = 0;
	elapseTime = 0.0f;
	lastTime = now();
}

double Engine::now() {
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);
	return (time.tv_sec*1000000000LL + time.tv_nsec) * 1.0e-9;
}


bool Engine::isRunning(){
	return running;
}

int Engine::engine_init_display() {

	const EGLint attribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_NONE
	};
	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, app->window, nullptr);

	const EGLint contextAttribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
	};

	context = eglCreateContext(display, config, nullptr, contextAttribs);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	this->display = display;
	this->context = context;
	this->surface = surface;
	width = w;
	height = h;
	if(balls == nullptr)
		balls = new Balls();
	if(balls != nullptr){
		balls->initialize(width, height);
		balls->setBallsList(ballsList);
	}

	return 0;
}

void Engine::engine_draw_frame() {
    if (display == nullptr) {
        // No display.
        return;
    }
    eglSwapBuffers(display, surface);
    onResume();
    if(!balls->getGameOver())
    	balls->update(elapseTime);
    balls->render();
}

void Engine::engine_term_display() {
    if (this->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
}

int32_t Engine::engine_handle_input(struct android_app* app, AInputEvent* event) {
    Engine* engine = (Engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    	unsigned int action  = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
    	int pointerCount = AMotionEvent_getPointerCount(event);
    	double angle = -1;
    	bool wasGo = 0;
    	bool wasShoot = 0;
		for (int32_t i = 0; i < pointerCount; i++) {
			int32_t x = AMotionEvent_getX(event, i);
			int32_t y = AMotionEvent_getY(event, i);
            size_t pointerId = AMotionEvent_getPointerId(event, i);
            size_t pointerIndex = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

            if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN) {
            	if(i == pointerIndex){
            		engine->balls->touchAnalysis(DOWN, x, y, pointerId);
            		if(engine->balls->getGameOver()){
            			if(engine->betweenTap == 0.0){
            				engine->betweenTap = engine->now();
            			} else {
            				if(engine->now() - engine->betweenTap < 1)
            					engine->balls->setNewGame();
            				engine->betweenTap = 0.0;
            			}

            		}
            	}
            }
            if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP) {
            	if(i == pointerIndex){
            		engine->balls->touchAnalysis(UP, x, y, pointerId);
             	}
            }
            if (action == AMOTION_EVENT_ACTION_MOVE) {
            	engine->balls->touchAnalysis(MOVE, x, y, pointerId);
        	}
            if (action == AMOTION_EVENT_ACTION_CANCEL) {
            	if(i == pointerIndex){
            		engine->balls->touchAnalysis(CANCEL, x, y, pointerId);
             	}
            }
		}
        return 1;
    }
    return 0;
}

void Engine::engine_handle_cmd(struct android_app* app, int32_t cmd) {
    Engine* engine = (Engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (engine->app->window != nullptr) {
                engine->engine_init_display();
                engine->engine_draw_frame();
            }
            break;
        case  APP_CMD_START:
        	engine->onStart();
        	break;
        case APP_CMD_TERM_WINDOW:
            engine->engine_term_display();
            break;
        case APP_CMD_GAINED_FOCUS:
            engine->onResume();
            break;
        case APP_CMD_LOST_FOCUS:
            engine->onPause();
            break;
    }
}
