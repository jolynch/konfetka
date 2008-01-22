#ifndef OPTIONS_CPP
#define OPTIONS_CPP
#include "options.h"
#include <iostream>

Options::Options(DataBackend * c, QWidget * parent,Qt::WindowFlags f):LayoutPanel(parent,f) {
	conn = c;
	layout = new QGridLayout();
	tab = new QTabWidget();
	yesNo = new QGroupBox();
	QHBoxLayout * temp = new QHBoxLayout();
	QPushButton * save = new QPushButton("Save");
	QPushButton * cancel = new QPushButton("Cancel");
	QPushButton * saveAll = new QPushButton("Save All");
	connect(save,SIGNAL(clicked()),this,SLOT(sendSettings()));
	connect(saveAll,SIGNAL(clicked()),this,SLOT(sendAllSettings()));
	temp->addWidget(cancel);
	temp->addWidget(save);
	temp->addWidget(saveAll);
	yesNo->setLayout(temp);
	

	constructOptions();

	layout->addWidget(tab,0,0);
	layout->addWidget(yesNo,1,0);
	setLayout(layout);	
	
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(updateGui(QString,QVariant)));

}

Options::~Options() {
	delete layout;
}

void Options::updateGui(QString name,QVariant value) {
	if(name == "konfetka/playlistValues") {
		QStringList temp = value.toStringList();
		plistHeaders->setList(temp);
	}
	else if(name == "konfetka/stayOnTop") {
		if(value.toBool())
		stayOnTop->setCheckState(Qt::Checked); 
	}
	else if(name == "konfetka/autoStartXMMS2") {
		if(value.toBool())
		autoStart->setCheckState(Qt::Checked);
	}
	else if(name == "konfetka/albumArtReflection") {
		if(value.toBool())
		albArtReflection->setCheckState(Qt::Checked);
	}
	else if(name == "konfetka/visFps") {
		visFps->setValue(value.toInt());
	}
	else if(name == "konfetka/visNumBars") {
		visNumBars->setValue(value.toInt());
	}
	else if(name == "konfetka/language") {
		language->setCurrentIndex(language->findText(value.toString()));
	}
}

void Options::setLayoutSide(bool right_side){//true=right, false=left
return;
}

//Sets up all the widgets and such
void Options::constructOptions() {
	//General Options
	genOpt = new QWidget();
	QGridLayout * genLayout = new QGridLayout();
	visFps = new QSpinBox();
	visFps->setRange(0,100);
	visFps->setValue(10);
	visFpsLabel = new QLabel("Frames per Second");	
	visFpsLabel->setToolTip("Number of times to refresh the Visualization per second");
	visNumBars = new QSpinBox();
	visNumBars->setRange(0,50);
	visNumBars->setValue(50);
	visNumBarsLabel = new QLabel("Number of Bars");
	visNumBars->setToolTip("Number of bars to represent the visualization data (more than recommended may lead to odd behavior)");
	
	QGroupBox * visOptions = new QGroupBox("Visualization Options");
	visOptions->setCheckable(true);
	QGridLayout * visLayout = new QGridLayout();
	visLayout->addWidget(visNumBars,0,0,1,1);
	visLayout->addWidget(visNumBarsLabel,0,1,1,1);
	visLayout->addWidget(visFps,1,0,1,1);
	visLayout->addWidget(visFpsLabel,1,1,1,1);
	visOptions->setLayout(visLayout);

	autoStart = new QCheckBox("Autostart XMMS2?");
	albArtReflection = new QCheckBox("Album Art Reflection");
	stayOnTop = new QCheckBox("Stays On Top");
	language = new QComboBox();
	QList<QString> languages;
	languages << "English" << "Russian";
	language->addItems(languages);

	genLayout->addWidget(stayOnTop,0,0,1,1);
	genLayout->addWidget(albArtReflection,1,0,1,1);
	genLayout->addWidget(autoStart,2,0,1,1);
	genLayout->addWidget(visOptions,0,1,2,1);
	genLayout->addWidget(language,2,1,1,1);
	genOpt->setLayout(genLayout);
	tab->addTab(genOpt,"General");
	//End General

	//Playlist Options
	plistOpt = new QWidget();
	plistHeaders = new ListEditor(conn,"konfetka/playlistValues");
	QGridLayout * plistGrid = new QGridLayout();
	plistGrid->addWidget(plistHeaders);
	plistOpt->setLayout(plistGrid);
	tab->addTab(plistOpt,"Playlist");
	//End Playlist

	//MediaLib Options
	mlibOpt = new QWidget();
	tab->addTab(mlibOpt,"MediaLib");
	//End MediaLib

	//Collections Options
	collOpt = new QWidget();
	collImportSortOrder = new ListEditor(conn,"konfetka/collectImportSortOrder");
	QGridLayout * collGrid = new QGridLayout();
	collGrid->addWidget(collImportSortOrder);
	collOpt->setLayout(collGrid);
	tab->addTab(collOpt,"Collections");
	//End Collections
}

