#ifndef HANDLE_H
#define HANDLE_H
#include <QLabel>
#include <QTimer>
class Handle:public QLabel
	{	Q_OBJECT
	private:
		bool locked;
		QTimer * animator;
		int currentPos;
		int destination;
		int height;
		int amount;
	public:
		Handle(QString image,  QWidget * parent = 0, Qt::WindowFlags f = 0);
	private:
		void lock();
		void unlock();
	protected:
		void mousePressEvent(QMouseEvent* event);
	private slots:
		void moveHandle();
	public slots:
		void moveThis(int,int,int,int);
	signals:
		void clicked();
		void done();
	};
#endif
