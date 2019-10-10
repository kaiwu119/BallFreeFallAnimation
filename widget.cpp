#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //this->setGeometry(500,500,1500,900);
    QDesktopWidget* desktopWidget = QApplication::desktop();

    //获取设备屏幕大小
    QRect screenRect = desktopWidget->screenGeometry();
    int x = screenRect.width();
    int y = screenRect.height();
    this->setGeometry(x/4,y/4,x/2,y/2);
    dist = 2000.0;
    horizontal = -50.0;
    vertical = 30.0;
    tex0 = new GLuint;
    tex1 = new GLuint;
    fps = new QTimer(this);
    connect(fps,SIGNAL(timeout()),this,SLOT(updateScane()));
    //fps->start(10);
    curPos.setY(400);
    time = 0;
    v0=0;
    max_height = curPos.y();
    lossrate = 0.6;
    this->setWindowTitle(QString("球体自由落体模拟"));
    program = new QOpenGLShaderProgram();
    depthShader = new QOpenGLShaderProgram();
    radius = 50;

}

Widget::~Widget()
{
    delete ui;
    delete vertexs;
    delete texcoords;
    delete normals;
    delete tex0;//纹理
    delete tex1;//纹理
    delete fps;
}

void Widget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(135.0/255,206.0/255,250.0/255,1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel (GL_SMOOTH);

    glEnable(GL_TEXTURE_2D);


    BuildBall(radius,30,30);
    Texture::loadTexture(texture0,":/football.jpg");
    Texture::loadTexture(texture1,":/ca.jpg");
    makeshader();
    core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    Texture::loadTexture(":/ca.jpg",tex1);
    Texture::loadTexture(":/football.jpg",tex0);
    depthMapFBO = new GLuint;
    depthMap = new GLuint;
    genDepthMap(depthMapFBO,depthMap,1024,1024);
    //    qDebug()<<*depthMapFBO;

}

void Widget::resizeGL(int w, int h)
{

    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    Projection.setToIdentity();
    Projection.perspective(45,(GLfloat)w/(GLfloat)h,0.1,10000);
    //    Projection.ortho(-(GLfloat)w,(GLfloat)w,-(GLfloat)h,(GLfloat)h,0.1,10000);
}

