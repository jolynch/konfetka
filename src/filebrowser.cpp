#ifndef FILEBROWSER_CPP
#define FILEBROWSER_CPP
#include "filebrowser.h"

#endif

FileBrowser::FileBrowser(DataBackend * c,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f) {
	conn = c;
	filterLine = new QLineEdit();
	splitter  = new QSplitter();

	model = new QDirModel;
	
	fileTree = new QTreeView(splitter);
	fileTree->setModel(model);
	fileTree->setSelectionMode(QAbstractItemView::ExtendedSelection);	

	fileList = new QListView(splitter);
	fileList->setViewMode(QListView::IconMode);
	fileList->setModel(model);
	fileList->setGridSize(QSize(64,64));
	fileList->setMovement(QListView::Snap);
	fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);

	layout = new QGridLayout();
	layout->setSpacing(1);
	layout->addWidget(filterLine,0,0);
	layout->addWidget(splitter,1,0);
	for(int i=0;i<2;i++)
	layout->setRowStretch(i,2);
	this->setLayout(layout);
	
// 	QStringList filters;
// 	filters << "*mp3*" << "*m4a*";
// 	dirModel->setNameFilters(filters);
// 	dirModel->setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
// 	
	connect(fileList,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(handleListDoubleClick(QModelIndex)));
	connect(fileTree,SIGNAL(expanded(QModelIndex)),fileList,SLOT(setRootIndex(QModelIndex)));
}

void FileBrowser::handleListDoubleClick(QModelIndex item) {
	if(model->isDir(item)) { 
	fileList->setRootIndex(item);
	fileTree->setCurrentIndex(item);
	fileTree->expand(item);
	}
	else {
		QDesktopServices::openUrl(QUrl((model->fileInfo(item).absoluteFilePath())));
	}
}


