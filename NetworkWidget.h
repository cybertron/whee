#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <WidgetContainer.h>
#include <string>
#include <QTime>

using std::string;

class NetworkWidget : public WidgetContainer
{
   public:
      string interface;
      bool down;
      size_t lastbytes;
      QTime timer;
      float max;
      
      NetworkWidget(QLabel*);
      virtual void Update();
};

#endif // NETWORKWIDGET_H
