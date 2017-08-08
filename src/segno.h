#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <libguile.h>
#include <gl-matrix.h>

#define M_PI 3.141592653589793


typedef struct {
    int n;
    GLuint vertex_buffer;
    GLuint vertex_array;
    double matrix[16];  // NOT mat4_t because I want it on the stack
} Shape;


typedef struct {
    GLuint id;
    GLuint uniform_matrix;
} Program;


typedef struct {
    Program program;
    GLFWwindow *window;
} Context;


Context gl_init();
void gl_loop(Context context);
void gl_clean(Context context);


Program program_new(const GLchar *vert_shader,
                    const GLchar *frag_shader);
void program_free(Program program);


void shape_draw(SCM shape, Program program);

void guile_bind_primitives();

GLFWwindow *global_window;
