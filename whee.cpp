#include "whee.h"

#include <QX11Info>
#include <QDesktopWidget>
#include <QApplication>
#include <QPaintEvent>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "MemoryWidget.h"
#include "NetworkWidget.h"
#include "CPUWidget.h"
#include "DiskWidget.h"
#include "MemoryWidget.h"
#include "CommandWidget.h"
#include "TemperatureWidget.h"

using std::cout;
using std::endl;

whee::whee(int argc, char** argv) : background(NULL)
{
   setWindowFlags(Qt::FramelessWindowHint);
   setAttribute(Qt::WA_TranslucentBackground);
   
   if (argc != 2)
   {
      cout << "Invalid number of arguments" << endl;
   }
   else
   {
      string filename = argv[1];
      IniReader read(filename);
      
      ssize_t x, y, width, height;
      size_t interval = 1000;
      string strut = "";
      read.Read(x, "Position", 0);
      read.Read(y, "Position", 1);
      read.Read(width, "Dimensions", 0);
      read.Read(height, "Dimensions", 1);
      read.Read(interval, "Interval");
      read.Read(strut, "Strut");
      
      string name;
      int buffer = 0;
      fontname = "Arial";
      read.ReadLine(fontname, "Font");
      fontsize = 12;
      read.Read(fontsize, "FontSize");
      
      fontweight = QFont::Normal;
      read.Read(buffer, "Bold");
      if (buffer > 0)
         fontweight = QFont::Bold;
      
      fontitalic = false;
      buffer = 0;
      read.Read(buffer, "Italic");
      if (buffer > 0)
         fontitalic = true;
      
      SetXProps(x, y, width, height, strut);
      
      setGeometry(x, y, width, height);
      
      GenUnitMap();
       
      background = new QLabel(this);
      
      ReadNode(read, 0, 0);
      
      RunUpdates();
      QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(RunUpdates()));
      timer.start(interval);
   }
}

whee::~whee() {}


void whee::ReadNode(const IniReader& read, int offx, int offy)
{
   for (size_t i = 0; i < read.NumChildren(); ++i)
   {
      const IniReader& curr = read.GetItem(i);
      string type;
      curr.Read(type, "Type");
      QString qtype(type.c_str());
      qtype = qtype.toLower();
      if (qtype == "text")
      {
         CreateTextWidget(curr, offx, offy);
      }
      else if (qtype == "image")
      {
         CreateImageWidget(curr, offx, offy);
      }
      else if (qtype == "cpu")
      {
         CreateCPUWidget(curr, offx, offy);
      }
      else if (qtype == "memory")
      {
         CreateMemoryWidget(curr, offx, offy);
      }
      else if (qtype == "disk")
      {
         CreateDiskWidget(curr, offx, offy);
      }
      else if (qtype == "network")
      {
         CreateNetworkWidget(curr, offx, offy);
      }
      else if (qtype == "command")
      {
         CreateCommandWidget(curr, offx, offy);
      }
      else if (qtype == "temperature")
      {
         CreateTemperatureWidget(curr, offx, offy);
      }
      else if (qtype == "group")
      {
         int x = 0;
         int y = 0;
         curr.Read(x, "Position", 0);
         curr.Read(y, "Position", 1);
         ReadNode(curr, offx + x, offy + y);
      }
   }
}


void whee::CreateTextWidget(const IniReader& curr, int offx, int offy)
{
   string text;
   curr.ReadLine(text, "Text");
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   SetLabelPalette(curr, label);
   SetLabelFont(curr, label);
   label->setText(text.c_str());
   widgets.push_back(WidgetContainerPtr(new WidgetContainer(label)));
}


void whee::CreateImageWidget(const IniReader& curr, int offx, int offy)
{
   string filename;
   curr.ReadLine(filename, "Filename");
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   label->setPixmap(QPixmap(filename.c_str()));
   widgets.push_back(WidgetContainerPtr(new WidgetContainer(label)));
}


