#ifndef SHADER_H
#define SHADER_H

#include <QString>
#include <QGLShaderProgram>
#include <QGLShader>
#include <QOpenGLFunctions>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GLES3/gl3.h>
class Shader
{
private:
    QGLShaderProgram *program;
public:
    Shader();
    Shader(QString vertexFile,QString fragmentFile,QString geometryFile="");
    void initShader(QString vertexFile,QString fragmentFile,QString geometryFile="");
    void use();
};

#endif // SHADER_H