void Widget::paintGL()
{

    int h = 0;
    QVector<QVector3D> v,vn;
    QVector<QVector2D> vt;
    v.push_back(QVector3D(-1000,h,-1000));
    v.push_back(QVector3D(1000,h,-1000));
    v.push_back(QVector3D(1000,h,1000));

    v.push_back(QVector3D(-1000,h,-1000));
    v.push_back(QVector3D(1000,h,1000));
    v.push_back(QVector3D(-1000,h,1000));
    vt.push_back(QVector2D(0.0,0.0));
    vt.push_back(QVector2D(1.0,0.0));
    vt.push_back(QVector2D(1.0,1.0));
    vt.push_back(QVector2D(0.0,0.0));
    vt.push_back(QVector2D(1.0,1.0));
    vt.push_back(QVector2D(0.0,1.0));
    for( int i = 0; i< 6; i++)
        vn.push_back(QVector3D(0,1,0));
    //    gen(d)
    //     gen(depthMapFBO,depthMap,1024,1024);
    glViewport(0, 0, 1024,1024);
    glBindFramebuffer(GL_FRAMEBUFFER, *depthMapFBO);
    qDebug()<<this->defaultFramebufferObject();
    //    if(core->glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    //        qDebug()<<"depthBuffer ok";
    glClear(GL_DEPTH_BUFFER_BIT);


    Model.setToIdentity();
    QMatrix4x4 lightView;
    lightView.lookAt(QVector3D(-300,800,-300),QVector3D(0,0,0),QVector3D(0,1,0));
    QMatrix4x4 lightProjection;
    lightProjection.ortho(-700.0f, 1300.0f, -900.0f, 1900,-700,1700);
    QMatrix4x4 lightSpaceMatrix = lightProjection * lightView;

    depthShader->link();
    depthShader->bind();
    depthShader->setUniformValue("lightSpaceMatrix",lightSpaceMatrix);
    Model.translate(curPos.x(),curPos.y(),curPos.z());
    depthShader->setUniformValue("model",Model);

    depthShader->bindAttributeLocation("position",0);
    depthShader->enableAttributeArray(0);
    depthShader->setAttributeArray(0,m_vertexs.data());
    glDrawArrays(GL_TRIANGLES,0,m_vertexs.size());
    glDrawArrays(GL_TRIANGLES,0,v.size());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0,this->width(),this->height());

    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    program->link();
    program->bind();
    Model.setToIdentity();
    View.setToIdentity();
    float eyex=dist*cos(vertical/180.0*PI)*sin(horizontal/180.0*PI);
    float eyey=dist*sin(vertical/180.0*PI);
    float eyez=dist*cos(vertical/180.0*PI)*cos(horizontal/180.0*PI);

    View.lookAt(QVector3D(eyex,eyey,eyez),QVector3D(0,0,0),QVector3D(0,1,0));
    //    View.lookAt(QVector3D(-300,800,-300),QVector3D(0,0,0),QVector3D(0,1,0));
    //    Projection.setToIdentity();

    //    Projection.ortho(-700.0f, 1300.0f, -900.0f, 1900,-700,1700);

    Model.translate(curPos.x(),curPos.y(),curPos.z());
    program->setUniformValue("m_projection",Projection);

    program->setUniformValue("m_model",Model);
    program->setUniformValue("m_view",View);
    int x = eyex,y = eyey,z = eyez;
    QVector3D viewPos(x, y,z);
    program->setUniformValue("viewPos",viewPos);
    program->setUniformValue("lPos",QVector3D(-300,800,-300));
    program->setUniformValue("lightSpaceMatrix",lightSpaceMatrix);

    program->setUniformValue("texture0",0);
    program->setUniformValue("shadowMap",1);
    //    texture0->bind(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,*tex0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,*depthMap);

    drawBall(curPos);
    program->link();
    program->bind();

    Model.setToIdentity();
    program->setUniformValue("m_projection",Projection);
    program->setUniformValue("m_model",Model);
    program->setUniformValue("m_view",View);
    program->setUniformValue("lightSpaceMatrix",lightSpaceMatrix);

    program->setUniformValue("texture0",0);
    program->setUniformValue("shadowMap",1);
    //    texture0->bind(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,*tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,*depthMap);

    program->setUniformValue("viewPos",viewPos);
    //    program->setUniformValue("lPos",QVector3D(5000,5000,5000));
    program->setUniformValue("lPos",QVector3D(-300,800,-300));
    //    program->setUniformValue("texture0",*depthMap);
    //    texture1->bind(*depthMap);
    //    texture1->bind(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,*tex1);



    program->bindAttributeLocation("a_position",0);
    program->enableAttributeArray(0);
    program->setAttributeArray(0,v.data());

    program->bindAttributeLocation("a_texcoord",1);
    program->enableAttributeArray(1);
    program->setAttributeArray(1,vt.data());

    program->bindAttributeLocation("a_normal",2);
    program->enableAttributeArray(2);
    program->setAttributeArray(2,vn.data());

    glDrawArrays(GL_TRIANGLES,0,v.size());


}

