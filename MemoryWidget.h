#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <WidgetContainer.h>

class MemoryWidget : public WidgetContainer
{
   public:
      enum Statistic{Total, Free, Used, TotalSwap, FreeSwap, UsedSwap};
      Statistic stat;
      unsigned long unit;
      
      MemoryWidget(QLabel*);
      virtual void Update();
      void DoUpdate();
      virtual void ProcessFinished(QString);
};

#endif // MEMORYWIDGET_H
