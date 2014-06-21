#ifndef XSQLITE_H
#define XSQLITE_H

#include <cstring>

#include <string>
#include <map>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <iterator>
#include <cassert>

#include <sqlite3.h>

#include "exception.h"

namespace TOOLS {

DEFINE_EXCEPTION(NoSuchColumnFound, BaseException);
DEFINE_EXCEPTION(SQLiteError, BaseException);
DEFINE_EXCEPTION(SQLQueryNotInited, BaseException);
DEFINE_EXCEPTION(SQLResultError, BaseException);
DEFINE_EXCEPTION(EmptySQLResultError, BaseException);

class SQLResult {
 public:
  bool done;
  bool started;

  uint cur_row;

  SQLResult(sqlite3_stmt* used_stmt);
  virtual ~SQLResult();

  bool next();

  int get_int(uint col);
  std::string get_string(uint col);
  long get_long(uint col);
  double get_double(uint col);

 private:
  sqlite3_stmt* stmt;
};

class XSQLite {
 public:
  XSQLite(const std::string& db_fn);
  virtual ~XSQLite();

  // prepare with arbitrary string
  bool init_raw_query(const std::string& q);

  // SELECT * FROM table LEFT JOIN t2 ON table.id = t2.id WHERE foo=blub
  // GROUP_BY t2.bla ORDER_BY foo
  // SELECT {what} FROM {table} [{join}] [WHERE {where}] [GROUP_BY {group_by}]
  // [ORDER_BY {order_by}]
  bool init_select(const std::string& table, const std::string& what = "*",
                   const std::string& join = "", const std::string& where = "",
                   const std::string& order_by = "",
                   const std::string& group_by = "",
                   const std::string& limit = "");
  SQLResult select();

  // INSERT INTO table(col1, col2, colN) VALUES (?, ?, ?)
  // INSERT INTO {table}({columns}) VALUES ("?"*{#columns})
  bool init_insert(const std::string& table, const tStringList& cols);
  bool insert();

  // UPDATE {table} SET {values} WHERE {where}
  bool init_update(const std::string& table, 
                   const tStringList& cols, const std::string& where = "");
  bool update();

  bool bind_int(const uint idx, const int& data);
  bool bind_string(const uint idx, const std::string& data);
  bool bind_long(const uint idx, const long& data);
  bool bind_double(const uint idx, const double& data);

  bool handle_err(int err_code);
  const uint col2idx(const std::string& name);

 private:
  typedef enum {
    _ready,
    _inited,
    _finished
  } tSQLState;

  std::string db_fn;
  sqlite3_stmt* stmt;
  sqlite3* db;

  tSQLState state;

  std::string tbl;
  tStringList insert_cols;
};

}  // end namspace TOOLS

#endif
