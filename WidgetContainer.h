#ifndef WIDGETCONTAINER_H
#define WIDGETCONTAINER_H

#include <boost/shared_ptr.hpp>
#include <QtGui/QLabel>
#include <QFont>
#include <iostream>
#include "NTreeReader.h"
#include "ProcessHelper.h"

using std::cout;
using std::endl;

/* A couple of quick reminders:
   /proc/stat - CPU stats
   /proc/net/dev - Network stats
*/

class WidgetContainer
{
   public:
      QLabel* label;
      QString format;
      enum Type{Text, Image, Graph};
      Type type;
      enum Orientation{Vertical, Horizontal};
      Orientation orientation;
      long interval;
      long remaining;
      bool controlmaster;
      QString host;
      QString username;
      QString idfile;
      QString temppath;

      WidgetContainer(QLabel*);
      WidgetContainer();
      // By default a no-op so this can be used directly for static widgets
      virtual void Update() {}
      virtual void SetText(QString);
      virtual void ProcessFinished(QString) {}
      
      static unsigned long KB() {return KBv;}
      static unsigned long MB() {return MBv;}
      static unsigned long GB() {return GBv;}
      static unsigned long TB() {return TBv;}
      static unsigned long PB() {return PBv;}
      static unsigned long EB() {return EBv;}
      static unsigned long ZB() {return ZBv;}
      static unsigned long YB() {return YBv;}
      
   protected:
      static unsigned long KBv, MBv, GBv, TBv, PBv, EBv, ZBv, YBv;
      QFont font;
      float lastpercent;
      // This object is no longer safe to copy once this has been initialized!
      ProcessHelperPtr helper;
      
      void SetLabelMask(float);
      void DrawGraph(float);
      
      string GetFile(QString, bool startprocess = true);
      void InitProcessHelper();
};

typedef boost::shared_ptr<WidgetContainer> WidgetContainerPtr;

template <typename t>
t clamp(t val, t min, t max)
{
   return val < min ? min : (val > max ? max : val);
}

#endif // WIDGETCONTAINER_H
