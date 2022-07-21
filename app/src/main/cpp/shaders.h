//
// Created by 李金祥 on 2022/7/21.
//

#ifndef CAMERARECORD_SHADERS_H
#define CAMERARECORD_SHADERS_H

static const char* DEFAULT_VERTEX_SHADER =
        "#ifdef GL_ES                                                                           \n"
        "precision highp float;                                                                 \n"
        "#else                                                                                  \n"
        "#define highp                                                                          \n"
        "#endif                                                                                 \n"
        "attribute vec4 position;                                                               \n"
        "attribute vec4 inputTextureCoordinate;                                                 \n"
        "varying vec2 textureCoordinate;                                                        \n"
        "uniform highp mat4 textureMatrix;                                                      \n"
        "void main() {                                                                          \n"
        "  textureCoordinate = (textureMatrix * inputTextureCoordinate).xy;                     \n"
        "  gl_Position = position;                                                              \n"
        "}                                                                                      \n";

static const char* DEFAULT_OES_FRAGMENT_SHADER =
        "#extension GL_OES_EGL_image_external : require                                         \n"
        "precision mediump float;                                                               \n"
        "uniform samplerExternalOES inputImageTexture;                                          \n"
        "varying vec2 textureCoordinate;                                                        \n"
        "void main() {                                                                          \n"
        "  gl_FragColor = texture2D(inputImageTexture, textureCoordinate);                      \n"
        "}                                                                                      \n";

#endif //CAMERARECORD_SHADERS_H
