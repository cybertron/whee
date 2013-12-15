#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <WidgetContainer.h>
#include <boost/shared_ptr.hpp>
#include <string>

using std::string;

class CommandWidget : public WidgetContainer
{
   public:
      CommandWidget(QLabel*);
      virtual void Update();
      void ProcessFinished(QString);
      
      string command;
};

typedef boost::shared_ptr<CommandWidget> CommandWidgetPtr;

#endif // COMMANDWIDGET_H
