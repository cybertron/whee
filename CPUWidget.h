#ifndef CPUWIDGET_H
#define CPUWIDGET_H

#include <WidgetContainer.h>
#include <QTime>

class CPUWidget : public WidgetContainer
{
   public:
      CPUWidget(QLabel*);
      virtual void Update();
      
      enum Statistic{User, Nice, System, Idle, IO, Busy};
      Statistic stat;
      
   private:
      size_t lastjiffies, lasttotal;
      QTime timer;
};

#endif // CPUWIDGET_H
