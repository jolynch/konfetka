#ifndef OPTIONS_CPP
#define OPTIONS_CPP
#include "options.h"
#include <iostream>

Options::Options(QWidget * parent,Qt::WindowFlags f):QWidget(parent,f) {
	optionTabs = new QTabWidget();
	
	gen = new GeneralOptions(this);
	mlib = new MlibOptions();
	plist = new PlistOptions();

	optionTabs->addTab(gen,"General");
	optionTabs->addTab(mlib,"MediaLib");
	optionTabs->addTab(plist,"Playlist");

	okButton = new QPushButton("Submit Changes");
	clearButton = new QPushButton("Reset Defaults");

	layout=new QGridLayout();
	layout->setMargin(0);
	layout->addWidget(optionTabs,0,0,1,2);
	layout->addWidget(okButton,1,0);
	layout->addWidget(clearButton,1,1);

	this->setLayout(layout);

	QSettings s;
	if(!s.contains("staysOnTop"))
	resetAll();

	
	connect(okButton,SIGNAL(clicked()),this,SLOT(saveAll()));
	connect(clearButton,SIGNAL(clicked()),this,SLOT(resetAll()));
	connect(gen,SIGNAL(sigStayOnTop(bool)),this,SIGNAL(stayOnTop(bool)));
	connect(gen,SIGNAL(sigReflectImage(bool)),this,SIGNAL(reflectImage(bool)));
	connect(gen,SIGNAL(sigNewVis()),this,SIGNAL(newVis()));
	connect(plist,SIGNAL(sigUpdatePlaylistHeaders(QList<QString>)),this,SIGNAL(updatePlaylistHeaders(QList<QString>)));
	connect(mlib,SIGNAL(sigDoubleClick(bool)),this,SIGNAL(doubleClick(bool)));
	connect(mlib,SIGNAL(sigSearchWhat(int)),this,SIGNAL(searchWhat(int)));
	emitAll();
	}

Options::~Options() {
	delete layout;
	delete optionTabs;
	delete okButton;
	delete clearButton;
	}

void Options::saveAll() {
	gen->saveChanges();
	plist->saveChanges();
	mlib->saveChanges();
	emitAll();
}

void Options::resetAll() {
	switch(optionTabs->currentIndex()) {
	case 0:
	gen->reset();
	break;
	case 1:
	mlib->reset();
	break;
	case 2:
	plist->reset();
	}
}

void Options::emitAll() {
	gen->emitSignals();
	mlib->emitSignals();
	plist->emitSignals();
}

GeneralOptions::GeneralOptions(Options* option,QWidget * parent,Qt::WindowFlags f):QWidget(parent,f) {
	parentOptions = option;
	QSettings s;
	
	staysOnTop=new QCheckBox("Stay On Top of Other Windows");
 	if(s.contains("staysOnTop") && s.value("staysOnTop").toInt() == Qt::Checked )
 	staysOnTop->setCheckState(Qt::Checked);

	reflectImage = new QCheckBox("Album Art Reflection");
	if(s.contains("reflectImage") && s.value("reflectImage").toInt() == Qt::Checked )
	reflectImage->setCheckState(Qt::Checked);

	autoStart = new QCheckBox("Autostart xmms2");
	if(s.contains("autostart") && s.value("autostart").toBool())
	autoStart->setCheckState(Qt::Checked);

	visNumBars = new QSpinBox();
	visNumBarsLabel = new QLabel("Visualizer Bar Count");
	if(s.contains("visNumBars"))
	visNumBars->setValue(s.value("visNumBars").toInt());

	visFps = new QSpinBox();
	visFpsLabel = new QLabel("Visualizer FPS");
	visFps->setRange(0,50);
	if(s.contains("visFps"))
	visFps->setValue(s.value("visFps").toInt());

	visOptions = new QGroupBox();
	visLayout = new QGridLayout();
	visLayout->addWidget(visNumBars,0,0,1,1);
	visLayout->addWidget(visNumBarsLabel,0,1,1,1);
	visLayout->addWidget(visFps,1,0,1,1);
	visLayout->addWidget(visFpsLabel,1,1,1,1);
	visOptions->setLayout(visLayout);

	layout = new QGridLayout();
	layout->addWidget(staysOnTop,0,0,1,1);
	layout->addWidget(reflectImage,1,0,1,1);
	layout->addWidget(autoStart,2,0,1,1);
	layout->addWidget(visOptions,0,1,2,3);
	
	this->setLayout(layout);
}

GeneralOptions::~GeneralOptions() {
	delete staysOnTop;
	delete reflectImage;
	delete autoStart;
	delete visNumBars;
	delete visNumBarsLabel;
	delete visFps;
	delete visFpsLabel;
}

