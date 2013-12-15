#include "DiskWidget.h"
#include "NTreeReader.h"
#include <sys/statvfs.h>
#include <iostream>

using std::cout;
using std::endl;

DiskWidget::DiskWidget(QLabel* l) : max(100), sectorsize(512), lastread(0), lastwrite(0)
{
   label = l;
}

void DiskWidget::Update()
{
   DoUpdate(false);
}
   
void DiskWidget::DoUpdate(bool force)
{
   struct statvfs stats;
   // The following calculations easily overflow 32-bit ints
   unsigned long long total, free, used, read, write;
   int elapsed;
   float localmax;
   
   // These stats only work locally, but you can always mount a remote filesystem to monitor it
   if (stat == Total || stat == Free || stat == Used)
   {
      statvfs(path.c_str(), &stats);
      // Theoretically someday it could be possible to overflow even a long long, so reduce
      // the numbers by a couple orders of magnitude and use KB instead of bytes
      total = stats.f_blocks / KB() * stats.f_frsize;
      free = stats.f_bavail / KB() * stats.f_frsize;
      used = (stats.f_blocks - stats.f_bfree) / KB() * stats.f_frsize;
      localmax = total;
   }
   else
   {
      NTreeReader reader(GetFile("/proc/diskstats"), 2);
      if (helper->Active() && !force)
         return;
      reader.Read(read, path, 2);
      reader.Read(write, path, 6);
      
      elapsed = timer.restart();
      // Convert to kilobytes for consistency with other stats - the numbers we read above are
      // number of sectors read/written from/to the disk, so need to account for sector size
      read /= (1024.f / sectorsize);
      write /= (1024.f / sectorsize);
      localmax = max * unit / KB();
   }
      
   unsigned long long val;
   float percent;
   if (stat == Total)
      val = total;
   else if (stat == Free)
      val = free;
   else if (stat == Used)
      val = used;
   else if (stat == Read)
      val = read - lastread;
   else if (stat == Write)
      val = write - lastwrite;
   else if (stat == ReadWrite)
      val = (read + write) - (lastread + lastwrite);
   
   if (stat == Read || stat == Write || stat == ReadWrite)
      val /= (float(elapsed) / 1000.f);
   
   
   percent = float(val) / localmax;
   val /= unit / KB();
   lastread = read;
   lastwrite = write;
   
   if (type == Text)
   {
      QString text = format;
      text = text.replace("%s", QString::number(val));
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


void DiskWidget::ProcessFinished(QString)
{
   DoUpdate(true);
}

