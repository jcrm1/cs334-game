#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

class Shader {
  public:
  unsigned int id;
  Shader(const char *vertexFilePath, const char *fragmentFilePath) {
    std::string vertexSource;
    std::string fragmentSource;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      vertexShaderFile.open(vertexFilePath);
      fragmentShaderFile.open(fragmentFilePath);
      std::stringstream vertexShaderStream;
      std::stringstream fragmentShaderStream;
      vertexShaderStream << vertexShaderFile.rdbuf();
      vertexShaderFile.close();
      fragmentShaderFile.close();
      vertexSource = vertexShaderStream.str();
      fragmentSource = fragmentShaderStream.str();
    } catch (std::ifstream::failure e) {
      fprintf(stderr, "Shader.hpp: error while attempting to read file\n");
    }
    const char* vertexShaderCode = vertexSource.c_str();
    const char* fragmentShaderCode = fragmentSource.c_str();
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      fprintf(stderr, "Shader.hpp: error while compiling vertex shader\n%s\n", infoLog);
    };
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      fprintf(stderr, "Shader.hpp: error while compiling fragment shader\n%s\n", infoLog);
    };

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    // print linking errors if any
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(id, 512, NULL, infoLog);
      fprintf(stderr, "Shader.hpp: error while linking shader program\n%s\n", infoLog);
    }
    // delete shaders; they’re linked into our program and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  };
  void use() {
    glUseProgram(id);
  }
  void setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
  }
  void setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
  }
  void setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
  }
};

#endif