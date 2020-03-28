#ifndef whee_H
#define whee_H

#include <list>
#include <string>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QTimer>
#include <QFont>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QBoxLayout>
#include "NTreeReader.h"
#include "WidgetContainer.h"

using std::list;
using std::string;

class whee : public QDialog
{
   Q_OBJECT
   public:
      whee(string);
      virtual ~whee();
   
   public slots:
      void RunUpdates();
      void contextMenu(const QPoint&);
      void reloadClicked();
      void closeClicked();
      
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
      string filename;
      QBoxLayout* currlayout;
      QString temppath;
      
      void CreateTemp();
      void RemoveTemp();
      void ReadConfig();
      void ReadNode(const NTreeReader&, int, int);
      void CreateTextWidget(const NTreeReader&, int, int);
      void CreateImageWidget(const NTreeReader&, int, int);
      void CreateMemoryWidget(const NTreeReader&, int, int);
      void CreateNetworkWidget(const NTreeReader&, int, int);
      void CreateCPUWidget(const NTreeReader&, int, int);
      void CreateCommandWidget(const NTreeReader&, int, int);
      void CreateDiskWidget(const NTreeReader&, int, int);
      void CreateTemperatureWidget(const NTreeReader&, int, int);
      void CreateLayout(const NTreeReader&, int, int);
      
      void SetLabelGeometry(const NTreeReader&, QLabel*, int, int);
      void SetLabelPalette(const NTreeReader&, QLabel*);
      void SetLabelFont(const NTreeReader&, QLabel*);
      void SetLabelPixmap(const NTreeReader&, QLabel*);
      void SetWidgetOrientation(const NTreeReader&, WidgetContainer&);
      void SetHost(const NTreeReader&, WidgetContainer&);
      void GenUnitMap();
      void SetXProps(int, int, int, int, string);
      void UpdateBackground();
      void mouseReleaseEvent(QMouseEvent*);
      void closeEvent(QCloseEvent*);
};

#endif // whee_H