void whee::CreateMemoryWidget(const IniReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   MemoryWidget w(label);
   SetLabelPalette(curr, label);
   if (qtype == "text")
   {
      SetLabelFont(curr, label);
      w.type = WidgetContainer::Text;
   }
   else
   {
      SetLabelPixmap(curr, label);
      SetWidgetOrientation(curr, w);
      w.type = WidgetContainer::Image;
   }
   
   string stat;
   curr.Read(stat, "Statistic");
   QString qstat = stat.c_str();
   qstat = qstat.toLower();
   if (qstat == "total")
   {
      w.stat = MemoryWidget::Total;
   }
   else if (qstat == "free")
   {
      w.stat = MemoryWidget::Free;
   }
   else if (qstat == "used")
   {
      w.stat = MemoryWidget::Used;
   }
   else if (qstat == "totalswap")
   {
      w.stat = MemoryWidget::TotalSwap;
   }
   else if (qstat == "freeswap")
   {
      w.stat = MemoryWidget::FreeSwap;
   }
   else if (qstat == "usedswap")
   {
      w.stat = MemoryWidget::UsedSwap;
   }
   
   string format;
   curr.ReadLine(format, "Format");
   w.format = QString(format.c_str());
   
   string unit = "KB";
   curr.Read(unit, "Unit");
   w.unit = unitmap[unit];
   
   widgets.push_back(WidgetContainerPtr(new MemoryWidget(w)));
}


void whee::CreateNetworkWidget(const IniReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   
   NetworkWidget w(label);
   SetLabelPalette(curr, label);
   if (qtype == "text")
   {
      SetLabelFont(curr, label);
   }
   else if (qtype == "image")
   {
      SetLabelPixmap(curr, label);
      SetWidgetOrientation(curr, w);
      w.type = WidgetContainer::Image;
   }
   else if (qtype == "graph")
   {
      w.type = WidgetContainer::Graph;
   }
   
   string dir;
   curr.Read(dir, "Direction");
   QString qdir = dir.c_str();
   qdir = qdir.toLower();
   if (qdir == "up")
      w.down = false;
   else w.down = true;
   
   string format;
   curr.ReadLine(format, "Format");
   w.format = QString(format.c_str());
   
   curr.Read(w.interface, "Interface");
   curr.Read(w.max, "Max");
   widgets.push_back(WidgetContainerPtr(new NetworkWidget(w)));
}


void whee::CreateCPUWidget(const IniReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   
   CPUWidget w(label);
   SetLabelPalette(curr, label);
   if (qtype == "text")
   {
      SetLabelFont(curr, label);
   }
   else if (qtype == "image")
   {
      SetLabelPixmap(curr, label);
      SetWidgetOrientation(curr, w);
      w.type = WidgetContainer::Image;
   }
   else if (qtype == "graph")
   {
      w.type = WidgetContainer::Graph;
   }
   
   string stat;
   curr.Read(stat, "Statistic");
   QString qstat = stat.c_str();
   qstat = qstat.toLower();
   if (qstat == "user")
      w.stat = CPUWidget::User;
   else if (qstat == "nice")
      w.stat = CPUWidget::Nice;
   else if (qstat == "system")
      w.stat = CPUWidget::System;
   else if (qstat == "idle")
      w.stat = CPUWidget::Idle;
   else if (qstat == "io")
      w.stat = CPUWidget::IO;
   else
      w.stat = CPUWidget::Busy;
   
   string format;
   curr.ReadLine(format, "Format");
   w.format = QString(format.c_str());
   
   widgets.push_back(WidgetContainerPtr(new CPUWidget(w)));
}


void whee::CreateCommandWidget(const IniReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   SetLabelPalette(curr, label);
   SetLabelFont(curr, label);
   
   // Have to use a pointer to this one because it inherits from QObject
   CommandWidgetPtr w(new CommandWidget(label));
   curr.ReadLine(w->command, "Command");
   
   widgets.push_back(w);
}


