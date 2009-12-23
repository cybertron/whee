#include "CommandWidget.h"
#include <iostream>

using std::cout;
using std::endl;

CommandWidget::CommandWidget(QLabel* l)
{
   label = l;
}

void CommandWidget::Update()
{
   if (process && process->state() != QProcess::NotRunning)
      process->kill(); // If it's still running from last time we need to end it immediately
   
   QString qcommand = command.c_str();
   qcommand = "sh -c \"" + qcommand + "\"";
   process = QProcessPtr(new QProcess());
   QObject::connect(&(*process), SIGNAL(readyReadStandardOutput()), this, SLOT(ReadOutput()));
   QObject::connect(&(*process), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ProcessFinished()));
   process->start(qcommand);
}


void CommandWidget::ReadOutput()
{
   QByteArray stdout = process->readAllStandardOutput();
   QString newtext(stdout);
   text += newtext;
}


void CommandWidget::ProcessFinished()
{
   label->setText(text);
   text = "";
}

#include "CommandWidget.moc"

