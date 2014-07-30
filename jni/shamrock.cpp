#include <jni.h>
#include <errno.h>
#include <android_native_app_glue.h>
#include <cstdlib>
#include "Engine.h"

void android_main(struct android_app* state) {
    Engine engine;
    srand(static_cast <unsigned> (time(0)));

    app_dummy();

    state->userData = &engine;
    state->onAppCmd = Engine::engine_handle_cmd;
    state->onInputEvent = Engine::engine_handle_input;
    engine.app = state;

    engine.onStart();
    while (1) {
    	int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(engine.isRunning() ? 0 : -1, nullptr, &events,
                (void**)&source)) >= 0) {

            if (source != nullptr) {
                source->process(state, source);
            }

            if (state->destroyRequested != 0) {
                engine.engine_term_display();
                return;
            }
        }

        if (engine.isRunning()) {
            engine.engine_draw_frame();
        }

    }
}

