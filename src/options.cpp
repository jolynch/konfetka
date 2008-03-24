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
	connect(cancel,SIGNAL(clicked()),this,SLOT(emitSettings()));
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
	connect(conn,SIGNAL(configValList(Xmms::Dict)),this,SLOT(updateXmms2Opts(Xmms::Dict)));
	connect(conn,SIGNAL(configValChanged(Xmms::Dict)),this,SLOT(updateXmms2Opts(Xmms::Dict)));
}

Options::~Options() {
	delete layout;
}

void Options::emitSettings() {
	qDebug()<<"init";
	conn->emitInitialQSettings();
}

void Options::updateGui(QString name,QVariant value) {
	if(name == "konfetka/playlistValues") {
		QStringList temp = value.toStringList();
		plistHeaders->setList(temp);
	}
	else if(name == "konfetka/collectImportSortOrder") {
		QStringList temp = value.toStringList();
		collImportSortOrder->setList(temp);
	}
	else if(name == "konfetka/mlibComplexValues") {
		QStringList temp = value.toStringList();
		searchTags->setList(temp);
	}
	else if(name == "konfetka/mlibSearchValues") {
		QStringList temp = value.toStringList();
		quickSearchTags->setList(temp);
	}
	else if(name == "konfetka/stayOnTop") {
		if(value.toBool())
		stayOnTop->setCheckState(Qt::Checked);
		else
		stayOnTop->setCheckState(Qt::Unchecked);
	}
	else if(name == "konfetka/autoStartXMMS2") {
		if(value.toBool())
		autoStart->setCheckState(Qt::Checked);
		else
		autoStart->setCheckState(Qt::Unchecked);
	}
	else if(name == "konfetka/mlibDblClick") {
		if(value.toBool())
		doubleClick->setCheckState(Qt::Checked);
		else
		doubleClick->setCheckState(Qt::Unchecked);
	}
	else if(name == "konfetka/albumArtReflection") {
		if(value.toBool())
		albArtReflection->setCheckState(Qt::Checked);
		else
		albArtReflection->setCheckState(Qt::Unchecked);
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
	else if(name == "konfetka/contextInfoDoubleClickPlay") {
		if(value.toBool())
		dblClickContextInfoPlay->setCheckState(Qt::Checked);
		else
		dblClickContextInfoPlay->setCheckState(Qt::Unchecked);
		
	}
	else if(name.startsWith("konfetka/shortcut_")) {
// 		qDebug()<<name<<value.toString();
		QList<QTableWidgetItem*> matches = shortcutMatrix->findItems(name.remove("konfetka/shortcut_"),Qt::MatchExactly);
		for(int i=0;i<matches.size();i++) {
			if(matches[i]->column()==0) {
				shortcutMatrix->setItem(matches[i]->row(),1,new QTableWidgetItem(value.toString()));
				shortcutMatrix->item(matches[i]->row(),1)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			}
		}
// 		qDebug()<<matches.size();
	}
}

void Options::updateXmms2Opts(const Xmms::Dict &val) {
	val.each(boost::bind(&Options::updateXmms2OptsForeach, this, _1, _2));
	xmms2Tree->sortItems(0, Qt::AscendingOrder);
	xmms2Tree->resizeColumnToContents(0);
}
void Options::updateXmms2OptsForeach(const std::string &key, const Xmms::Dict::Variant &val) {
	//std::cout << key << " = " << val << std::endl;
	QString keyQStr = QString::fromStdString(key);
	QString valQStr = QString::fromStdString(boost::get<std::string>(val));
	QList<QTreeWidgetItem*> matchItems = xmms2Tree->findItems(keyQStr, Qt::MatchExactly);
	QTreeWidgetItem *curItem;
	if (matchItems.count() == 0) {
		curItem = new QTreeWidgetItem(xmms2Tree);
		curItem->setText(0, keyQStr);
		curItem->setFlags(Qt::ItemIsEnabled);
	} else {
		curItem = matchItems[0];
	}
	curItem->setText(1, valQStr);
}

void Options::filterXmms2Opts() {
	QList<QTreeWidgetItem*> foundList = xmms2Tree->findItems(xmms2Search->displayText(), Qt::MatchContains, 0);
	foundList += xmms2Tree->findItems(xmms2Search->displayText(), Qt::MatchContains, 1);
	for (int i=0; i<xmms2Tree->topLevelItemCount(); i++) {
		if (foundList.contains(xmms2Tree->topLevelItem(i)))
			xmms2Tree->topLevelItem(i)->setHidden(false);
		else
			xmms2Tree->topLevelItem(i)->setHidden(true);
	}
}

void Options::setLayoutSide(bool right_side){//true=right, false=left
return;
}

void Options::xmms2TreeStartEdit(QTreeWidgetItem* item, int col) {
	if (col == 0) {
		item->setFlags(Qt::ItemIsEnabled);
	}
	else if (col == 1) {
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		xmms2Tree->editItem(item, 1);
	}
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
	dblClickContextInfoPlay = new QCheckBox("Context Info Double Click Play");
	dblClickContextInfoPlay->setToolTip("When double clicking to append items to the playlsit in contextInfo, this option controls \
	whether or not to try playing those songs");
	dblClickContextInfoPlay->setCheckState(Qt::Checked);
	language = new QComboBox();
	QList<QString> languages;
	languages << "English" << "Russian";
	language->addItems(languages);

	genLayout->addWidget(stayOnTop,0,0,1,1);
	genLayout->addWidget(albArtReflection,1,0,1,1);
	genLayout->addWidget(autoStart,2,0,1,1);
	genLayout->addWidget(dblClickContextInfoPlay,3,0,1,1);
	genLayout->addWidget(visOptions,0,1,3,1);
	genLayout->addWidget(language,3,1,1,1);
	genOpt->setLayout(genLayout);
	tab->addTab(genOpt,"General");
	//End General

	//Playlist Options
	plistOpt = new QWidget();

	QGroupBox * playlistValuesBox = new QGroupBox("Playlist Headers");
	QGridLayout * playlistValuesLayout = new QGridLayout();

	plistHeaders = new ListEditor(conn,"konfetka/playlistValues");
	
	QGridLayout * plistGrid  = new QGridLayout();
	playlistValuesLayout->addWidget(plistHeaders);
	playlistValuesBox->setLayout(playlistValuesLayout);
	plistGrid->addWidget(playlistValuesBox);
	plistOpt->setLayout(plistGrid);
	tab->addTab(plistOpt,"Playlist");
	//End Playlist

	//MediaLib Options
	mlibOpt = new QWidget();
	
	QGridLayout * mlibGrid = new QGridLayout();
		
	doubleClick = new QCheckBox("Double Click");
	doubleClick->setToolTip("Double click on medialib items adds them to the playlist");
	
	QGroupBox * searchBox = new QGroupBox("Search Tags");
	QGridLayout * searchLayout = new QGridLayout();
	QList<QString> temp;
	searchTags = new ListEditor(conn,"konfetka/mlibComplexValues");
	temp = ((MlibData*)(conn->getDataBackendObject(DataBackend::MLIB)))->getStandardTags();
	searchTags->setList(temp);
	quickSearchTags = new ListEditor(conn,"konfetka/mlibSearchValues");
	temp.clear();
	temp<<"Artist"<<"Album"<<"Title"<<"Url"<<"ID";
	quickSearchTags->setList(temp);
	searchLayout->addWidget(new QLabel("Complex Search Tags"),0,0,1,1);
	searchLayout->addWidget(new QLabel("Quick Search Tags"),0,1,1,1);
	searchLayout->addWidget(searchTags,1,0,1,1);
	searchLayout->addWidget(quickSearchTags,1,1,1,1);
	searchBox->setLayout(searchLayout);
	
	mlibGrid->addWidget(doubleClick,0,0,1,2);
	mlibGrid->addWidget(searchBox,1,0,1,2);
	mlibOpt->setLayout(mlibGrid);
	
	tab->addTab(mlibOpt,"MediaLib");
	//End MediaLib

	//Collections Options
	collOpt = new QWidget();
	
	QGroupBox * collSortOrderBox = new QGroupBox("Collection Import Sort Order");
	QGridLayout * collSortOrderLayout = new QGridLayout();
	
	collImportSortOrder = new ListEditor(conn,"konfetka/collectImportSortOrder");
	
	QGridLayout * collGrid = new QGridLayout();
	collSortOrderLayout->addWidget(collImportSortOrder);
	collSortOrderBox->setLayout(collSortOrderLayout);
	collGrid->addWidget(collSortOrderBox);
	collOpt->setLayout(collGrid);
	tab->addTab(collOpt,"Collections");
	//End Collections
	
	//XMMS2 Options
	xmms2Opt = new QWidget();
	QGridLayout * xmms2Grid = new QGridLayout();
	
	xmms2Search = new QLineEdit();
	xmms2Grid->addWidget(xmms2Search, 0,0,1,1);
	connect(xmms2Search,SIGNAL(editingFinished()),this,SLOT(filterXmms2Opts()));
	
	QStringList xmms2TreeHeaders;
	xmms2Tree = new QTreeWidget();
	xmms2TreeHeaders << "Key" << "Value";
	xmms2Tree->setHeaderLabels(xmms2TreeHeaders);
	connect(xmms2Tree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(xmms2TreeStartEdit(QTreeWidgetItem*,int)));
	xmms2Grid->addWidget(xmms2Tree, 1,0,1,1);
	
	xmms2Opt->setLayout(xmms2Grid);
	tab->addTab(xmms2Opt,"XMMS2");
	//End XMMS2 Options

	//Shortcut Options
	shortcutOpt = new QWidget();
	QGridLayout * shortcutGrid = new QGridLayout();

	shortcutMatrix = new QTableWidget(4,2);
	shortcutGrid->addWidget(shortcutMatrix,0,0);
	QStringList hlabels;
	hlabels<<""<<""<<""<<"";
	shortcutMatrix->setVerticalHeaderLabels(hlabels);
	hlabels.clear(); hlabels<<"Action"<<"Shortcut (Double click to edit)";
	shortcutMatrix->setShowGrid(false);
	shortcutMatrix->setHorizontalHeaderLabels(hlabels);
	shortcutMatrix->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	shortcutMatrix->resizeColumnToContents(0);
	shortcutMatrix->setItem(0,0,new QTableWidgetItem(tr("Show-Hide")));
	shortcutMatrix->item(0,0)->setFlags(Qt::ItemIsEnabled);
	shortcutMatrix->setItem(0,1,new QTableWidgetItem(tr("Ctrl+`")));
	shortcutMatrix->item(0,1)->setFlags(Qt::ItemIsEnabled);

	shortcutMatrix->setItem(1,0,new QTableWidgetItem(tr("Play-Pause")));
	shortcutMatrix->item(1,0)->setFlags(Qt::ItemIsEnabled);
	shortcutMatrix->setItem(1,1,new QTableWidgetItem(tr("Ctrl+Shift+X")));
	shortcutMatrix->item(1,1)->setFlags(Qt::ItemIsEnabled);

	shortcutMatrix->setItem(2,0,new QTableWidgetItem(tr("Next song")));
	shortcutMatrix->item(2,0)->setFlags(Qt::ItemIsEnabled);
	shortcutMatrix->setItem(2,1,new QTableWidgetItem(tr("Ctrl+Shift+C")));
	shortcutMatrix->item(2,1)->setFlags(Qt::ItemIsEnabled);

	shortcutMatrix->setItem(3,0,new QTableWidgetItem(tr("Previous song")));
	shortcutMatrix->item(3,0)->setFlags(Qt::ItemIsEnabled);
	shortcutMatrix->setItem(3,1,new QTableWidgetItem(tr("Ctrl+Shift+Z")));
	shortcutMatrix->item(3,1)->setFlags(Qt::ItemIsEnabled);

	shortcutOpt->setLayout(shortcutGrid);
	tab->addTab(shortcutOpt,"Shortcuts");

	chooserDialog = new ShortcutChooser();
	connect(shortcutMatrix,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(editShortcut(QTableWidgetItem*)));
	//End Shortcut Options
}

void Options::editShortcut(QTableWidgetItem* item) {
	qDebug()<<"Clicked";
	chooserDialog->exec();
}

void Options::sendSettings(bool all) {
	int which = tab->currentIndex();
	/*
	0 = General, 1 = Playlist, 2 = MediaLib, 3 = Collections, 4 = XMMS2, 5 = Shortcuts
	*/
	if(which == 0 || all) {
	std::cout<<"sending gen"<<std::endl;
	conn->changeAndSaveQSettings("konfetka/stayOnTop",QVariant(stayOnTop->checkState() == Qt::Checked));
	conn->changeAndSaveQSettings("konfetka/autoStartXMMS2",QVariant(autoStart->checkState() == Qt::Checked));
	conn->changeAndSaveQSettings("konfetka/albumArtReflection",QVariant(albArtReflection->checkState() == Qt::Checked));
	conn->changeAndSaveQSettings("konfetka/visFps",QVariant(visFps->value()));
	conn->changeAndSaveQSettings("konfetka/visNumBars",QVariant(visNumBars->value()));
	conn->changeAndSaveQSettings("konfetka/language",QVariant(language->currentText()));
	conn->changeAndSaveQSettings("konfetka/contextInfoDoubleClickPlay",QVariant(dblClickContextInfoPlay->checkState() == Qt::Checked));
	}
	if (which == 1 || all) {
	std::cout<<"sending plist"<<std::endl;
	plistHeaders->saveChanges();
	}
	if (which == 2 || all) {
	std::cout<<"sending mlib"<<std::endl;
	searchTags->saveChanges();
	quickSearchTags->saveChanges();
	conn->changeAndSaveQSettings("konfetka/mlibDblClick",QVariant(doubleClick->checkState() == Qt::Checked));
	}
	if (which == 3 || all) {
	std::cout<<"sending coll"<<std::endl;	
	collImportSortOrder->saveChanges();
	}
	if(which == 4 || all) {
	std::cout<<"sending xmms2"<<std::endl;
	QList<QTreeWidgetItem*> items = xmms2Tree->findItems("*", Qt::MatchWildcard);
	for (int i=0; i<items.count(); i++)
		conn->changeAndSaveXMMS2Settings(items[i]->text(0).toStdString(), items[i]->text(1).toStdString());
	}
	if(which == 5 || all) {
	qDebug()<<"sending shortcuts";
	
	for(int i=0;i<shortcutMatrix->rowCount();i++) {
		conn->changeAndSaveQSettings(("konfetka/shortcut_"+shortcutMatrix->item(i,0)->text()),
					     (shortcutMatrix->item(i,1)->text()));
	}
	}
}

void Options::sendAllSettings() {
std::cout<<"sending all"<<std::endl;
sendSettings(true);
}



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
	labels=((MlibData*)(conn->getDataBackendObject(DataBackend::MLIB)))->getStandardTags();
	labels.prepend("Custom: edit with property");
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

/**************************************************************************************************************/
// ShortcutChooser
ShortcutChooser::ShortcutChooser(QWidget * parent,Qt::WindowFlags f):QDialog(parent,f) {
	currentCombo = "";
	shortcutLabel = new QLabel(currentCombo);

	done = new QPushButton("Done");
	connect(done,SIGNAL(clicked()),this,SLOT(accept()));

	QGridLayout * grid = new QGridLayout();
	grid->addWidget(shortcutLabel,0,0);
	grid->addWidget(done,1,0);
	setLayout(grid);
}

void ShortcutChooser::keyPressEvent(QKeyEvent * event) {
	qDebug()<<event->count();
}

void ShortcutChooser::keyReleaseEvent(QKeyEvent * event) {
}

QString ShortcutChooser::getCombo(QString initCombo) {
	shortcutLabel->setText(initCombo);
	return NULL;
}

/**********************************************************************************************/	
// ShortcutChooser

#endif
