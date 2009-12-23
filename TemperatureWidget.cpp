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
   if (process && process->state() != QProcess::NotRunning)
      process->kill(); // If it's still running from last time we need to end it immediately
   
   QString qcommand = "sensors | grep 'Core " + QString::number(core) + "' | perl -pe 's/(.*?\\+)([0-9]*)(.*)/\\2/'";
   qcommand = "sh -c \"" + qcommand + "\"";
   process = QProcessPtr(new QProcess());
   QObject::connect(&(*process), SIGNAL(readyReadStandardOutput()), this, SLOT(ReadOutput()));
   QObject::connect(&(*process), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ProcessFinished()));
   process->start(qcommand);
}


void TemperatureWidget::ReadOutput()
{
   QByteArray stdout = process->readAllStandardOutput();
   QString newtext(stdout);
   text += newtext;
}


void TemperatureWidget::ProcessFinished()
{
   float temp = text.toFloat();
   if (type == Text)
   {
      QString value = format;
      QString stemp;
      stemp.setNum(temp);
      value.replace("%s", stemp);
      label->setText(value);
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

#include "TemperatureWidget.moc"

