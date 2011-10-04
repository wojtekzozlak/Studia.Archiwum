#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include "database.h"

#define SQLCHECK(x, y, z) if(SQLITE_OK != x) throw QuotationsDBException(sqlite3_errmsg(y), z);

using namespace std;


static int as_array(void *acc, int argc, char **argv, char **azColName)
/* sqlite3_exec handler which converts sql result into a two-dimensional */
/* table of strings.                                                      */
{
  SqlResult *data = (SqlResult *) (acc);
  vector<string> row;
  int i;

  for(i = 0; i < argc; i++)
    row.push_back(argv[i]); // empty string is a NULL value
  data->push_back(row);
  return 0;
}

QuotationsDBException::QuotationsDBException(const char *db_msg,
                                             const char *sys_msg) : msg()
{ msg = string(sys_msg).append(":\n").append(db_msg); }


const char * QuotationsDBException::what() const throw()
{ return msg.c_str(); }


QuotationsDB::QuotationsDB(const char *filename) throw() : insert_stmt(NULL),
    delete_stmt(NULL)
{
  SQLCHECK(sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE, NULL),
           db, "database open");
}


QuotationsDB::~QuotationsDB()
{
  // if exception is thrown, we can only die, or call Aunt Tilda
  sqlite3_finalize(insert_stmt);
  sqlite3_finalize(delete_stmt);
  sqlite3_close(db);
}


void QuotationsDB::insert_quotation(string quotation)
{
  if(NULL == insert_stmt) // initialization
     SQLCHECK(sqlite3_prepare(db, "INSERT INTO quotations VALUES(NULL, :quo)",
              -1024, &insert_stmt, NULL),
     db, "insert prepare");
  SQLCHECK(sqlite3_reset(insert_stmt), db, "insert reset");
  SQLCHECK(sqlite3_clear_bindings(insert_stmt), db, "insert clear bindings")

  SQLCHECK(sqlite3_bind_text(insert_stmt, 1, quotation.c_str(),
                             quotation.size(), SQLITE_STATIC),
           db, "insert bind text");
  if(SQLITE_DONE != sqlite3_step(insert_stmt))
    throw QuotationsDBException(sqlite3_errmsg(db), "insert step");
}

pair<uint32_t, string> QuotationsDB::draw_quotation(uint32_t q_id)
{
  SqlResult data;
  char *sql = new char[200];
  char *err_msg = NULL;

  if(0 == q_id) /* Maybe it's*/
    sprintf(sql, "SELECT q_id, quotation FROM quotations ORDER BY Random() LIMIT 1");
  else
    sprintf(sql, "SELECT q_id, quotation FROM quotations WHERE q_id = %d", q_id);

  if(SQLITE_OK != sqlite3_exec(db, sql, as_array, &data, &err_msg))
     throw QuotationsDBException(err_msg, "draw quotations");

  delete sql;
  if(0 == data.size())
    throw NoSuchQuotationException();
  return make_pair(atoi(data[0][0].c_str()), data[0][1]);
}
