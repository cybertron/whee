#include "NetworkWidget.h"
#include <QTextStream>
#include <QFile>
#include <iostream>
#include <math.h>

NetworkWidget::NetworkWidget(QLabel* l) : interface("eth0"), down(true), lastbytes(0)
{
   label = l;
}

void NetworkWidget::Update()
{
   QFile f("/proc/net/dev");
   f.open(QIODevice::ReadOnly);
   QTextStream stream(&f);
   QString contents = stream.readAll();
   contents.replace(":", " ");
   
   size_t val;
   QStringList l = contents.split(" ", QString::SkipEmptyParts);
   for (size_t i = 0; i < l.length(); ++i)
   {
      if (l[i].toStdString() == interface)
      {
         if (down)
            val = l[i + 1].toULong();
         else
            val = l[i + 9].toULong();
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

