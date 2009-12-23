#include "DiskWidget.h"
#include <sys/statvfs.h>
#include <iostream>

using std::cout;
using std::endl;

DiskWidget::DiskWidget(QLabel* l)
{
   label = l;
}

void DiskWidget::Update()
{
   struct statvfs stats;
   // The following calculations easily overflow 32-bit ints
   unsigned long long total, free, used;
   
   statvfs(path.c_str(), &stats);
   // Theoretically someday it could be possible to overflow even a long long, so reduce
   // the numbers by a couple orders of magnitude and use KB instead of bytes
   total = stats.f_blocks / KB() * stats.f_frsize;
   free = stats.f_bavail / KB() * stats.f_frsize;
   used = (stats.f_blocks - stats.f_bfree) / KB() * stats.f_frsize;
   
   unsigned long long val;
   float percent;
   if (stat == Total)
      val = total;
   else if (stat == Free)
      val = free;
   else if (stat == Used)
      val = used;
   percent = float(val) / float(total);
   val /= unit / KB();
   
   if (type == Text)
   {
      QString text = format;
      text = text.replace("%s", QString::number(val));
      label->setText(text);
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

