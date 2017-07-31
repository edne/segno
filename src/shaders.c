#include <segno.h>

GLuint compile_shader(GLenum type, const GLchar *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint param;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &param);
    if (!param) {
        GLchar log[4096];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "error: %s: %s\n",
                type == GL_FRAGMENT_SHADER ? "frag" : "vert", (char *) log);
        exit(EXIT_FAILURE);
    }
    return shader;
}

GLuint link_program(GLuint vert, GLuint frag) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    GLint param;
    glGetProgramiv(program, GL_LINK_STATUS, &param);
    if (!param) {
        GLchar log[4096];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "error: link: %s\n", (char *) log);
        exit(EXIT_FAILURE);
    }
    return program;
}


Program make_program(const GLchar *vert_shader, const GLchar *frag_shader) {
    Program program;

    GLuint vert = compile_shader(GL_VERTEX_SHADER, vert_shader);
    GLuint frag = compile_shader(GL_FRAGMENT_SHADER, frag_shader);

    program.id = link_program(vert, frag);

    glDeleteShader(frag);
    glDeleteShader(vert);

    program.uniform_matrix = glGetUniformLocation(program.id, "matrix");

    return program;
}

void free_program(Program program) {
    glDeleteProgram(program.id);
}