void GeneralOptions::saveChanges() {
	QSettings s;
	s.setValue("staysOnTop",QVariant(staysOnTop->checkState()));
	s.setValue("reflectImage",QVariant(reflectImage->checkState()));
	s.setValue("autostart",QVariant(reflectImage->checkState() == Qt::Checked));
	s.setValue("visNumBars",QVariant(visNumBars->value()));
	s.setValue("visFps",QVariant(visFps->value()));
}

void GeneralOptions::emitSignals() {
	emit sigStayOnTop(staysOnTop->checkState() == Qt::Checked);
	emit sigReflectImage(reflectImage->checkState() == Qt::Checked);
	emit sigNewVis();
}

void GeneralOptions::reset() {
	staysOnTop->setCheckState(Qt::Unchecked);
	reflectImage->setCheckState(Qt::Checked);
	autoStart->setCheckState(Qt::Checked);
	visNumBars->setValue(50);
	visFps->setValue(10);
	saveChanges();
}

MlibOptions::MlibOptions(Options* option,QWidget * parent,Qt::WindowFlags f):QWidget(parent,f) {
	QSettings s;
	doubleClick = new QCheckBox();
	if(s.contains("mlibDoubleClick") && s.value("mlibDoubleClick").toInt() == Qt::Checked)
	doubleClick->setCheckState(Qt::Checked); 
	doubleClickLabel = new QLabel("Double click adds item to playlist");

	searchWhat = new QComboBox();	
	searchWhat->addItem("All");
	searchWhat->addItem("Artist");
	searchWhat->addItem("Album");
	searchWhat->addItem("Title");
	if(s.contains("searchWhatIndex"))
	searchWhat->setCurrentIndex(s.value("searchWhatIndex").toInt());
	searchWhatLabel = new QLabel("Search :");
	layout = new QGridLayout();
	
	layout->addWidget(doubleClick,0,1);
	layout->addWidget(doubleClickLabel,0,0);
	layout->addWidget(searchWhatLabel,1,0);
	layout->addWidget(searchWhat,1,1);
	this->setLayout(layout);
}

MlibOptions::~MlibOptions() {
	delete doubleClick;
	delete doubleClickLabel;
	delete searchWhat;
	delete searchWhatLabel;
	delete layout;
}

void MlibOptions::saveChanges() {
	QSettings s;
	s.setValue("mlibDoubleClick",QVariant(doubleClick->checkState()));
	s.setValue("searchWhatIndex",QVariant(searchWhat->currentIndex()));
}

void MlibOptions::emitSignals() {
	emit sigDoubleClick(doubleClick->checkState() == Qt::Checked);
	emit sigSearchWhat(searchWhat->currentIndex());
}

void MlibOptions::reset() {
	doubleClick->setCheckState(Qt::Unchecked);
	searchWhat->setCurrentIndex(0);
}

//Modifications:
//BEGIN
PlistOptions::PlistOptions(Options* option,QWidget * parent):QWidget(parent)
	{
	layout=new QGridLayout();
	headers=new QListWidget();
	QSettings s;
	QList<QString> tmp;
	tmp=s.value("konfetka/playlistValues").toStringList();
	if(tmp.isEmpty())
		tmp<<"Title"<<"Artist"<<"Album"<<"Time";
	headers->addItems(tmp);
	headers->setDragEnabled(true);
	headers->setAcceptDrops(true);
	layout->addWidget(headers,0,0,1,2);
// 	del= new QShortcut(QKeySequence(Qt::Key_Delete),this,SLOT(remove()),SLOT(remove()));
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
	this->setLayout(layout);
	QObject::connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
	QObject::connect(addItem, SIGNAL(currentIndexChanged(int)), this, SLOT(editable(int)));
	}

QList<QString> PlistOptions::getHeaders()
	{
	QList<QString> final;
		for(int i=0;i<headers->count();i++) {
		final.append(headers->item(i)->text());
		}
	return final;
	}

void PlistOptions::remove()
	{
	QList<QListWidgetItem *> sel=headers->selectedItems();
	for(int i=0; i<sel.size(); i++)
		delete headers->takeItem(headers->row(sel[i]));
	}

void PlistOptions::add()
	{
	headers->addItem(addItem->currentText());
	}

void PlistOptions::editable(int npos)
	{
	if(npos==0)
		addItem->setEditable(true);
	else
		addItem->setEditable(false);
	}

void PlistOptions::saveChanges() {
	QList<QString> toStore = getHeaders();
	QSettings s;
	s.setValue("konfetka/playlistValues",QVariant(toStore));
}

void PlistOptions::reset()
	{
	headers->clear();
	QStringList labels;
	labels<<"Title"<<"Artist"<<"Album"<<"Time";
	headers->addItems(labels);
	}

void PlistOptions::emitSignals() {
	emit sigUpdatePlaylistHeaders(getHeaders());
}
//END

#endif
