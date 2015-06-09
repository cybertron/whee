#include "MemoryWidget.h"
#include "NTreeReader.h"
#include <iostream>
#include <sys/sysinfo.h>

using std::cout;
using std::endl;

MemoryWidget::MemoryWidget(QLabel* l) : stat(Free)
{
   label = l;
}

void MemoryWidget::Update()
{
   if (GetFile("/proc/meminfo") == "/proc/meminfo")
      DoUpdate();
}
   
   
void MemoryWidget::DoUpdate()
{
   // Cool, /proc/meminfo is parsable by NTreeReader
   NTreeReader read(GetFile("/proc/meminfo", false));
   size_t val;
   size_t total, free, buffer, cache, totalswap, freeswap;
   total = totalswap = 1;
   free = buffer = cache = freeswap = 0;
   float percent;
   read.Read(total, "MemTotal:");
   read.Read(free, "MemFree:");
   read.Read(buffer, "Buffers:");
   read.Read(cache, "Cached:");
   read.Read(totalswap, "SwapTotal:");
   read.Read(freeswap, "SwapFree:");
   if (stat == Total)
   {
      val = total;
      percent = 100.f;
   }
   else if (stat == Free)
   {
      val = free + buffer + cache;
      percent = float(val) / float(total);
   }
   else if (stat == Used)
   {
      val = total - (free + buffer + cache);
      percent = float(val) / float(total);
   }
   else if (stat == TotalSwap)
   {
      val = totalswap;
      percent = 100.f;
   }
   else if (stat == FreeSwap)
   {
      val = freeswap;
      percent = float(val) / float(totalswap);
   }
   else if (stat == UsedSwap)
   {
      val = totalswap - freeswap;
      percent = float(val) / float(totalswap);
   }
   
   // /proc/meminfo gives values in KB, for the sake of converting it's easier to have it in bytes
   val = val * KB() / unit;
   
   if (type == Text)
   {
      QString text = format;
      text.replace("%s", QString::number(val));
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


void MemoryWidget::ProcessFinished(QString)
{
   DoUpdate();
}

