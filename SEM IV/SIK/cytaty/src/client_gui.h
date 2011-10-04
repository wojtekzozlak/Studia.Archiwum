#ifndef _CLIENT_GUI_H_
#define _CLIENT_GUI_H_

#include <QWidget>
#include <QMap>
#include <QFutureWatcher>
#include <QFuture>
#include "client.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class QuotationsClientGui : public QWidget
/* Simple QuotationsClient Gui in a Qt4. */
{
  Q_OBJECT

  private:
    QLineEdit *hostLine;
    QLineEdit *portLine;
    QLabel *notice;
    QPushButton *getQuotationButton;
    QTextEdit *quotationText;
    QuotationsClient dbClient;
    QFutureWatcher< pair<bool, string> > quotationWatcher;
    QFuture< pair<bool, string> > quotationFuture;

  signals:
    void gotQuotation(string q);

  public slots:
    void getQuotation();
    pair<bool, string> getQuotationThread();
    void setQuotation();

  public:
    QuotationsClientGui(const char *hostname, const char *port,
                        QWidget *parent = 0);
};

#endif
