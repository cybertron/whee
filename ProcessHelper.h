#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QProcess>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<QProcess> QProcessPtr;

class WidgetContainer;

class ProcessHelper : public QObject
{
   Q_OBJECT
   public:
      ProcessHelper();
      void Start(QString, WidgetContainer*);
      bool Active();
      
   public slots:
      void ReadOutput();
      void ReadError();
      void ProcessFinished();
   
   private:
      WidgetContainer* widget;
      QProcessPtr process;
      QString text;
      bool active;
};

typedef boost::shared_ptr<ProcessHelper> ProcessHelperPtr;

#endif // PROCESSHELPER_H
