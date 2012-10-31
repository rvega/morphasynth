#include <iostream>
#include "MainWindow.hpp"

/**
 * Construction & destruction
 */
MainWindow::MainWindow(const QApplication& _app, QWidget * parent, Qt::WindowFlags flags) :
   QMainWindow(parent, flags),
   app(_app),
   webView(this),
   width(800),
   height(600)
{ 
   setMaximumSize(width, height);
   setMinimumSize(width, height);

   webView.load(QUrl("qrc:///html/index.html"));
   // webView.load(QUrl("http://wizard.ae.krakow.pl/~jb/localio.html"));
   webView.resize(width, height);
}
   
MainWindow::~MainWindow(){

}

void MainWindow::start(){
   show();
}

void MainWindow::stop(){
   std::cout << "Stop!" << "\n";
}
