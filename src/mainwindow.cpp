#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "streamdisplay.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    focuesdDisplay = NULL;
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            StreamDisplay* streamDisplay = new StreamDisplay(ui->displayWidget);
            ui->gridLayout->addWidget(streamDisplay, i, j);
            connect(streamDisplay, &StreamDisplay::focusIn, this, [this](StreamDisplay* widget) {
                focuesdDisplay = widget;
            });
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
