#ifndef DISKWIDGET_H
#define DISKWIDGET_H

#include <WidgetContainer.h>
#include <string>
#include <QTime>

using std::string;

class DiskWidget : public WidgetContainer
{
   public:
      DiskWidget(QLabel*);
      virtual void Update();
      void DoUpdate(bool);
      void ProcessFinished(QString);
      
      enum Statistic{Total, Free, Used, Read, Write, ReadWrite};
      Statistic stat;
      string path;
      size_t unit;
      float max;
      float sectorsize;
      
      QTime timer;
      unsigned long long lastread, lastwrite;
};

#endif // DISKWIDGET_H
