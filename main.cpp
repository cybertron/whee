#include <QtGui/QApplication>
#include "whee.h"

int main(int argc, char** argv)
{
   if (argc < 2)
   {
      cout << "Invalid number of arguments" << endl;
      exit(-1);
   }
   QApplication app(argc, argv);
   // As far as I can tell, we're okay doing this.  At the moment these objects can't be destroyed
   // until the application ends anyway, so the apparent memory leak here is not an issue.
   for (size_t i = 1; i < argc; ++i)
   {
      whee* foo = new whee(argv[i]);
      foo->show();
   }
   return app.exec();
}
