#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <WidgetContainer.h>
#include <QProcess>
#include <boost/shared_ptr.hpp>
#include <string>

using std::string;

typedef boost::shared_ptr<QProcess> QProcessPtr;

class CommandWidget : public QObject, public WidgetContainer
{
   Q_OBJECT;
   public:
      CommandWidget(QLabel*);
      virtual void Update();
      
      string command;
      
   public slots:
      void ReadOutput();
      void ProcessFinished();
   
   private:
      QProcessPtr process;
      QString text;
};

typedef boost::shared_ptr<CommandWidget> CommandWidgetPtr;

#endif // COMMANDWIDGET_H
