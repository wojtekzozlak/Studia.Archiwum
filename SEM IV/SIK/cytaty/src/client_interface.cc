/* --Quotations storage protocol client-- */

#include <QtGui>
#include "client_gui.h"

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    fprintf(stderr, "usage: %s hostname port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QApplication app(argc, argv);

  QuotationsClientGui addressBook(argv[1], argv[2]);
  addressBook.show();

  return app.exec();
}
