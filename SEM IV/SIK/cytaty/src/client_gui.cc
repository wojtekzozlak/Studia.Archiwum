#include <QtGui>
#include <QWidget>
#include <QMap>
#include "client_gui.h"
#include "client.h"
#include "unistd.h"

QuotationsClientGui::QuotationsClientGui(const char *hostname, const char *port,
                                         QWidget *parent) : QWidget(parent)
{
  QLabel *hostLabel = new QLabel(tr("Hostname:"));
  QLabel *portLabel = new QLabel(tr("Port:"));
  notice = new QLabel();
  hostLine = new QLineEdit(hostname);
  portLine = new QLineEdit(port);
  getQuotationButton = new QPushButton(tr("&Get Quotation"));
  quotationText = new QTextEdit();

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(hostLabel, 0, 0);
  mainLayout->addWidget(hostLine, 0, 1);
  mainLayout->addWidget(portLabel, 1, 0);
  mainLayout->addWidget(portLine, 1, 1);
  mainLayout->addWidget(notice, 2, 1, 1, 2);
  mainLayout->addWidget(quotationText, 3, 0, 1, 3);
  mainLayout->addWidget(getQuotationButton, 4, 1);

  connect(getQuotationButton, SIGNAL(clicked()), this, SLOT(getQuotation()));

  setLayout(mainLayout);
  setWindowTitle(tr("Quotations Client"));
}

void QuotationsClientGui::setQuotation()
/* Shows a quotation in the GUI. */
{
  pair<bool, string> ret = quotationFuture.result();
  if(ret.first)
  {
    quotationText->setPlainText(QString::fromStdString(ret.second));
    notice->setText(tr("Success!"));
  }
  else
    notice->setText(tr("Error occured."));
  getQuotationButton->setEnabled(true);
  return ;
}

pair<bool, string> QuotationsClientGui::getQuotationThread()
/* Gets a quotation from a server. */
{
  bool success = true;
  string quotation;
  try
  {
    dbClient.prepare(hostLine->text().toLocal8Bit().data(),
                     atoi(portLine->text().toLocal8Bit().data()), 2);
    quotation = dbClient.getQuotation();
  }
  catch(QuotationsClientException &e)
  { success = false; }
  return make_pair(success, quotation);
}

void QuotationsClientGui::getQuotation()
/* Sets a thread which gets a quotation from a server. */
{
  notice->setText(tr("Processing..."));
  getQuotationButton->setEnabled(false);
  connect(&quotationWatcher, SIGNAL(finished()), this, SLOT(setQuotation()));
  quotationFuture = QtConcurrent::run(this,
                                      &QuotationsClientGui::getQuotationThread);
  quotationWatcher.setFuture(quotationFuture);
  return ;
}
