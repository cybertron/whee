#include "WidgetContainer.h"
#include <QPaintEngine>
#include <QFile>
#include <QTextStream>
#include <stdlib.h>

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
                                              interval(0),
                                              remaining(0)
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


// NOTE: It is the caller's responsibility to clean up the resulting temp file
QString WidgetContainer::GetFile(QString path, QString tempdir)
{
   QStringList list = path.split('/');
   QString filename = list[list.size() - 1];
   QString localpath = tempdir + "/" + filename;
   QString command = "ssh " + host + " cat " + path + " > " + localpath;
   system(command.toAscii().data());
   return localpath;
}


NTreeReader WidgetContainer::GetNTreeReader(QString path, size_t kl)
{
   if (host == "localhost")
      return NTreeReader(path.toStdString(), kl);
   QString tempdir = CreateTemp();
   QString localpath = GetFile(path, tempdir);
   NTreeReader retval(localpath.toStdString(), kl);
   RemoveTemp(tempdir);
   return retval;
}


QString WidgetContainer::GetFileContents(QString path)
{
   QString localpath;
   QString tempdir;
   if (host == "localhost")
      localpath = path;
   else
   {
      tempdir = CreateTemp();
      localpath = GetFile(path, tempdir);
   }
   QFile f(localpath);
   f.open(QIODevice::ReadOnly);
   QTextStream stream(&f);
   QString contents = stream.readAll();
   f.close();
   if (host != "localhost")
      RemoveTemp(tempdir);
   return contents;
}


QString WidgetContainer::CreateTemp()
{
   QString tmp = "/tmp/wheeXXXXXX";
   QString tempdir = mkdtemp(tmp.toAscii().data());
   return tempdir;
}


void WidgetContainer::RemoveTemp(QString path)
{
   QString command = "rm -rf " + path;
   system(command.toAscii().data());
}