void whee::CreateDiskWidget(const IniReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   
   DiskWidget w(label);
   SetLabelPalette(curr, label);
   if (qtype == "text")
   {
      SetLabelFont(curr, label);
   }
   else if (qtype == "image")
   {
      SetLabelPixmap(curr, label);
      SetWidgetOrientation(curr, w);
      w.type = WidgetContainer::Image;
   }
   else if (qtype == "graph")
   {
      w.type = WidgetContainer::Graph;
   }
   
   string format;
   curr.ReadLine(format, "Format");
   w.format = QString(format.c_str());
   
   curr.ReadLine(w.path, "Path");
   
   string stat;
   curr.Read(stat, "Statistic");
   QString qstat = stat.c_str();
   qstat = qstat.toLower();
   
   if (qstat == "free")
      w.stat = DiskWidget::Free;
   else if (qstat == "used")
      w.stat = DiskWidget::Used;
   else if (qstat == "total")
      w.stat = DiskWidget::Total;
   
   string unit = "KB";
   curr.Read(unit, "Unit");
   w.unit = unitmap[unit];
   
   widgets.push_back(WidgetContainerPtr(new DiskWidget(w)));
}


void whee::CreateTemperatureWidget(const IniReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(this);
   SetLabelGeometry(curr, label, offx, offy);
   
   // Have to use a pointer to this one because it inherits from QObject
   TemperatureWidgetPtr w(new TemperatureWidget(label));
   SetLabelPalette(curr, label);
   if (qtype == "text")
   {
      SetLabelFont(curr, label);
   }
   else if (qtype == "image")
   {
      SetLabelPixmap(curr, label);
      SetWidgetOrientation(curr, *w);
      w->type = WidgetContainer::Image;
   }
   else if (qtype == "graph")
   {
      w->type = WidgetContainer::Graph;
   }
   
   string format;
   curr.ReadLine(format, "Format");
   w->format = QString(format.c_str());
   
   curr.Read(w->core, "Core");
   curr.Read(w->max, "Max");
   
   widgets.push_back(w);
}


