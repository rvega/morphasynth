#pragma once

#include <QMainWindow>
#include <QApplication>
#include <QtWebKit>

class Audio;

class MainWindow : public QMainWindow {
   Q_OBJECT
   
   public:
      explicit MainWindow(const QApplication& app, QWidget* parent=0, Qt::WindowFlags flags=0);
      virtual ~MainWindow();

      void start();

   public slots:
      void stop();

   protected slots:
      void print(QString message);
      void htmlDidLoad();
      void connectToJS();

   private:
      MainWindow(const MainWindow&);
      MainWindow& operator=(const MainWindow&);

      const QApplication& app;
      QWebView webView;
      const int width;
      const int height;
};
