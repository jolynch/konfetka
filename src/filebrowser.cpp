#ifndef FILEBROWSER_CPP
#define FILEBROWSER_CPP
#include "filebrowser.h"

FileBrowser::FileBrowser(DataBackend * c,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f) {
	this->setStyleSheet("");
	conn = c;
	model = new QDirModel;
	model->setReadOnly(false);
	model->setResolveSymlinks(true);

	filterLine = new QLineEdit();
	QCompleter* completer = new QCompleter(filterLine);
	filterLine->setCompleter(completer);
	completer->setModel(model);
	
	splitter  = new QSplitter(this);

	fileTree = new QTreeView(splitter);
	fileTree->setModel(model);
	fileTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	fileTree->setDragEnabled(true);
	fileTree->setAcceptDrops(true);	
	for(int i =1;i<4;i++)
	fileTree->setColumnHidden(i,1);

	fileList = new QListView(splitter);
// 	fileList->setViewMode(QListView::IconMode);
	fileList->setModel(model);
// 	fileList->setGridSize(QSize(64,64));
	fileList->setIconSize(QSize(32,32));
	fileList->setMovement(QListView::Snap);
	fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	fileList->setDragEnabled(true);
	fileList->setAcceptDrops(true);	

	QStringList temp;
	temp<<"Media Library"<<"Playlist";
	toWhat = new QComboBox();
	toWhat->addItems(temp);
	
	QLabel * tempLabel = new QLabel(":");

	whichPlaylist = new QComboBox();
	whichPlaylist->setEnabled(false);
	
	QToolBar * lineBar = new QToolBar();
	lineBar->addWidget(filterLine);
	lineBar->addAction(model->iconProvider()->icon(QFileInfo(QDir::homePath())),"Home",this,SLOT(goHome()));
	lineBar->addAction(QIcon(":images/refresh"),"Refresh",this,SLOT(refresh()));
	lineBar->addAction(QIcon(":images/upArrow"),"Up",this,SLOT(goUp()));
	lineBar->addAction(QIcon(":images/leftArrow"),"Back",this,SLOT(goBack()));
	lineBar->addAction(QIcon(":images/rightArrow"),"Forward",this,SLOT(goForward()));
	
	lineBar->setFloatable(true);
	QToolBar * doStuff = new QToolBar();
	doStuff->addAction("Add Selected Files to:",this, SLOT(addSelected()));
	doStuff->addWidget(toWhat);
	doStuff->addWidget(tempLabel);
	doStuff->addWidget(whichPlaylist);

	layout = new QGridLayout();
	layout->setSpacing(1);
	layout->addWidget(lineBar,0,0);
	layout->addWidget(splitter,1,0,2,2);
	layout->addWidget(doStuff,3,0);
	layout->setRowStretch(1,1);
	layout->setRowStretch(0,1);
	this->setLayout(layout);

	deleteSC = new QShortcut(Qt::Key_Delete,this,SLOT(slotRemove()),SLOT(slotRemove()));

	connect(toWhat,SIGNAL(currentIndexChanged(QString)),this,SLOT(destinationChanged(QString)));
	connect(fileTree, SIGNAL(clicked(QModelIndex)),this,SLOT(setRoot(QModelIndex)));
	connect(fileList, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(setRoot(QModelIndex)));
	connect(completer,SIGNAL(activated(QModelIndex)),this,SLOT(handleCompleter(QModelIndex)));
	connect(filterLine,SIGNAL(returnPressed()),this,SLOT(handleCompleter()));
	connect((conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(playlistsChanged(QStringList)),
		this,SLOT(updatePlaylists(QStringList)));
	
	last = model->index(QDir::homePath());
	setRoot(last);
}

void FileBrowser::setRoot(QModelIndex index, bool keep) {
	if(index.isValid() && model->isDir(index)) {
		if(keep) {
		forward.clear();
		backward.push(last);
		last = index;
		}
	fileList->setRootIndex(index);
	fileTree->setCurrentIndex(index);
	filterLine->setText(model->filePath(index));
	}
}

void FileBrowser::slotRemove() {
	if(!this->hasFocus()) return;
	if(QMessageBox::warning(this,"Delete","Are you sure that you want to\ndelete these files from the filesystem?",
		QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes) == QMessageBox::Yes) {
		QModelIndexList list;
		QItemSelectionModel * sel; 
		if(fileList->hasFocus()) {
		sel = fileList->selectionModel();
		list = sel->selectedIndexes();
		}
		else if(fileTree->hasFocus()) {
		sel = fileTree->selectionModel();
		list = sel->selectedIndexes();
		} else {return;}
	
	std::cout<<list.size()<<std::endl;
		for(int i =0;i<list.size();i++) {
			if(model->isDir(list.value(i))) {
				if(model->rmdir(list.value(i)))
				std::cout<<"YAY"<<std::endl;
			}
			else {
				if(model->remove(list.value(i)))
				std::cout<<"YAY"<<std::endl;
			}
		}
	model->refresh();	
	}
}

void FileBrowser::addSelected() {
	QModelIndexList list;
	QItemSelectionModel * sel; 
	sel = fileList->selectionModel();
	list = sel->selectedIndexes();
	for(int i=0;i<list.size();i++) {
	QString url = model->filePath(list.value(i));
	if(!url.contains("file://"))
	url.prepend("file://");
		if(toWhat->currentText()=="Media Library") {
			if(model->isDir(list.value(i))) {
			conn->medialib.pathImport(url.toStdString())(Xmms::bind(&DataBackend::scrapResult, conn));
			}
			else {
			conn->medialib.addEntry(url.toStdString())(Xmms::bind(&DataBackend::scrapResult, conn));
			}
		}
		else if(toWhat->currentText()=="Playlist") {
// 			QString url = conn->encodeUrl(model->filePath(list.value(i)));
// 			std::cout<<url.toStdString()<<std::endl;
			if(model->isDir(list.value(i))) {
			conn->playlist.addRecursive(url.toStdString(),
				whichPlaylist->currentText().toStdString())(Xmms::bind(&DataBackend::scrapResult, conn));
// 			std::cout<<"Path "<<model->filePath(list.value(i)).toStdString()<<std::endl;
			}
			else {
			conn->playlist.addUrl(url.toStdString(),
				whichPlaylist->currentText().toStdString())(Xmms::bind(&DataBackend::scrapResult, conn));
// 			std::cout<<model->filePath(list.value(i)).toStdString()<<std::endl;
			}
		}
		else {
			qDebug("Only takes playlist or mlib, carefull what you call");
		}
	}
}

void FileBrowser::destinationChanged(QString what) {
	if(what=="Media Library") {
	whichPlaylist->clear();
	whichPlaylist->setEnabled(false);
	}
	else if(what=="Playlist") {
	whichPlaylist->clear();
	whichPlaylist->addItems(((CollData*)(conn->getDataBackendObject(DataBackend::COLL)))->getPlaylists());
	whichPlaylist->setEnabled(true);
	QString cur = ((PlistData*)(conn->getDataBackendObject(DataBackend::PLIST)))->getCurrentName();
	whichPlaylist->setCurrentIndex(whichPlaylist->findText(cur));
	}
	else {
	whichPlaylist->clear();
	whichPlaylist->addItems(((CollData*)(conn->getDataBackendObject(DataBackend::COLL)))->getCollections());
	whichPlaylist->setEnabled(true);
	}
}

void FileBrowser::handleCompleter() {
	setRoot(model->index(filterLine->text()));
}

void FileBrowser::handleCompleter(QModelIndex index) {
	setRoot(model->index(filterLine->text()));
}

void FileBrowser::goHome() {
	setRoot(model->index(QDir::homePath()));
}

void FileBrowser::goUp() {
	QModelIndex dir = fileList->rootIndex();
	if(dir.parent().isValid())
	setRoot(dir.parent());
}

void FileBrowser::goBack() {
	if(!backward.isEmpty()) {
		forward.push(fileList->rootIndex());
		last = backward.pop();
		setRoot(last, false);
	}
}

void FileBrowser::goForward() {
	if(!forward.isEmpty()) {
		backward.push(fileList->rootIndex());
		last = forward.pop();
		setRoot(last, false);
	}
}

void FileBrowser::refresh() {
	model->refresh(fileList->rootIndex());
	setRoot(fileList->rootIndex(), false);
}

void FileBrowser::updatePlaylists(QStringList newList) {
	std::cout<<"NEW LIST"<<std::endl;
	if(toWhat->currentText()=="Playlist") {
	whichPlaylist->clear();
	whichPlaylist->addItems(newList);
	}
}

#endif

