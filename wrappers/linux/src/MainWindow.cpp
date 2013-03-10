#include <iostream>
#include "dirent.h"
#include <fstream>
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
}
   
MainWindow::~MainWindow(){

}

void MainWindow::start(){
   show();

   // The flow is like this:
   // 1. We tell the webview to load a document. It has a bunch of js code.
   // 2. HTML finishes loading and js sends us the htmlDidLoad signal
   // 3. We call the js function `main()` from C++.
   webView.load(QUrl("./res/html/index.html"));
   webView.resize(width, height);
   connectToJS();
   connect( webView.page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(connectToJS()) );
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
QString MainWindow::getPresets(){
   // List all files ending in .preset in the presets dir
   std::vector<std::string> filenames;
   DIR *dir = opendir("./res/pd/presets");
   struct dirent *entry;
   std::string filename;
   while(NULL != (entry = readdir(dir))){
      filename = std::string(entry->d_name);
      if(std::string::npos != filename.find(".preset", filename.length()-7)){
         filenames.push_back(filename);
      }
   }
   closedir(dir);

   // Open each file and assemble a json string
   std::string out = "[";
   std::string line;
   for(unsigned int i=0; i<filenames.size(); i++){
      out += "{";
      std::ifstream file("res/pd/presets/" + filename);
      if(file.is_open()){
         while(file.good()){
            getline(file, line);
            if(line.size() > 0){
               std::string value = line.substr(line.find_last_of(' ')+1, line.find_last_of(';')-line.find_last_of(' ')-1);
               std::string key = line.substr(0, line.find(' '));
               out += "\"" + key + "\": " + "\"" + value + "\",";
            }
         }
      }
      file.close();
      out = out.substr(0, out.size()-1);
      out += "},";
   }
   out = out.substr(0, out.size()-1);
   out += "]";

   return QString::fromStdString(out);
}

void MainWindow::print(QString message){
   std::cout << "JS: " << qPrintable(message) << "\n";
}

void MainWindow::htmlDidLoad(){
   QString js = "main(); null;";
   webView.page()->mainFrame()->evaluateJavaScript(js);
}
