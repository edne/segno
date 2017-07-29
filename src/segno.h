#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>

#define GLFW_INCLUDE_NONE
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define countof(x) (sizeof(x) / sizeof(0[x]))

#define M_PI 3.141592653589793
#define ATTRIB_POINT 0

GLuint
compile_shader(GLenum type, const GLchar *source);

GLuint
link_program(GLuint vert, GLuint frag);