#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include <QCoreApplication>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
	{
	QApplication app(argc, argv);
	app.setFont(QFont("DejaVu", 8));

	QPixmap pixmap(":images/logo.png");
	QSplashScreen splash(pixmap);
	app.setStyle(new QPlastiqueStyle());
	splash.show();
	splash.showMessage("Welcome To Konfetka",Qt::AlignRight | Qt::AlignBottom,Qt::black );

	QCoreApplication::setApplicationName("Konfetka");
	app.setWindowIcon(QIcon(":images/logomini"));
	
	MainWindow mainwindow(&app,0,Qt::FramelessWindowHint);
	mainwindow.show();
	splash.finish(&mainwindow);
	return app.exec();
	}

