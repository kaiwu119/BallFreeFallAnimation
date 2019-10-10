#include "wind.h"
#include "ui_wind.h"

wind::wind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wind)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("自由落体演示"));
    QDesktopWidget* desktopWidget = QApplication::desktop();

    QRect screenRect = desktopWidget->screenGeometry();
    int x = screenRect.width();
    int y = screenRect.height();
    this->setGeometry(100,100,x-300,y-300);
    int w = this->width();
    int h = this->height();
    ui->groupBox->setGeometry(w/8*7,h/4,w/8,h/3);


     glwidget = new Widget(this);
     glwidget->setGeometry(0,0,w/8*7,h);
//     glwidget->initializeOpenGLFunctions();
     ui->spinBox->setRange(1,5);
     QSurfaceFormat format;
     format.setVersion(3,3);
     format.setProfile( QSurfaceFormat::CoreProfile);
     format.setDepthBufferSize(24);
     QSurfaceFormat::setDefaultFormat(format);

}

wind::~wind()
{
    delete ui;

}

void wind::on_pushButton_clicked()
{
int fps =  ui->spinBox->value();
//float height = ui->lineEdit->text().toFloat();
//float v = ui->lineEdit_2->text().toFloat();
//float lose = ui->lineEdit_3->text().toFloat();
//float r = ui->lineEdit_4->text().toFloat();


//glwidget->setRadius(r);
//glwidget->setHeight(height);
//glwidget->setLossrate(lose);
//glwidget->setV0(v);
  glwidget->start(60-fps*10);

}

void wind::on_pushButton_2_clicked()
{
 glwidget->stop();
}

void wind::on_pushButton_3_clicked()
{
   int fps =  ui->spinBox->value();
 glwidget->start(60-fps*10);
}

void wind::on_pushButton_4_clicked()
{

    float height = ui->lineEdit->text().toFloat();
    float v = ui->lineEdit_2->text().toFloat();
    float lose = ui->lineEdit_3->text().toFloat();
    float r = ui->lineEdit_4->text().toFloat();


    glwidget->setRadius(r);
    glwidget->setHeight(height);
    glwidget->setLossrate(lose);
    glwidget->setV0(v);
    glwidget->setTime();
    glwidget->stop();
    glwidget->update();
}
