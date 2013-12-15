#ifndef TEMPERATUREWIDGET_H
#define TEMPERATUREWIDGET_H

#include <WidgetContainer.h>
#include <QProcess>
#include <boost/shared_ptr.hpp>
#include <string>

using std::string;

typedef boost::shared_ptr<QProcess> QProcessPtr;

class TemperatureWidget : public QObject, public WidgetContainer
{
   public:
      TemperatureWidget(QLabel*);
      virtual void Update();
      void ProcessFinished(QString);
      
      string chip, tempid;
      float max;
};

typedef boost::shared_ptr<TemperatureWidget> TemperatureWidgetPtr;

#endif // TEMPERATUREWIDGET_H
