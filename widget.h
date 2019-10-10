#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGLWidget>
#include <QVector>
#include <QVector3D>
//#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <QPoint>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QGLFunctions>
#include <QTimer>
#include <QDesktopWidget>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QGLShader>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLExtraFunctions>

//#include <GL/glext.h>
//#include <GLES3/gl3.h>
//#include <QOpenGLWidget>
#define PI 3.1415926
namespace Ui {
class Widget;
}

struct Face
{
    int v[3];
    int t[3];
    int n[3];
};
class Texture : public QGLWidget,protected QGLFunctions
{
public:
        static void loadTexture(QString filepath, GLuint *texture)
        {
            QImage tex, buf;
            if(!buf.load(filepath))
            {
                qDebug()<<"Error: failed to load image!";
                exit(1);
            }
            tex = convertToGLFormat(buf);
            glGenTextures(1, texture);

            glBindTexture(GL_TEXTURE_2D, *texture);
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex.width(), tex.height(), GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,3);
        }
    static void loadTexture(QOpenGLTexture *&texture, const QString &img)
    {
        glEnable(GL_TEXTURE_2D);
        texture = new QOpenGLTexture(QImage(img));

        texture->setMinificationFilter(QOpenGLTexture::Linear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
    }

};
class Widget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void BuildVase(int v_levels, int h_levels);
    void BuildBall(int R, int split_v, int split_h, QVector3D positon = QVector3D(0,0,0));
    void drawBall(QVector3D position= QVector3D(0,0,0));
    void drawVase();
    void setRadius(float r);
    //void drawObj();
    QVector3D cross(QVector3D a, QVector3D b);

    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);


    QOpenGLShaderProgram *program;
    QOpenGLShaderProgram *depthShader;
    GLuint *depthMapFBO;
    GLuint *depthMap;
    QMatrix4x4 Projection,Model,View;
    void makeshader();
    QOpenGLTexture* texture0,*texture1;

    QVector<QVector3D> m_vertexs;
    QVector<QVector3D> m_texcoords;
    QVector<QVector3D> m_normals;
    void init();
    void stop();
    void start(int fps);

    GLdouble *vertexs;
    GLdouble *texcoords;
    GLdouble *normals;
    GLuint *tex0;//纹理
    GLuint *tex1;//纹理
    float horizontal;
    float vertical;
    float dist;
    QPoint m_last;

    void setHeight(int value);

    void setLossrate(float value);

    void setV0(float value);
    void setTime();
    void RenderCube();

public slots:
    void updateScane();

private:
    void genDepthMap(GLuint *&depthMapFBO,GLuint *&depthMap,GLuint SHADOW_WIDTH,GLuint SHADOW_HEIGHT);

    float time;
    float v0;
    float cur_v;
    float radius;
    float max_height;
    float lossrate;
    QVector3D curPos;
    QTimer *fps;
    int ballHeight;
    QOpenGLFunctions_3_3_Core * core;


    Ui::Widget *ui;

};

#endif // WIDGET_H
