#include "WidgetContainer.h"
#include <QPaintEngine>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

unsigned long WidgetContainer::KBv = 1024;
unsigned long WidgetContainer::MBv = WidgetContainer::KB() * 1024;
unsigned long WidgetContainer::GBv = WidgetContainer::MB() * 1024;
unsigned long WidgetContainer::TBv = WidgetContainer::GB() * 1024;
unsigned long WidgetContainer::PBv = WidgetContainer::TB() * 1024;
unsigned long WidgetContainer::EBv = WidgetContainer::PB() * 1024;
unsigned long WidgetContainer::ZBv = WidgetContainer::EB() * 1024;
unsigned long WidgetContainer::YBv = WidgetContainer::ZB() * 1024;

WidgetContainer::WidgetContainer(QLabel* l) : label(l),
                                              type(Text),
                                              orientation(Vertical),
                                              lastpercent(0.f),
                                              interval(0),
                                              remaining(0),
                                              controlmaster(true)
{
}


WidgetContainer::WidgetContainer() : type(Text),
                                     orientation(Vertical),
                                     lastpercent(0.f),
                                     interval(0),
                                     remaining(0),
                                     controlmaster(true)
{
}


void WidgetContainer::SetLabelMask(float percent)
{
   int width = label->width();
   int height = label->height();
   
   if (orientation == Vertical)
      percent = 1.f - percent;
   
   // Apparently subtracting two equal QRegions leaves you with the original region - adding one to
   // both directions just unmasks parts of the image that don't exist, so it shouldn't be a problem
   QRegion region(-1, 0, width, height + 1);
   if (orientation == Vertical)
   {
      region -= QRegion(0, 0, width, float(height) * percent);
   }
   else
   {
      region -= QRegion(float(width) * percent, 0, width, height);
   }
   label->setMask(region);
}


void WidgetContainer::DrawGraph(float percent)
{
   percent = 1.f - percent;
   if (percent < 0.f)
      percent = 0.f;
   float width = label->width();
   float height = label->height();
   const QPixmap* p = label->pixmap();
   QPixmap pixmap(width, height);
   if (p)
      pixmap = *p;
   else
      pixmap.fill(Qt::transparent);
   
   QPixmap newp(width, height);
   newp.fill(Qt::transparent);
   
   QPainter painter(&newp);
   QPalette palette = label->palette();
   QPen pen(palette.color(QPalette::WindowText));
   // Scroll the old image to the left one pixel
   painter.drawPixmap(0, 0, pixmap, 1, 0, 0, 0);
   
   int lastpos = clamp(height * lastpercent, 1.f, height - 1.f);
   int currpos = clamp(height * percent, 1.f, height - 1.f);;
   QLine line(width - 2, lastpos, width - 1, currpos);
   painter.setPen(palette.color(QPalette::WindowText));
   painter.drawLine(line);
   QLine baseline(0, height - 1, width, height - 1);
   painter.drawLine(baseline);
   painter.end();
   lastpercent = percent;
   
   label->setPixmap(newp);
}


/* This function is a vicious hack to deal with the fact that changing the label text in Qt 4.7 results in
 * an irritating ghost flicker where everything overlapping the label seems to show double for an instant.
 */
void WidgetContainer::SetText(QString text)
{
   label->hide();
   label->setText(text);
   label->show();
}


string WidgetContainer::GetFile(QString path, bool startproc)
{
   InitProcessHelper();
   if (host == "localhost")
      return path.toStdString();
   
   QString localpath = temppath + "/" + host + path;
   QFileInfo local(localpath);
   QDir("/").mkpath(local.dir().path());
   if (!helper->Active() && startproc)
   {
      QStringList opts;
      // Bleh.  This isn't playing nicely with rhel 7.1 right now.  Allow shutting it off.
      if (controlmaster)
      {
         opts << "ControlMaster=auto";
         opts << "ControlPersist=60";
         opts << "ControlPath=" + temppath + "/%r@%h:%p";
      }
      opts << "PasswordAuthentication=no";
      opts << "StrictHostKeyChecking=no";
      opts << "UserKnownHostsFile=/dev/null";
      QString optstring = "";
      for (size_t i = 0; i < opts.size(); ++i)
      {
         optstring += " -o " + opts[i];
      }
      if (idfile != "")
      {
         optstring += " -i " + idfile;
      }
      if (username != "")
      {
         optstring += " -l " + username;
      }
      QString ssh = "ssh" + optstring + " ";
      QString command = ssh + host + " cat " + path;
      helper->Start(command, this, localpath);
   }
   QFile f(localpath);
   if (f.exists())
      return localpath.toStdString();
   return "/dev/null";
}


void WidgetContainer::InitProcessHelper()
{
   if (!helper)
      helper = ProcessHelperPtr(new ProcessHelper());
}
