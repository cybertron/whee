#include "TemperatureWidget.h"
#include <iostream>

using std::cout;
using std::endl;

TemperatureWidget::TemperatureWidget(QLabel* l)
{
   label = l;
}

void TemperatureWidget::Update()
{
   InitProcessHelper();
   if (helper->Active())
      return;
   
   QString remote = "";
   if (host != "localhost")
      remote = "ssh " + host + " ";
   QString qcommand = remote + "sensors " + QString(chip.c_str()) + " | grep '" + QString(tempid.c_str()) + "' | perl -pe 's/(.*?\\+)([0-9]*)(.*)/\\2/'";
   helper->Start(qcommand, this);
}


void TemperatureWidget::ProcessFinished(QString text)
{
   float temp = text.toFloat();
   if (type == Text)
   {
      QString value = format;
      QString stemp;
      stemp.setNum(temp);
      value.replace("%s", stemp);
      SetText(value);
   }
   else if (type == Image)
   {
      SetLabelMask(float(temp) / max);
   }
   else if (type == Graph)
   {
      DrawGraph(float(temp) / max);
   }
   text = "";
}
