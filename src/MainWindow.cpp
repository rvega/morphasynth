#include <iostream>
#include "MainWindow.hpp"

/**
 * Construction & destruction
 */
MainWindow::MainWindow(const QApplication& _app, QWidget * parent, Qt::WindowFlags flags) :
   QMainWindow(parent, flags),
   app(_app),
   webView(this),
   width(490),
   height(670)
{ 
   setMaximumSize(width, height);
   setMinimumSize(width, height);

   // The flow is like this:
   // 1. We tell the webview to load a document. It has a bunch of js code.
   // 2. HTML finishes loading and js sends us the htmlDidLoad signal
   // 3. We call the `main()` function in js.
   webView.load(QUrl("qrc:///html/index.html"));
   webView.resize(width, height);
   connectToJS();
   connect( webView.page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(connectToJS()) );
}
   
MainWindow::~MainWindow(){

}

void MainWindow::start(){
   show();
}

void MainWindow::stop(){
   // std::cout << "Stop!" << "\n";
}

/**
 * Exposes this class to the javascript code.
 */
void MainWindow::connectToJS(){
   webView.page()->mainFrame()->addToJavaScriptWindowObject(QString("ContainerApp"), this);
}

/**
 * Methods that are visible from javascript
 */
void MainWindow::htmlDidLoad(){
   QString js = "main(); null;";
   webView.page()->mainFrame()->evaluateJavaScript(js);
}

void MainWindow::print(QString message){
   std::cout << qPrintable(message) << "\n";
}
