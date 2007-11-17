#ifndef FILEBROWSER_CPP
#define FILEBROWSER_CPP
#include "filebrowser.h"

FileBrowser::FileBrowser(DataBackend * c,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f) {
	this->setStyleSheet("");
	conn = c;
	model = new QDirModel;
	model->setReadOnly(false);

	filterLine = new QLineEdit();
	QCompleter* completer = new QCompleter(filterLine);
	filterLine->setCompleter(completer);
	completer->setModel(model);
	
	splitter  = new QSplitter(this);

	MyFileIconProvider * customFIP = new MyFileIconProvider();
	model->setIconProvider(customFIP);	

	fileTree = new QTreeView(splitter);
	fileTree->setModel(model);
	fileTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	fileTree->setDragEnabled(true);
	fileTree->setAcceptDrops(true);	

	fileList = new QListView(splitter);
	fileList->setViewMode(QListView::IconMode);
	fileList->setModel(model);
	fileList->setGridSize(QSize(64,64));
	fileList->setIconSize(QSize(32,32));
	fileList->setMovement(QListView::Snap);
	fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	fileList->setDragEnabled(true);
	fileList->setAcceptDrops(true);	
	

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

// 	QFileDialog * tmp = new QFileDialog();
// 	tmp->show();
	connect(fileList,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(handleListDoubleClick(QModelIndex)));
	connect(fileTree,SIGNAL(expanded(QModelIndex)),fileList,SLOT(setRootIndex(QModelIndex)));
	connect(completer,SIGNAL(activated(QModelIndex)),this,SLOT(handleListDoubleClick(QModelIndex)));
// 	connect(lineEdit,SIGNAL(returnPressed()),this, SLOT(setRootPath()));
}

void FileBrowser::handleListDoubleClick(QModelIndex index) {
	QModelIndex dir = index.sibling(index.row(), 0);
	if (!model->isDir(dir))
	dir = dir.parent();
		if (dir != fileList->rootIndex() && model->isDir(dir)) {
                fileList->setRootIndex(dir);
                fileTree->setCurrentIndex(index);
                filterLine->setText(model->filePath(dir));
            }
}

QIcon MyFileIconProvider::icon(IconType type) {
	if(type == QFileIconProvider::Folder) {
	std::cout<<type<<std::endl;
	}
}

QIcon MyFileIconProvider::icon(QFileInfo info) {
	if(info.isDir()) {
	std::cout<<"DIR"<<std::endl;
	}
}

#endif

