#include "whee.h"

#include <QX11Info>
#include <QDesktopWidget>
#include <QApplication>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

whee::whee(string fn) : background(NULL), filename(fn), currlayout(NULL)
{
   setWindowFlags(Qt::FramelessWindowHint);
   // Causes issues in some non-compositing WM's (notably Fluxbox)
   if (QX11Info::isCompositingManagerRunning())
      setAttribute(Qt::WA_TranslucentBackground);
   
   GenUnitMap();
   
   QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(RunUpdates()));
   
   ReadConfig();
   
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenu(const QPoint&)));
   
   show();
}

whee::~whee() {}


void whee::ReadConfig()
{
   NTreeReader read(filename);
   
   ssize_t x, y, width, height;
   size_t interval = 1000;
   string strut = "";
   read.Read(x, "Position", 0);
   read.Read(y, "Position", 1);
   read.Read(width, "Dimensions", 0);
   read.Read(height, "Dimensions", 1);
   read.Read(interval, "Interval");
   if (interval < 1000)
      cout << "Warning: Interval < 1000 may cause some sensors to behave improperly" << endl;
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
   
   background = new QLabel(this);
   background->setGeometry(0, 0, width, height);
   background->show();
   
   ReadNode(read, 0, 0);
   
   for (list<WidgetContainerPtr>::iterator i = widgets.begin(); i != widgets.end(); ++i)
   {
      (*i)->label->show();
   }
   
   RunUpdates();
   timer.start(interval);
   
   SetXProps(x, y, width, height, strut);
   
   setGeometry(x, y, width, height);
}


void whee::ReadNode(const NTreeReader& read, int offx, int offy)
{
   for (size_t i = 0; i < read.NumChildren(); ++i)
   {
      const NTreeReader& curr = read.GetItem(i);
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
      else if (qtype == "layout")
      {
         QBoxLayout* save = currlayout;
         CreateLayout(curr, offx, offy);
         ReadNode(curr, 0, 0);
         currlayout = save;
      }
      else if (qtype == "stretch")
      {
         if (currlayout)
         {
            int amount = 1;
            curr.Read(amount, "Amount");
            currlayout->addStretch(amount);
         }
      }
   }
}


void whee::CreateTextWidget(const NTreeReader& curr, int offx, int offy)
{
   string text;
   curr.ReadLine(text, "Text");
   
   QLabel* label = new QLabel(background);
   SetLabelGeometry(curr, label, offx, offy);
   SetLabelPalette(curr, label);
   SetLabelFont(curr, label);
   label->setText(text.c_str());
   widgets.push_back(WidgetContainerPtr(new WidgetContainer(label)));
}


void whee::CreateImageWidget(const NTreeReader& curr, int offx, int offy)
{
   string filename;
   curr.ReadLine(filename, "Filename");
   
   QLabel* label = new QLabel(background);
   SetLabelGeometry(curr, label, offx, offy);
   label->setPixmap(QPixmap(filename.c_str()));
   widgets.push_back(WidgetContainerPtr(new WidgetContainer(label)));
}


void whee::CreateMemoryWidget(const NTreeReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(background);
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
   
   curr.Read(w.interval, "Interval");
   
   SetHost(curr, w);
   
   widgets.push_back(WidgetContainerPtr(new MemoryWidget(w)));
}


void whee::CreateNetworkWidget(const NTreeReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(background);
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
   curr.Read(w.interval, "Interval");
   
   SetHost(curr, w);
   
   widgets.push_back(WidgetContainerPtr(new NetworkWidget(w)));
}


void whee::CreateCPUWidget(const NTreeReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(background);
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
   
   curr.Read(w.interval, "Interval");
   
   SetHost(curr, w);
   
   widgets.push_back(WidgetContainerPtr(new CPUWidget(w)));
}


void whee::CreateCommandWidget(const NTreeReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(background);
   SetLabelGeometry(curr, label, offx, offy);
   SetLabelPalette(curr, label);
   SetLabelFont(curr, label);
   
   // Have to use a pointer to this one because it inherits from QObject
   CommandWidgetPtr w(new CommandWidget(label));
   curr.ReadLine(w->command, "Command");
   
   curr.Read(w->interval, "Interval");
   
   widgets.push_back(w);
}


void whee::CreateDiskWidget(const NTreeReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(background);
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
   else if (qstat == "read")
      w.stat = DiskWidget::Read;
   else if (qstat == "write")
      w.stat = DiskWidget::Write;
   else if (qstat == "readwrite")
      w.stat = DiskWidget::ReadWrite;
   
   string unit = "KB";
   curr.Read(unit, "Unit");
   w.unit = unitmap[unit];
   
   curr.Read(w.max, "Max");
   curr.Read(w.sectorsize, "SectorSize");
   
   curr.Read(w.interval, "Interval");
   
   SetHost(curr, w);
   
   widgets.push_back(WidgetContainerPtr(new DiskWidget(w)));
}


void whee::CreateTemperatureWidget(const NTreeReader& curr, int offx, int offy)
{
   string type;
   curr.Read(type, "SubType");
   QString qtype = type.c_str();
   qtype = qtype.toLower();
   
   QLabel* label = new QLabel(background);
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
   
   curr.ReadLine(w->chip, "Chip");
   curr.ReadLine(w->tempid, "ID");
   curr.Read(w->max, "Max");
   curr.Read(w->interval, "Interval");
   
   SetHost(curr, *w);
   
   widgets.push_back(w);
}