void Widget::BuildBall(int R, int split_v, int split_h,QVector3D positon)
{
    m_vertexs.clear();
    m_texcoords.clear();
    m_normals.clear();
    float dx = positon.x(),dy=positon.y(),dz=positon.z();
    QVector<QVector3D> v;
    QVector<QVector3D> vn;
    QVector<QVector3D> vt;


    QVector<Face> faces;
    QVector<QVector<int> > tmp;//保存每个顶点 相关联的面的索引
    QVector<QVector3D> tmp_normal;
    //int R = 50;//球体半径

    for( int i =0; i<= split_v; i++)
    {
        float r1 = R*sin(i*180.0/split_v *PI/180);
        float h1 = R*cos(i*(180.0/split_v) *(PI/180.0));

        for( int k = 0; k<= split_h; k++)
        {
            QVector3D v1(r1*cos( k*360.0/split_h*PI/180 )+dx,h1+dy, r1*sin( k*360.0/split_h*PI/180 )+dz);
            v.push_back(v1);
            vt.push_back(QVector3D( k*1.0/split_h,i*1.0/split_v,0) );

            //qDebug()<<v1; 法线数组在后面计算
        }
    }
    tmp.resize(v.size());
    for( int i =0; i<split_v; i++)
    {
        for( int k = 0; k< split_h; k++)
        {
            Face face;
            int index1,index2,index3,index4;

            index1 = i*(split_h+1) + k;
            index2 = i*(split_h+1) + (k +1)%split_h;
            index3 = index1+ split_h+1;
            index4 = index2+ split_h+1;

            face.v[0] = index1;//顶点索引
            face.v[1] = index2;
            face.v[2] = index4;
            face.t[0] = index1;//纹理索引
            face.t[1] = index2;
            face.t[2] = index4;
            face.n[0] = index1;//法线索引
            face.n[1] = index2;
            face.n[2] = index4;
            faces.push_back(face);
            face.v[0] = index1;
            face.v[1] = index4;
            face.v[2] = index3;
            face.t[0] = index1;
            face.t[1] = index4;
            face.t[2] = index3;
            face.n[0] = index1;
            face.n[1] = index4;
            face.n[2] = index3;
            faces.push_back(face);
        }
    }
    for( int i =0; i< faces.size(); i++)
    {
        Face face = faces[i];
        for( int k = 0; k< 3; k++)
        {
            //face.v[k]顶点的索引，tmp是面的索引，表达的含义是在点face.v[k]的相连面数组中加入第i面
            tmp[face.v[k] ].push_back(i);
        }
        //qDebug()<<face.v[0]<<" "<<face.v[1]<<" "<<face.v[2]<<" ";
        // 计算每个面的法线
        QVector3D E1 = v[face.v[1]] - v[face.v[0]];   // Edge 1
        QVector3D E2 = v[face.v[2]] - v[face.v[0]];   // Edge 1

        QVector3D normal = cross(E1,E2);
        normal.normalize();
        tmp_normal.push_back(normal);

    }

    //法线平均
    for( int i = 0; i< tmp.size(); i++)
    {

        QVector3D tn;
        for( int j = 0; j< tmp[i].size(); j++)
        {
            tn+= tmp_normal[tmp[i][j]];
        }

        tn.normalize();

        vn.push_back(tn);
    }
    //一个面对应三个点
    for( int i = 0; i<faces.size();i++)
    {
        for( int k = 0; k < 3;k++)
        {
            m_vertexs.push_back( v[faces[i].v[k]]);//把每一个面从索引转换为具体坐标
            m_normals.push_back(  vn[faces[i].n[k]]);
            m_texcoords.push_back(vt[faces[i].t[k]]);


        }
    }
    free(vertexs);
    free(normals);
    free(texcoords);
    vertexs = new GLdouble[m_vertexs.size()*3];
    normals = new GLdouble[m_normals.size()*3];
    texcoords = new GLdouble[m_texcoords.size()*3];
    for(int i= 0; i< m_vertexs.size();i++){
        vertexs[i*3+0] = m_vertexs[i].x();
        vertexs[i*3+1] = m_vertexs[i].y();
        vertexs[i*3+2] = m_vertexs[i].z();
    }
    for(int i= 0; i< m_normals.size();i++){
        normals[i*3+0] = m_normals[i].x();
        normals[i*3+1] = m_normals[i].y();
        normals[i*3+2] = m_normals[i].z();
    }

    for(int i = 0; i< m_texcoords.size(); i++)
    {
        texcoords[i*3+0] =m_texcoords[i].x();
        texcoords[i*3+1] =m_texcoords[i].y();
        texcoords[i*3+2] =m_texcoords[i].z();
    }
    // Texture::loadTexture(":/football.jpg",tex0);

}

void Widget::drawBall(QVector3D position)
{

    program->bindAttributeLocation("a_position",0);
    program->enableAttributeArray(0);
    program->setAttributeArray(0,m_vertexs.data());

    program->bindAttributeLocation("a_texcoord",1);
    program->enableAttributeArray(1);
    program->setAttributeArray(1,m_texcoords.data());

    program->bindAttributeLocation("a_normal",2);
    program->enableAttributeArray(2);
    program->setAttributeArray(2,m_normals.data());

    glDrawArrays(GL_TRIANGLES,0,m_vertexs.size());

}

void Widget::setRadius(float r)
{
    radius  =r;
    BuildBall(int(r),30,30);
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    m_last = e->pos();
    //    if(e->buttons() & Qt::RightButton)
    //    {
    //        fps->start(10);
    //        curPos.setY(500);
    //        time = 0;
    //        v0=0;
    //        max_height = curPos.y();
    //        lossrate = 0.4;
    //    }
}
void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        QPoint m_now = e->pos();
        float disx = m_now.x() - m_last.x();
        float disy = m_now.y() - m_last.y();
        horizontal -= disx/25;
        vertical += disy/25;

        m_last = m_now;

        update();
    }

}
void Widget::wheelEvent(QWheelEvent *e){
    if(e->delta() >0){
        if(dist-10 <=30)
            return;
        dist-=10;
    }
    else{
        dist+=10;
    }
    update();
    //this->updateScane();
}

void Widget::updateScane()
{
    cur_v = v0 -9.8*time;
    float delta_h =cur_v*0.3;//v0*time -0.5*0.098*time*time;
    time +=0.3;
    if(curPos.y()+delta_h<=radius){
        v0 = -(cur_v+9.8*0.3)*(lossrate);
        //lossrate-=0.05;
        if(fabs(v0)<0.3){
            fps->stop();
            return;
        }
        time = 0;

    }else
    {
        curPos.setY(curPos.y()+delta_h);
    }
    qDebug()<<v0;
    update();
}

