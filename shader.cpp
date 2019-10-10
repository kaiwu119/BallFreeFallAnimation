#include "shader.h"

Shader::Shader()
{

}

Shader::Shader(QString vertexFile, QString fragmentFile, QString geometryFile)
{
    program =new QGLShaderProgram();
    initShader(vertexFile,fragmentFile,geometryFile);
}

void Shader::initShader(QString vertexFile, QString fragmentFile, QString geometryFile)
{

    program->addShaderFromSourceFile(QGLShader::Vertex,vertexFile);
    program->addShaderFromSourceFile(QGLShader::Fragment,fragmentFile);
    if(!geometryFile.isEmpty())
        program->addShaderFromSourceFile(QGLShader::Geometry,geometryFile);

    //    qDebug()<<"makeshader ok";
}

void Shader::use()
{
    program->link();
    program->bind();
}
