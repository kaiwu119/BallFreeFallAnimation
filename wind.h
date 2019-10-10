#ifndef WIND_H
#define WIND_H

#include <QWidget>

#include <QDesktopWidget>
#include <QRect>
#include <QResizeEvent>
#include <QTimer>
#include <QButtonGroup>
#include <QSurfaceFormat>
#include <widget.h>
namespace Ui {
class wind;
}

class wind : public QWidget
{
    Q_OBJECT

public:
    explicit wind(QWidget *parent = 0);
    ~wind();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::wind *ui;
    Widget *glwidget;

};

#endif // WIND_H
