#ifndef OPTIONS_H
#define OPTIONS_H
#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
#include <QTabWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QSettings>
//PLIST	OPTIONS headers BEGIN
#include <QShortcut>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStringList>
//and END.

class GeneralOptions;
class MlibOptions;
class PlistOptions;
class Options:public QWidget
	{	Q_OBJECT
	private:
		QGridLayout* layout;
		QTabWidget * optionTabs;
		GeneralOptions * gen;
		MlibOptions * mlib;
		PlistOptions * plist;
		QPushButton * okButton;
		QPushButton * clearButton;
	public:
		Options(QWidget * parent=0,Qt::WindowFlags f=0);
		~Options();
	public slots:
		void saveAll();
		void resetAll();
		void emitAll();
	signals:
		void stayOnTop(bool);
		void reflectImage(bool);
		void newVis();
		void updatePlaylistHeaders(QList<QString>);
		void doubleClick(bool);
		void searchWhat(int);
	};
class GeneralOptions:public QWidget 
	{	Q_OBJECT
	private:
		Options * parentOptions;
		QCheckBox * reflectImage;
		QCheckBox * staysOnTop;
		QCheckBox * autoStart;
		QGroupBox * visOptions;
		QSpinBox * visNumBars;	QLabel * visNumBarsLabel;
		QSpinBox * visFps;	QLabel * visFpsLabel;
		QGridLayout * visLayout;

		QGridLayout* layout;
	public:
		GeneralOptions(Options* option,QWidget * parent=0,Qt::WindowFlags f=0);
		~GeneralOptions();
		void saveChanges();
		void reset();
		void emitSignals();
	signals:
		void sigStayOnTop(bool);
		void sigReflectImage(bool);
		void sigNewVis();
	};

class MlibOptions:public QWidget 
	{	Q_OBJECT
	private:
		QCheckBox * doubleClick;
		QLabel * doubleClickLabel;
		QComboBox * searchWhat;
		QLabel * searchWhatLabel;
		QGridLayout * layout;
	public:
		MlibOptions(Options* option=0,QWidget * parent=0,Qt::WindowFlags f=0);
		~MlibOptions();
		void saveChanges();
		void reset();
		void emitSignals();
	signals:
		void sigDoubleClick(bool);
		void sigSearchWhat(int);
	};

///!!VARIABLE SHOULD BE SAVED AS "konfetka/playlistValues"
///!!updatePlaylistHeaders needs to be connected to Playlist::setHeader() with data from getHeaders()
//Modifications:
//BEGIN
class PlistOptions:public QWidget 
	{	Q_OBJECT
	private:
		QGridLayout * layout;
		QListWidget * headers;
		QShortcut * del;
		QComboBox * addItem;
		QPushButton * addButton;
	public:
		PlistOptions(Options* option=0,QWidget * parent=0);
		void saveChanges();
		void reset();
		QList<QString> getHeaders(); //Alternatively, "getTheShit()"
		void emitSignals();
	private slots:
		void remove();
		void add();
		void editable(int npos);
	signals:
		void sigUpdatePlaylistHeaders(QList<QString>);
	};
//END

#endif
