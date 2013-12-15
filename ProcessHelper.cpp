#include "ProcessHelper.h"
#include "WidgetContainer.h"

ProcessHelper::ProcessHelper() : active(false)
{
}

void ProcessHelper::Start(QString qcommand, WidgetContainer* w)
{
   widget = w;
   process = QProcessPtr(new QProcess());
   QObject::connect(&(*process), SIGNAL(readyReadStandardOutput()), this, SLOT(ReadOutput()));
   QObject::connect(&(*process), SIGNAL(readyReadStandardError()), this, SLOT(ReadError()));
   QObject::connect(&(*process), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ProcessFinished()));
   active = true;
   process->start("sh", QStringList() << "-c" << qcommand);
}

bool ProcessHelper::Active()
{
   return active;
}

void ProcessHelper::ReadOutput()
{
   QByteArray stdout = process->readAllStandardOutput();
   QString newtext(stdout);
   text += newtext;
}

void ProcessHelper::ReadError()
{
   return;
   QByteArray stderr = process->readAllStandardError();
   QString newtext(stderr);
   cout << newtext.toStdString() << endl;
}

void ProcessHelper::ProcessFinished()
{
   widget->ProcessFinished(text);
   text = "";
   active = false;
}

#include "ProcessHelper.moc"
