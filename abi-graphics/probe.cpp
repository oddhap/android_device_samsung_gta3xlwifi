#include <jni.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_org_lineageos_gta3xlwifi_smoke_MainActivity_probe(JNIEnv* env, jclass, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface egl_surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    bool initialized = false;
    GLuint vertex_shader = 0, fragment_shader = 0, program = 0;
    std::string result = "FAIL: native window unavailable";
    do {
        if (!window) break;
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY || !eglInitialize(display, nullptr, nullptr)) {
            result = "FAIL: eglInitialize"; break;
        }
        initialized = true;
        const EGLint attributes[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES2_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_NONE};
        EGLConfig config;
        EGLint count = 0, format = 0;
        if (!eglChooseConfig(display, attributes, &config, 1, &count) || count != 1 ||
            !eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format) ||
            ANativeWindow_setBuffersGeometry(window, 0, 0, format) != 0 ||
            !eglBindAPI(EGL_OPENGL_ES_API)) {
            result = "FAIL: EGL config/window setup"; break;
        }
        const EGLint context_attributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attributes);
        egl_surface = eglCreateWindowSurface(display, config, window, nullptr);
        if (context == EGL_NO_CONTEXT || egl_surface == EGL_NO_SURFACE ||
            !eglMakeCurrent(display, egl_surface, egl_surface, context)) {
            result = "FAIL: EGL context/surface or buffer import"; break;
        }
        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        if (!vendor || !renderer || !version) { result = "FAIL: GL identification"; break; }
        std::string details = "ABI=" + std::to_string(sizeof(void*) * 8) + " vendor=" + vendor +
            " renderer=" + renderer + " version=" + version;
        if (std::string(renderer).find("Mali") == std::string::npos) {
            result = "FAIL: hardware Mali renderer required; " + details; break;
        }
        const char* vertex_source = "attribute vec2 position; void main() { gl_Position=vec4(position,0.,1.); }";
        const char* fragment_source = "precision mediump float; uniform vec3 color; void main() { gl_FragColor=vec4(color,1.); }";
        auto shader = [](GLenum type, const char* source) {
            GLuint object = glCreateShader(type);
            glShaderSource(object, 1, &source, nullptr);
            glCompileShader(object);
            return object;
        };
        vertex_shader = shader(GL_VERTEX_SHADER, vertex_source);
        fragment_shader = shader(GL_FRAGMENT_SHADER, fragment_source);
        GLint vertex_ok = 0, fragment_ok = 0, link_ok = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_ok);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_ok);
        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
        if (!vertex_ok || !fragment_ok || !link_ok) {
            result = "FAIL: Mali shader compile/link; " + details; break;
        }
        EGLint width = 0, height = 0;
        eglQuerySurface(display, egl_surface, EGL_WIDTH, &width);
        eglQuerySurface(display, egl_surface, EGL_HEIGHT, &height);
        glViewport(0, 0, width, height);
        glUseProgram(program);
        GLint position = glGetAttribLocation(program, "position");
        GLint color = glGetUniformLocation(program, "color");
        const GLfloat vertices[] = {-1.f, -1.f, 1.f, -1.f, 0.f, 1.f};
        if (position < 0 || color < 0 || width < 1 || height < 1) {
            result = "FAIL: shader attributes or window dimensions; " + details; break;
        }
        glEnableVertexAttribArray(static_cast<GLuint>(position));
        glVertexAttribPointer(static_cast<GLuint>(position), 2, GL_FLOAT, GL_FALSE, 0, vertices);
        bool frames_ok = true;
        for (int frame = 0; frame < 120; ++frame) {
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);
            glUniform3f(color, static_cast<float>(frame % 60) / 60.0f, 0.5f, 0.2f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            if (frame == 0 || frame == 119) {
                GLubyte pixel[4] = {};
                glReadPixels(width/2, height/2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
                if (pixel[1] < 120 || pixel[1] > 136 || pixel[2] < 43 || pixel[2] > 59) {
                    frames_ok = false; break;
                }
            }
            if (glGetError() != GL_NO_ERROR || !eglSwapBuffers(display, egl_surface)) {
                frames_ok = false; break;
            }
        }
        glFinish();
        result = (frames_ok && glGetError() == GL_NO_ERROR ? "PASS: 120 shader draws/window buffers/pixel readback; " :
            "FAIL: GL/draw/swap/readback; ") + details;
    } while (false);
    EGLint error = eglGetError();
    if (result.rfind("FAIL:", 0) == 0) result += " EGLerror=" + std::to_string(error);
    if (initialized) {
        if (program) glDeleteProgram(program);
        if (vertex_shader) glDeleteShader(vertex_shader);
        if (fragment_shader) glDeleteShader(fragment_shader);
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl_surface != EGL_NO_SURFACE) eglDestroySurface(display, egl_surface);
        if (context != EGL_NO_CONTEXT) eglDestroyContext(display, context);
        eglTerminate(display);
    }
    if (window) ANativeWindow_release(window);
    return env->NewStringUTF(result.c_str());
}
