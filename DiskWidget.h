#ifndef DISKWIDGET_H
#define DISKWIDGET_H

#include <WidgetContainer.h>
#include <string>

using std::string;

class DiskWidget : public WidgetContainer
{
   public:
      DiskWidget(QLabel*);
      virtual void Update();
      
      enum Statistic{Total, Free, Used};
      Statistic stat;
      string path;
      size_t unit;
};

#endif // DISKWIDGET_H