void Widget::setV0(float value)
{
    v0 = value;
}

void Widget::setTime()
{
    time = 0;
}

void Widget::RenderCube()
{
//    GLuint cubeVAO = 0;
//    GLuint cubeVBO = 0;
//    // Initialize (if necessary)
//    if (cubeVAO == 0)
//    {
//        GLfloat vertices[] = {
//            // Back face
//            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
//            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
//            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
//            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
//            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
//            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
//            // Front face
//            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
//            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
//            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
//            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
//            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
//            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
//            // Left face
//            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
//            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
//            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
//            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
//            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
//            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
//            // Right face
//            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
//            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
//            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
//            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
//            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
//            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
//            // Bottom face
//            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
//            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
//            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
//            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
//            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
//            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
//            // Top face
//            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
//            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
//            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
//            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
//            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
//            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
//        };

//#define VERTEX_ATTRIBUTE_BRANCH 0
//#define NORMAL_ATTRIBUTE_BRANCH 1
//#define TEXTURE_ATTRIBUTE_BRANCH 2


//    program->enableAttributeArray(VERTEX_ATTRIBUTE_BRANCH);
//    program->enableAttributeArray(NORMAL_ATTRIBUTE_BRANCH);
//    program->enableAttributeArray(TEXTURE_ATTRIBUTE_BRANCH);
//    program->setAttributeBuffer(VERTEX_ATTRIBUTE_BRANCH,   GL_FLOAT, 0,                3,8*sizeof(GLfloat));
//    program->setAttributeBuffer(NORMAL_ATTRIBUTE_BRANCH,   GL_FLOAT, 3*sizeof(GLfloat),3,8*sizeof(GLfloat));
//    program->setAttributeBuffer(TEXTURE_ATTRIBUTE_BRANCH,  GL_FLOAT, 6*sizeof(GLfloat),2,8*sizeof(GLfloat));

////        glGenVertexArrays(1, &cubeVAO);
////        glGenBuffers(1, &cubeVBO);
////        // Fill buffer
////        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
////        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
////        // Link vertex attributes
////        glBindVertexArray(cubeVAO);
////        glEnableVertexAttribArray(0);
////        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
////        glEnableVertexAttribArray(1);
////        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
////        glEnableVertexAttribArray(2);
////        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
////        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//    }

//    // Render Cube
////    glBindVertexArray(cubeVAO);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
//    glBindVertexArray(0);


}

void Widget::genDepthMap(GLuint *&depthMapFBO, GLuint *&depthMap, GLuint SHADOW_WIDTH, GLuint SHADOW_HEIGHT)
{
    glGenFramebuffers(1, depthMapFBO); //生成一个帧缓冲对象

    glGenTextures(1, depthMap);
    glBindTexture(GL_TEXTURE_2D, *depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //把生成的深度纹理作为帧缓冲的深度缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, *depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthMap, 0);
    //设置不适用任何颜色
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::setLossrate(float value)
{
    lossrate = value;
}

void Widget::setHeight(int value)
{
    ballHeight = value;
    curPos.setY(ballHeight);
}

QVector3D Widget::cross(QVector3D a, QVector3D b)
{
    float x = a.y()*b.z() - a.z()*b.y();
    float y = a.z()*b.x() - a.x()*b.z();
    float z = a.x()*b.y() - a.y()*b.x();
    return  QVector3D(x,y,z);
}

void Widget::makeshader()
{
    static const QString vertexFile = ":/vshader.vert";
    static const QString fragmentFile = ":/fshader.frag";
//    static const QString geometryFile ="C:/Users/WK/Documents/qt/shader1/gshader.geom";
    program->addShaderFromSourceFile(QOpenGLShader::Vertex,vertexFile);
    program->addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentFile);
    //program->addShaderFromSourceFile(QGLShader::Geometry,geometryFile);
    /*  program->link();
    program->bind();*/ //绑定到当前opengl的上下文
    depthShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/vdepthshader.vert");
    depthShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/fdepthshader.frag");

    qDebug()<<"makeshader ok";

}

void Widget::init()
{
    fps->start(10);
    curPos.setY(500);
    time = 0;
    v0=0;
    max_height = curPos.y();
    lossrate = 0.4;

}

void Widget::stop()
{
    fps->stop();
}

void Widget::start(int fps)
{

    this->fps->start(fps);
}


