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
   InitProcessHelper();
   if (helper->Active())
      return;
   
   QString qcommand = command.c_str();
   helper->Start(qcommand, this);
}


void CommandWidget::ProcessFinished(QString text)
{
   SetText(text);
}