void whee::CreateLayout(const NTreeReader& curr, int offx, int offy)
{
   string orientation;
   // Dummy widget to read orientation
   WidgetContainer w;
   SetWidgetOrientation(curr, w);
   QBoxLayout* newlayout;
   if (w.orientation == w.Vertical)
      newlayout = new QVBoxLayout();
   else
      newlayout = new QHBoxLayout();
   
   int spacing = 5;
   curr.Read(spacing, "Spacing");
   newlayout->setSpacing(spacing);
   int left, top, right, bottom;
   left = top = right = bottom = 10;
   curr.Read(left, "Margins", 0);
   curr.Read(top, "Margins", 1);
   curr.Read(right, "Margins", 2);
   curr.Read(bottom, "Margins", 3);
   newlayout->setContentsMargins(left, top, right, bottom);
   
   if (currlayout)
   {
      currlayout->addLayout(newlayout);
   }
   else
   {
      if (layout())
         delete layout();
      background->setLayout(newlayout);
   }
   
   currlayout = newlayout;
}


void whee::SetLabelGeometry(const NTreeReader& curr, QLabel* label, int offx, int offy)
{
   if (!currlayout)
   {
      ssize_t x = 0, y = 0, width = 100, height = 100;
      curr.Read(x, "Position", 0);
      curr.Read(y, "Position", 1);
      curr.Read(width, "Dimensions", 0);
      curr.Read(height, "Dimensions", 1);
      
      label->setGeometry(x + offx, y + offy, width, height);
   }
   else
   {
      currlayout->addWidget(label);
   }
   
   label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


void whee::SetLabelPalette(const NTreeReader& curr, QLabel* label)
{
   int fgr = 0, fgg = 0, fgb = 0, fga = 255;
   int bgr = 255, bgg = 255, bgb = 255, bga = 0;
   curr.Read(fgr, "FGColor", 0);
   curr.Read(fgg, "FGColor", 1);
   curr.Read(fgb, "FGColor", 2);
   curr.Read(fga, "FGColor", 3);
   curr.Read(bgr, "BGColor", 0);
   curr.Read(bgg, "BGColor", 1);
   curr.Read(bgb, "BGColor", 2);
   curr.Read(bga, "BGColor", 3);
   
   QPalette currpal = label->palette();
   currpal.setColor(QPalette::WindowText, QColor(fgr, fgg, fgb, fga));
   currpal.setColor(QPalette::Window, QColor(bgr, bgg, bgb, bga));
   label->setPalette(currpal);
}


void whee::SetLabelFont(const NTreeReader& curr, QLabel* label)
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


void whee::SetLabelPixmap(const NTreeReader& curr, QLabel* label)
{
   string filename;
   curr.ReadLine(filename, "Filename");
   label->setPixmap(QPixmap(filename.c_str()));
   label->setScaledContents(true);
}


void whee::SetWidgetOrientation(const NTreeReader& curr, WidgetContainer& w)
{
   string o;
   curr.Read(o, "Orientation");
   QString qo = o.c_str();
   qo = qo.toLower();
   if (qo == "horizontal")
      w.orientation = WidgetContainer::Horizontal;
}


void whee::SetHost(const NTreeReader& curr, WidgetContainer& w)
{
   string host = "localhost";
   curr.ReadLine(host, "Host");
   QString qhost = host.c_str();
   w.host = qhost;
}


void whee::RunUpdates()
{
   for (list<WidgetContainerPtr>::iterator i = widgets.begin(); i != widgets.end(); ++i)
   {
      (*i)->remaining -= timer.interval();
      if ((*i)->remaining <= 0)
      {
         (*i)->Update();
         (*i)->remaining = (*i)->interval;
      }
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
   Atom net_wm_state_add = XInternAtom(display, "_NET_WM_STATE_ADD", false);
   Atom net_wm_state_sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", false);
   Atom net_wm_state_below = XInternAtom(display, "_NET_WM_STATE_BELOW", false);
   Atom net_wm_state_skip_taskbar = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", false);
   Atom net_wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", false);
   Atom net_wm_window_type_desktop = XInternAtom(display, "_NET_WM_TYPE_DESKTOP", false);
   Atom net_wm_desktop = XInternAtom(display, "_NET_WM_DESKTOP", false);
   
   long atoms[5];
   atoms[0] = net_wm_state_add;
   atoms[1] = net_wm_state_sticky;
   atoms[2] = net_wm_state_skip_taskbar;
   atoms[3] = net_wm_state_below;
   atoms[4] = 1;
   XChangeProperty(display, window, net_wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 5);
   
   long data[12];
   
   // Apparently KDE doesn't honor sticky state
   data[0] = 0xFFFFFFFF;
   data[1] = 1;
   XChangeProperty(display, window, net_wm_desktop, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)data, 2);
   
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


void whee::contextMenu(const QPoint& pos)
{
   QMenu* menu = new QMenu(this);
   menu->addAction("Reload", this, SLOT(reloadClicked()));
   menu->addAction("Close", this, SLOT(closeClicked()));
   menu->exec(mapToGlobal(pos));
}


void whee::reloadClicked()
{
   widgets.clear();
   delete background;
   ReadConfig();
}


void whee::closeClicked()
{
   close();
}

#include "whee.moc"
