#include <QFile>
#include <QTextStream>
#include "ProcessHelper.h"
#include "WidgetContainer.h"

ProcessHelper::ProcessHelper() : active(false)
{
}

void ProcessHelper::Start(QString qcommand, WidgetContainer* w, QString outfile)
{
   command = qcommand;
   widget = w;
   ofile = outfile;
   process = QProcessPtr(new QProcess());
   QObject::connect(&(*process), SIGNAL(readyReadStandardOutput()), this, SLOT(ReadOutput()));
   QObject::connect(&(*process), SIGNAL(readyReadStandardError()), this, SLOT(ReadError()));
   QObject::connect(&(*process), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ProcessFinished(int, QProcess::ExitStatus)));
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
   QByteArray stderr = process->readAllStandardError();
   QString newtext(stderr);
   error += newtext;
}

void ProcessHelper::ProcessFinished(int retcode, QProcess::ExitStatus status)
{
   if (retcode == 0 && status == 0)
   {
      // Write output to the file specified by the caller
      if (ofile != "")
      {
         QFile f(ofile);
         if (f.open(QIODevice::WriteOnly))
         {
            QTextStream output(&f);
            output << text << endl;
         }
      }
      widget->ProcessFinished(text);
   }
   else
   {
      cout << "Command '" << command.toStdString() << "' failed with return code: " << retcode << endl;
      cout << error.toStdString() << endl;
   }
   text = "";
   error = "";
   active = false;
}

#include "ProcessHelper.moc"
