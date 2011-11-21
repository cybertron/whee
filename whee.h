#ifndef whee_H
#define whee_H

#include <list>
#include <string>
#include <QtGui/QMainWindow>
#include <QtGui/QLabel>
#include <QTimer>
#include <QFont>
#include <QMouseEvent>
#include "NTreeReader.h"
#include "WidgetContainer.h"

using std::list;
using std::string;

class whee : public QMainWindow
{
   Q_OBJECT
   public:
      whee(string);
      virtual ~whee();
   
   public slots:
      void RunUpdates();
      
   private:
      list<WidgetContainerPtr> widgets;
      QTimer timer;
      map<string, unsigned long> unitmap;
      // Default font properties
      string fontname;
      int fontsize;
      int fontweight;
      bool fontitalic;
      QLabel* background;
      
      void ReadNode(const NTreeReader&, int, int);
      void CreateTextWidget(const NTreeReader&, int, int);
      void CreateImageWidget(const NTreeReader&, int, int);
      void CreateMemoryWidget(const NTreeReader&, int, int);
      void CreateNetworkWidget(const NTreeReader&, int, int);
      void CreateCPUWidget(const NTreeReader&, int, int);
      void CreateCommandWidget(const NTreeReader&, int, int);
      void CreateDiskWidget(const NTreeReader&, int, int);
      void CreateTemperatureWidget(const NTreeReader&, int, int);
      
      void SetLabelGeometry(const NTreeReader&, QLabel*, int, int);
      void SetLabelPalette(const NTreeReader&, QLabel*);
      void SetLabelFont(const NTreeReader&, QLabel*);
      void SetLabelPixmap(const NTreeReader&, QLabel*);
      void SetWidgetOrientation(const NTreeReader&, WidgetContainer&);
      void GenUnitMap();
      void SetXProps(int, int, int, int, string);
      void UpdateBackground();
      void mouseReleaseEvent(QMouseEvent*);
};

#endif // whee_H
