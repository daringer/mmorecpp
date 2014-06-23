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

namespace helper {
  // bind type-dispatcher helper classes 
  template<class T> struct bind_dispatch;

  template<> struct bind_dispatch<std::string> {
    static int apply(sqlite3_stmt* stmt, const std::string& val, const int& idx) {
      return sqlite3_bind_text(stmt, idx, val.c_str(),
          val.length(), SQLITE_TRANSIENT);
    }
  };
  template<> struct bind_dispatch<long> {
    static int apply(sqlite3_stmt* stmt, const long& val, const int& idx) {
      return sqlite3_bind_int64(stmt, idx, val);
    }
  };
  template<> struct bind_dispatch<int> {
    static int apply(sqlite3_stmt* stmt, const int& val, const int& idx) {
      return sqlite3_bind_int(stmt, idx, val);
    }
  };
  template<> struct bind_dispatch<double> {
    static int apply(sqlite3_stmt* stmt, const double& val, const int& idx) {
      return sqlite3_bind_double(stmt, idx, val);
    }
  };
  // get type-dispatcher helper classes
  template<class T> struct get_dispatch;

  template<> struct get_dispatch<double> {
    static double apply(sqlite3_stmt* stmt, const uint& col) {
      return sqlite3_column_double(stmt, col);
    }
  };
  template<> struct get_dispatch<std::string> {
    static std::string apply(sqlite3_stmt* stmt, const uint& col) {
      return (const char*) sqlite3_column_text(stmt, col);
    }
  };
  template<> struct get_dispatch<int> {
    static int apply(sqlite3_stmt* stmt, const uint& col) {
      return sqlite3_column_int(stmt, col);
    }
  };
  template<> struct get_dispatch<long> {
    static long apply(sqlite3_stmt* stmt, const uint& col) {
      return sqlite3_column_int64(stmt, col);
    }
  };
}

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

  // get value from col
  template<class T> T get(const uint& col) {
    assert(started);
    return helper::get_dispatch<T>::apply(stmt, col);
  };

 private:
  sqlite3_stmt* stmt;
};

class XSQLite {
 public:
  // next column idx to be used on bind() without idx
  uint next_idx;
  // last inserted row id
  long last_insert_id;

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

  // PRAGMA statement (init included)
  bool pragma(const std::string& key, const std::string& val);

  // reset all query related members
  void reset();

  // binding values to sql-query idx == -1 means 'next_idx'
  template<class T> bool bind(const T& data, const int& idx=-1) {
    int i = (idx == -1) ? ++next_idx : idx + 1;
    int ret = helper::bind_dispatch<T>::apply(stmt, data, i);
    return handle_err(ret);
  };
  // dummy bind - to bind a NULL 
  bool bind();

  /*bool bind_int(const uint idx, const int& data);
  bool bind_string(const uint idx, const std::string& data);
  bool bind_long(const uint idx, const long& data);
  bool bind_double(const uint idx, const double& data);
  
  bool bind_int(const int& data);
  bool bind_string(const std::string& data);
  bool bind_long(const long& data);
  bool bind_double(const double& data);*/

  /*bool bind_int(const tIntList& data);
  bool bind_string(const tStringList& data);
  bool bind_long(const tLongList& data);
  bool bind_double(const tDoubleList& data);*/

  // load / save(backup) / exec sql-file on db
  sqlite3* load_db(const std::string& fn);
  bool save_db(const std::string& fn);
  bool exec_sql_fn(const std::string& fn);

  // sqlite error handler, throws error on critical
  bool handle_err(int err_code);
  const uint col2idx(const std::string& name);

 private:
  typedef enum {
    _ready,
    _inited,
    _finished
  } tSQLState;
  tSQLState state;

  // db-file and internals
  std::string db_fn;
  sqlite3_stmt* stmt;
  sqlite3* db;
  std::string last_query;

  // target-table
  std::string tbl;

  // columns to be inserted/updated
  tStringList columns;
};

}  // end namspace TOOLS

#endif
