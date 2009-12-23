#ifndef whee_H
#define whee_H

#include <list>
#include <string>
#include <QtGui/QMainWindow>
#include <QtGui/QLabel>
#include <QTimer>
#include <QFont>
#include "IniReader.h"
#include "WidgetContainer.h"

using std::list;
using std::string;

class whee : public QMainWindow
{
   Q_OBJECT
   public:
      whee(int argc, char** argv);
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
      
      void ReadNode(const IniReader&, int, int);
      void CreateTextWidget(const IniReader&, int, int);
      void CreateImageWidget(const IniReader&, int, int);
      void CreateMemoryWidget(const IniReader&, int, int);
      void CreateNetworkWidget(const IniReader&, int, int);
      void CreateCPUWidget(const IniReader&, int, int);
      void CreateCommandWidget(const IniReader&, int, int);
      void CreateDiskWidget(const IniReader&, int, int);
      void CreateTemperatureWidget(const IniReader&, int, int);
      
      void SetLabelGeometry(const IniReader&, QLabel*, int, int);
      void SetLabelPalette(const IniReader&, QLabel*);
      void SetLabelFont(const IniReader&, QLabel*);
      void SetLabelPixmap(const IniReader&, QLabel*);
      void SetWidgetOrientation(const IniReader&, WidgetContainer&);
      void GenUnitMap();
      void SetXProps(int, int, int, int, string);
      void UpdateBackground();
      void mouseReleaseEvent(QMouseEvent*);
};

#endif // whee_H