void Options::sendSettings(bool all) {
	int which = tab->currentIndex();
	/*
	0 = General, 1 = Playlist, 2 = MediaLib, 3 = Collections
	*/
	if(which == 0 || all) {
	std::cout<<"sending gen"<<std::endl;
	conn->changeAndSaveQSettings("konfetka/stayOnTop",QVariant(stayOnTop->checkState() == Qt::Checked));
	conn->changeAndSaveQSettings("konfetka/autoStartXMMS2",QVariant(autoStart->checkState() == Qt::Checked));
	conn->changeAndSaveQSettings("konfetka/albumArtReflection",QVariant(albArtReflection->checkState() == Qt::Checked));
	conn->changeAndSaveQSettings("konfetka/visFps",QVariant(visFps->value()));
	conn->changeAndSaveQSettings("konfetka/visNumBars",QVariant(visNumBars->value()));
	conn->changeAndSaveQSettings("konfetka/language",QVariant(language->currentText()));
	}
	if (which == 1 || all) {
	std::cout<<"sending plist"<<std::endl;
	plistHeaders->saveChanges();
	}
	if (which == 2 || all) {
	std::cout<<"sending mlib"<<std::endl;
	}
	if (which == 3 || all) {
	std::cout<<"sending coll"<<std::endl;	
	collImportSortOrder->saveChanges();
	}
}

void Options::sendAllSettings() {
std::cout<<"sending all"<<std::endl;
sendSettings(true);
}

/**************************************************************************************************************/
// List Editor

ListEditor::ListEditor(DataBackend * c,QString property,QWidget * parent):QWidget(parent) {
	conn = c;
	prop = property;
	layout=new QGridLayout();
	list=new QListWidget();
	QSettings s;
	QList<QString> tmp;
	tmp=s.value(prop).toStringList();
	if(tmp.isEmpty())
		tmp<<"Title"<<"Artist"<<"Album"<<"Time";
	list->addItems(tmp);
	list->setDragEnabled(true);
	list->setAcceptDrops(true);
	layout->addWidget(list,0,0,1,2);
	addItem=new QComboBox();
	QStringList labels;
	labels<<"Custom: edit with property"<<"Title"<<"Artist"<<"Album"<<"Time"<<"Filename"<<"Track"<<"Genre";
	addItem->addItems(labels);
	addItem->setCurrentIndex(0);
	addItem->setEditable(true);
	layout->addWidget(addItem,1,0);
	addButton=new QPushButton();
	addButton->setIcon(QIcon(":images/plus.png"));
	addButton->setToolTip("Add new property");
	layout->addWidget(addButton,1,1);
	setLayout(layout);
	connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
	connect(addItem, SIGNAL(currentIndexChanged(int)), this, SLOT(editable(int)));
}

//This simply adds whatever is in the combobox to the list
void ListEditor::add() {
	list->addItem(addItem->currentText());
}

//Allows the custom tags and such
void ListEditor::editable(int npos) {
	if(npos==0)
		addItem->setEditable(true);
	else
		addItem->setEditable(false);
}

//Saves the lists of tags under the constructed property
void ListEditor::saveChanges() {
	QList<QString> toStore = getList();
	conn->changeAndSaveQSettings(prop,QVariant(toStore));
}

/***    Might add support for this in the future    ***/
// void ListEditor::reset() {
// 	list->clear();
// 	QStringList labels;
// 	labels<<"Title"<<"Artist"<<"Album"<<"Time";
// 	list->addItems(labels);
// } 
/*****************************************************/

//Easily access the list of tags in a QList
QList<QString> ListEditor::getList() {
	QList<QString> ret;
	for(int i=0;i<list->count();i++) {
		ret.append(list->item(i)->text());	
	}
	return ret;
}

//Allows for delete (or any other keypress)
void ListEditor::keyPressEvent(QKeyEvent * event) {
	if(event->key() == (Qt::Key_Delete))
	removeSelected();
	else
	event->ignore();
}

//Removes all selected items (use with keypress)
void ListEditor::removeSelected() {
	QList<QListWidgetItem *> sel=list->selectedItems();
	for(int i=0; i<sel.size(); i++)
		delete list->takeItem(list->row(sel[i]));
}	

//Basically for initiating the list
void ListEditor::setList(QList<QString> newList) {
	list->clear();
	list->addItems(QStringList(newList));
}

/**********************************************************************************************/	
//End List Editor


#endif
