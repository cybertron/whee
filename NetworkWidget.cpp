#include "NetworkWidget.h"
#include <iostream>
#include <math.h>
#include <QFile>
#include <QTextStream>

NetworkWidget::NetworkWidget(QLabel* l) : interface("eth0"), down(true), lastbytes(0)
{
   label = l;
}

void NetworkWidget::Update()
{
   GetFile("/proc/net/dev");
   if (helper->Active())
      return;
   DoUpdate();
}
   
void NetworkWidget::DoUpdate()
{
   QFile f(QString(GetFile("/proc/net/dev").c_str()));
   f.open(QIODevice::ReadOnly);
   QTextStream stream(&f);
   QString contents = stream.readAll();
   f.close();
   contents.replace(":", " ");
   contents.replace("\n", " ");
   
   size_t val = 0;
   QStringList l = contents.split(" ", QString::SkipEmptyParts);
   for (size_t i = 0; i < l.length(); ++i)
   {
      if (l[i].toStdString() == interface)
      {
         if (down)
         {
            // On 32-bit archs it's possible to overflow size_t, so truncate to only the
            // last nine digits.  If you need to be able to measure more than a gigabyte of
            // bandwidth, switch to 64-bits. :-)
            if (sizeof(val) > 4)
               val = l[i + 1].toULong();
            else
               val = l[i + 1].right(9).toULong();
         }
         else
         {
            if (sizeof(val) > 4)
               val = l[i + 9].toULong();
            else
               val = l[i + 9].right(9).toULong();
         }
         break;
      }
   }
   size_t currbytes = val;
   int elapsed = timer.restart();
   float rate = (currbytes - lastbytes) / (float(elapsed) / 1000.f) / float(KB());
   rate = floor(rate * 10.f) / 10.f;
   lastbytes = currbytes;
   float percent = rate / max;
   
   if (type == Text)
   {
      QString qrate;
      qrate.setNum(rate);
      QString text = format;
      text.replace("%s", qrate);
      SetText(text);
   }
   else if (type == Image)
   {
      SetLabelMask(percent);
   }
   else if (type == Graph)
   {
      DrawGraph(percent);
   }
}

void NetworkWidget::ProcessFinished(QString)
{
   DoUpdate();
}
