#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef SHOUTCAST_H
#define SHOUTCAST_H
#include "../databackend.h"
#include "../databackend/plistdata.h"
#include "webservice.h"

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QString>
#include <QGroupBox>

#include <QList>
#include <QStringList>
#include <QHash>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QVariant>

//class DragTreeWidget;

class Shoutcast:public WebService{
	Q_OBJECT
	private :
		QLabel * info;
		QLineEdit * search;
		QGroupBox * searchOptions;
		QLineEdit * bitrate;
		QLineEdit * streamtype;
		QLineEdit * genre;
		QLineEdit * limit;
		QGridLayout * layout;
		
		//DragTreeWidget * tree;
		QTreeWidget * tree;
		QPushButton * refresh;
		QPushButton * addStream;
		QPushButton * searchButton;
		QComboBox * searchType;
		QComboBox * addMethod;
		
		QNetworkAccessManager * manager;
		DataBackend * conn;
		PlistData * plist;
		
		QHash<QTreeWidgetItem*,QString> genres; 
		QHash<QNetworkReply *,QTreeWidgetItem*> requests;
		QList<QTreeWidgetItem*> requestsCompleted;
		QHash<QString,QUrl> streams;
		QString currentSearch;


	public :
		Shoutcast(DataBackend * c,QWidget * p=0, Qt::WindowFlags f=0);
		~Shoutcast();
		QString getInfo();
		QString getServiceName();
		QMimeData* getMimeData(QList<QTreeWidgetItem*> items);
		
	public slots:
		//Shows some more search options
		void toggleOptions(bool overide = 0);

		//Fetch entire list of genres
		void fetchGenres();
		void recievedGenres(QNetworkReply * reply);
		//Fetch streams from single genre
		void fetchGenre(QTreeWidgetItem * genre);
		void addGenre(QNetworkReply * reply);
		//Do a search
		void fetchQuery();
		void recievedQuery(QNetworkReply * reply);

		//Handles the streams
		void processStream();
		void recievedStream(QNetworkReply * reply);
};


/* No Drag and drop until Anton and I talk about how we want to do it
class DragTreeWidget:public QTreeWidget {
Q_OBJECT
	
	private:
	Shoutcast * shout;

	public:
	DragTreeWidget(Shoutcast *);
	~DragTreeWidget();
	
	//Drag Drop / User interaction
	void dropEvent(QDropEvent *event);
	void dragMoveEvent ( QDragMoveEvent * ); 
	void dragEnterEvent(QDragEnterEvent *event);
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const;
};
*/

#endif
