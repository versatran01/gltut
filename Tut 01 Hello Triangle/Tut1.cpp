#include <algorithm>
#include <fmt/core.h>
#include <string>
#include <vector>

//
#include <glload/gl_3_2_comp.h>

//
#include <GL/freeglut.h>

std::string GetShaderTypeStr(GLenum shader_type) {
  switch (shader_type) {
  case GL_VERTEX_SHADER:
    return "vertex";
  case GL_GEOMETRY_SHADER:
    return "geometry";
  case GL_FRAGMENT_SHADER:
    return "fragment";
  default:
    return "unknown";
  }
}

GLuint CreateShader(GLenum shader_type, const std::string &shader_src) {
  GLuint shader = glCreateShader(shader_type);
  const char *shader_src_str = shader_src.c_str();
  glShaderSource(shader, 1, &shader_src_str, NULL);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    fmt::print("Compile failure in {} shader:\n",
               GetShaderTypeStr(shader_type));
  }

  return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaders) {
  GLuint program = glCreateProgram();

  for (const auto &shader : shaders) {
    glAttachShader(program, shader);
  }

  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    fmt::print("Linker failure");
  }

  for (const auto &shader : shaders) {
    glDeleteShader(shader);
  }

  return program;
}

GLuint theProgram;

const std::string strVertexShader("#version 330\n"
                                  "layout(location = 0) in vec4 position;\n"
                                  "void main()\n"
                                  "{\n"
                                  "   gl_Position = position;\n"
                                  "}\n");

const std::string
    strFragmentShader("#version 330\n"
                      "out vec4 outputColor;\n"
                      "void main()\n"
                      "{\n"
                      "   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
                      "}\n");

void InitializeProgram() {
  std::vector<GLuint> shaders;
  shaders.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
  shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

  theProgram = CreateProgram(shaders);
  std::for_each(shaders.begin(), shaders.end(), glDeleteShader);
}

const float vertex_positions[] = {
    // clang-format off
    0.75f, 0.75f,  0.0f,   1.0f,   
    0.75f, -0.75f, 0.0f,  1.0f,  
    -0.75f, -0.75f, 0.0f,  1.0f,
    // clang-format on
};

GLuint position_buffer_obj;
GLuint vao;

void InitializeVertexBuffer() {
  glGenBuffers(1, &position_buffer_obj);

  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_obj);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Called after the window and OpenGL are initialized. Called exactly once,
// before the main loop.
void init() {
  InitializeProgram();
  InitializeVertexBuffer();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
}

// Called to update the display.
// You should call glutSwapBuffers after all of your rendering to display what
// you rendered. If you need continuous updates of the screen, call
// glutPostRedisplay() at the end of the function.
void display() {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(theProgram);

  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_obj);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);
  glUseProgram(0);

  glutSwapBuffers();
}

// Called whenever the window is resized. The new window size is given, in
// pixels. This is an opportunity to call glViewport or glScissor to keep up
// with the change in size.
void reshape(int w, int h) { glViewport(0, 0, w, h); }

// Called whenever a key on the keyboard was pressed.
// The key is given by the ''key'' parameter, which is in ASCII.
// It's often a good idea to have the escape key (ASCII value 27) call
// glutLeaveMainLoop() to exit the program.
void keyboard(unsigned char key, int x, int y) {
  if (key == 27) {
    glutLeaveMainLoop();
  }
}

unsigned int defaults(unsigned int displayMode, int &width, int &height) {
  return displayMode;
}
