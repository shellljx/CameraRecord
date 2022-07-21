//
// Created by 李金祥 on 2022/6/4.
//

#ifndef SOCKET_SRC_RENDERER_SHADERLOADER_H_
#define SOCKET_SRC_RENDERER_SHADERLOADER_H_
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

static void checkCompileError(GLuint shader, std::string type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "shader compile error " << type << " info " << infoLog << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "program linking error " << type << infoLog << std::endl;
    }
  }
}

static GLuint loadShader(const char * vertex, const char* frag) {
  const char* vShaderCode = vertex;
  const char* fShaderCode = frag;

  GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShaderId, 1, &vShaderCode, nullptr);
  glCompileShader(vShaderId);
  checkCompileError(vShaderId, "VERTEX");
  GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShaderId, 1, &fShaderCode, nullptr);
  glCompileShader(fShaderId);
  checkCompileError(fShaderId, "FRAGMENT");

  GLuint program = glCreateProgram();
  glAttachShader(program, vShaderId);
  glAttachShader(program, fShaderId);
  glLinkProgram(program);
  checkCompileError(program, "PROGRAM");
  glDeleteShader(vShaderId);
  glDeleteShader(fShaderId);
  return program;
}
#endif //SOCKET_SRC_RENDERER_SHADERLOADER_H_
