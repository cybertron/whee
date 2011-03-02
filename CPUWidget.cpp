#include "CPUWidget.h"
#include "NTreeReader.h"

CPUWidget::CPUWidget(QLabel* l) : lastjiffies(0), lasttotal(0)
{
   label = l;
}

void CPUWidget::Update()
{
   NTreeReader read("/proc/stat");
   
   size_t val;
   size_t user, nice, system, idle, io, irq, sirq, total;
   read.Read(user, "cpu", 0);
   read.Read(nice, "cpu", 1);
   read.Read(system, "cpu", 2);
   read.Read(idle, "cpu", 3);
   read.Read(io, "cpu", 4);
   read.Read(irq, "cpu", 5);
   read.Read(sirq, "cpu", 6);
   
   total = user + nice + system + idle + io + irq + sirq;
   
   if (stat == User)
      val = user;
   else if (stat == Nice)
      val = nice;
   else if (stat == System)
      val = system;
   else if (stat == IO)
      val = io;
   else if (stat == Idle)
      val = idle;
   else if (stat == Busy)
      val = user + nice + system + io + irq + sirq;
   
   int elapsed = timer.restart();
   
   size_t result = float(val - lastjiffies) / float(total - lasttotal) * 100.f;
   lastjiffies = val;
   lasttotal = total;
   
   if (type == Text)
   {
      QString text = format;
      text.replace("%s", QString::number(result));
      label->setText(text);
   }
   else if (type == Image)
   {
      SetLabelMask(float(result) / 100.f);
   }
   else if (type == Graph)
   {
      DrawGraph(float(result) / 100.f);
   }
}

