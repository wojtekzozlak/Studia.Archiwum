#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <string>
#include <vector>
#include <sqlite3.h>
using namespace std;

typedef vector< vector<string> > SqlResult;

class QuotationsDBException : public std::exception
/* Standard exception for QuotationsDB related problems. */
{
  private:
    string msg;

  public:
    QuotationsDBException(const char *, const char *);
    ~QuotationsDBException() throw() {};

    const char * what() const throw();
};

class NoSuchQuotationException : public std::exception
{};


class QuotationsDB
/* Simple quotations database based on SQLite3. */
{
  private:
    sqlite3 *db;
    sqlite3_stmt *insert_stmt;
    sqlite3_stmt *delete_stmt;

  public:
    QuotationsDB(const char *filename) throw();
    ~QuotationsDB();

    void insert_quotation(string quotation);
    /* Inserts a quotation to a database. */
    
    pair<uint32_t, string> draw_quotation(uint32_t q_id = 0);
    /* Draws a quotation from a database. If [q_id] is not specified */
    /* a random quotation is drawn.                                  */
};


#endif
