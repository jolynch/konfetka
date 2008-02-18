/*
A basic Options editor for Konfetka
Main dev: Joe Lynch
*/

#ifndef OPTIONS_H
#define OPTIONS_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include "layoutpanel.h"
#include <QGridLayout>
#include <QTabWidget>
#include <QList>
#include <QString>
#include <QStringList>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QListWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QKeyEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLineEdit>

class ListEditor;
class Options:public LayoutPanel {
	Q_OBJECT
	private:
	DataBackend * conn;
	QGridLayout * layout;
	QTabWidget * tab;
	QGroupBox * yesNo;

	//Various Tabs
	/*Development Note: This is not the most portable way of doing this, it theoretically would be easier
	for me to use a class for each 'tab', i.e. a "General" class, etc... but 
	1) that's harder
	2) anyone who reads this should be able to make the necessary changes in the data members to provide
	the same kind of changes (if you can't, request that one of the devs does)
	*/

	//General
	QWidget * genOpt;

	QSpinBox * visFps;
	QLabel * visFpsLabel;
	QSpinBox * visNumBars;
	QLabel * visNumBarsLabel;
	QCheckBox * autoStart;
	QCheckBox * albArtReflection;
	QCheckBox * stayOnTop;
	QCheckBox * dblClickContextInfoPlay;
	QComboBox * language;

	//End General

	//Playlist
	QWidget * plistOpt;
	ListEditor * plistHeaders;
	//End Playlist

	//Media Library
	QWidget * mlibOpt;
		
	QCheckBox * doubleClick;
	ListEditor * searchTags;
	ListEditor * quickSearchTags;

	//End Media Library
	
	//Collections
	QWidget * collOpt;
	ListEditor * collImportSortOrder;
	//End Collections
	
	
	//XMMS2 
	QWidget *xmms2Opt;
	QTreeWidget *xmms2Tree;
	QLineEdit *xmms2Search;
	QPushButton *xmms2BtnSearch;
	void updateXmms2OptsForeach(const std::string&, const Xmms::Dict::Variant&);
	//End XMMS2

	void constructOptions();

	public:
	Options(DataBackend * c,QWidget * parent=0,Qt::WindowFlags f=0);
	~Options();
	void setLayoutSide(bool right_side);//true=right, false=left

	public slots:
	void sendSettings(bool all = false);
	void sendAllSettings();
	void updateGui(QString,QVariant);
	void updateXmms2Opts(const Xmms::Dict&);
	void xmms2TreeStartEdit(QTreeWidgetItem* item, int col);
	void filterXmms2Opts();
};

//Often lists need to be edited, for example the import order for collections, or labels of a song to show
class ListEditor:public QWidget {
	Q_OBJECT
	private:
	DataBackend * conn;
	QString prop;
	QGridLayout * layout;
	QListWidget * list;
	QComboBox * addItem;
	QPushButton * addButton;
	void removeSelected();
	
	public:
	ListEditor(DataBackend * c,QString property,QWidget * parent=0);
	void keyPressEvent(QKeyEvent * event);
	void saveChanges();
	void reset();
	QList<QString> getList();
	void setList(QList<QString>);	

	private slots:
	void add();
	void editable(int npos);
};

#endif