void whee::SetLabelGeometry(const IniReader& curr, QLabel* label, int offx, int offy)
{
   ssize_t x = 0, y = 0, width = 100, height = 100;
   curr.Read(x, "Position", 0);
   curr.Read(y, "Position", 1);
   curr.Read(width, "Dimensions", 0);
   curr.Read(height, "Dimensions", 1);
   
   label->setGeometry(x + offx, y + offy, width, height);
   
   label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


void whee::SetLabelPalette(const IniReader& curr, QLabel* label)
{
   int fgr = 0, fgg = 0, fgb = 0;
   int bgr = 255, bgg = 255, bgb = 255;
   curr.Read(fgr, "FGColor", 0);
   curr.Read(fgg, "FGColor", 1);
   curr.Read(fgb, "FGColor", 2);
   curr.Read(bgr, "BGColor", 0);
   curr.Read(bgg, "BGColor", 1);
   curr.Read(bgb, "BGColor", 2);
   
   QPalette currpal = label->palette();
   currpal.setColor(QPalette::WindowText, QColor(fgr, fgg, fgb));
   currpal.setColor(QPalette::Window, QColor(bgr, bgg, bgb));
   label->setPalette(currpal);
}


void whee::SetLabelFont(const IniReader& curr, QLabel* label)
{
   string font = fontname;
   int buffer = 0;
   int size = fontsize;
   int weight = fontweight;
   bool italic = fontitalic;
   
   curr.ReadLine(font, "Font");
   curr.Read(size, "FontSize");
   curr.Read(buffer, "Bold");
   if (buffer > 0)
      weight = QFont::Bold;
   buffer = 0;
   curr.Read(buffer, "Italic");
   if (buffer > 0)
      italic = true;
   label->setFont(QFont(QString(font.c_str()), size, weight, italic));
}


void whee::SetLabelPixmap(const IniReader& curr, QLabel* label)
{
   string filename;
   curr.ReadLine(filename, "Filename");
   label->setPixmap(QPixmap(filename.c_str()));
   label->setScaledContents(true);
}


void whee::SetWidgetOrientation(const IniReader& curr, WidgetContainer& w)
{
   string o;
   curr.Read(o, "Orientation");
   QString qo = o.c_str();
   qo = qo.toLower();
   if (qo == "horizontal")
      w.orientation = WidgetContainer::Horizontal;
}


void whee::RunUpdates()
{
   for (list<WidgetContainerPtr>::iterator i = widgets.begin(); i != widgets.end(); ++i)
   {
      (*i)->Update();
   }
}


void whee::SetXProps(int x, int y, int width, int height, string strut)
{
   Window window = winId();
   Display* display = QX11Info::display();
   Window rootwindow = DefaultRootWindow(display);
   Atom net_wm_strut = XInternAtom(display, "_NET_WM_STRUT", false);
   Atom net_wm_strut_partial = XInternAtom(display, "_NET_WM_STRUT_PARTIAL", false);
   Atom net_wm_state = XInternAtom(display, "_NET_WM_STATE", false);
   Atom net_wm_state_sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", false);
   Atom net_wm_state_below = XInternAtom(display, "_NET_WM_STATE_BELOW", false);
   Atom net_wm_state_skip_taskbar = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", false);
   Atom net_wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", false);
   Atom net_wm_window_type_desktop = XInternAtom(display, "_NET_WM_TYPE_DESKTOP", false);
   Atom net_wm_desktop = XInternAtom(display, "_NET_WM_DESKTOP", false);
   
   long atoms[3];
   atoms[0] = net_wm_state_sticky;
   atoms[1] = net_wm_state_skip_taskbar;
   atoms[2] = net_wm_state_below;
   XChangeProperty(display, window, net_wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 3);
   
   long data[12];
   
   // Apparently KDE doesn't honor sticky state
   data[0] = 0xFFFFFFFF;
   XChangeProperty(display, window, net_wm_desktop, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)data, 1);
   
   QString qstrut = strut.c_str();
   qstrut = qstrut.toLower();
   if (qstrut == "top")
   {
      data[0] = 0;
      data[1] = 0;
      data[2] = height;
      data[3] = 0;
      data[4] = 0;
      data[5] = 0;
      data[6] = 0;
      data[7] = 0;
      data[8] = x;
      data[9] = x + width;
      data[10] = 0;
      data[11] = 0;
      // Apparently Fluxbox doesn't support partial struts:-(
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 4);
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 12);
   }
   else if (qstrut == "bottom")
   {
      data[0] = 0;
      data[1] = 0;
      data[2] = 0;
      data[3] = height;
      data[4] = 0;
      data[5] = 0;
      data[6] = 0;
      data[7] = 0;
      data[8] = 0;
      data[9] = 0;
      data[10] = x;
      data[11] = x + width;
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 4);
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 12);
   }
   else if (qstrut == "left")
   {
      data[0] = width;
      data[1] = 0;
      data[2] = 0;
      data[3] = 0;
      data[4] = y;
      data[5] = y + height;
      data[6] = 0;
      data[7] = 0;
      data[8] = 0;
      data[9] = 0;
      data[10] = 0;
      data[11] = 0;
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 4);
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 12);
   }
   else if (qstrut == "right")
   {
      data[0] = 0;
      data[1] = width;
      data[2] = 0;
      data[3] = 0;
      data[4] = 0;
      data[5] = 0;
      data[6] = y;
      data[7] = y + height;
      data[8] = 0;
      data[9] = 0;
      data[10] = 0;
      data[11] = 0;
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 4);
      XChangeProperty(display, window, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 12);
   }
}


// This is a bad way to handle transparency, but it works well enough and doesn't require
// any nasty Xlib use.
void whee::UpdateBackground()
{
   if (!QX11Info::isCompositingManagerRunning())
   {
      QDesktopWidget* desktop = QApplication::desktop();
      WId id = desktop->winId();
      int savewidth = width();
      setGeometry(x() - 1, y(), 1, height());
      QPixmap pixmap = QPixmap::grabWindow(id, x() + 1, y(), savewidth, height());
      setGeometry(x() + 1, y(), savewidth, height());
      background->setGeometry(0, 0, width(), height());
      background->setPixmap(pixmap);
   }
}


void whee::mouseReleaseEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
      UpdateBackground();
}


void whee::GenUnitMap()
{
   unitmap["KB"] = WidgetContainer::KB();
   unitmap["MB"] = WidgetContainer::MB();
   unitmap["GB"] = WidgetContainer::GB();
   unitmap["TB"] = WidgetContainer::TB();
   unitmap["PB"] = WidgetContainer::PB();
   unitmap["EB"] = WidgetContainer::EB();
   unitmap["ZB"] = WidgetContainer::ZB();
   unitmap["YB"] = WidgetContainer::YB();
}

#include "whee.moc"
