#include <string>
#include <iostream>

#include "general.h"
#include "xstring.h"
#include "xsqlite.h"

using namespace std;
using namespace TOOLS;


SQLResult::SQLResult(sqlite3_stmt* used_stmt) 
    : done(false), started(false), cur_row(0), stmt(used_stmt) { 
}

SQLResult::~SQLResult() { }

bool SQLResult::next() {
  int ret = sqlite3_step(stmt);
  if(ret == SQLITE_ROW) {
    cur_row++;
    started = true;
    done = false;
    return true;
  } else if(ret == SQLITE_DONE) {
    done = true;
    return false;
  } else
    throw SQLResultError(ret);
}

int SQLResult::get_int(uint col) {
  assert(started);
  return sqlite3_column_int(stmt, col);
}

std::string SQLResult::get_string(uint col){
  assert(started);
  std::string s = TOOLS::str(sqlite3_column_text(stmt, col));
  return s;
}

long SQLResult::get_long(uint col){
  assert(started);
  return sqlite3_column_int64(stmt, col);
}

double SQLResult::get_double(uint col){
  assert(started);
  return sqlite3_column_double(stmt, col);
}


XSQLite::XSQLite(const std::string& db_fn) 
  : db_fn(db_fn), stmt(nullptr), db(nullptr), state(_ready) {

  handle_err(sqlite3_open(db_fn.c_str(), &db));
}

XSQLite::~XSQLite() { }

bool XSQLite::bind_int(const uint idx, const int& data) {
  return handle_err(sqlite3_bind_int(stmt, idx+1, data));
}

bool XSQLite::bind_string(const uint idx, const std::string& data) {
  return handle_err(sqlite3_bind_text(stmt, idx+1, data.c_str(), data.length(), SQLITE_TRANSIENT));
}

bool XSQLite::bind_long(const uint idx, const long& data) {
  return handle_err(sqlite3_bind_int64(stmt, idx+1, data));
}

bool XSQLite::bind_double(const uint idx, const double& data) {
  return handle_err(sqlite3_bind_double(stmt, idx+1, data));
}

bool XSQLite::handle_err(int err_code) {
  if(err_code != SQLITE_OK) {
    const char* msg = sqlite3_errmsg(db);
    cerr << "[SQLite3 err]: " << msg << endl;
    throw SQLiteError(msg);
  }
  return true;
}

const uint XSQLite::col2idx(const std::string& name) {
  auto it = std::find(insert_cols.begin(), insert_cols.end(), name);

  if(it == insert_cols.end())
    throw NoSuchColumnFound(name);
  
  return std::distance(insert_cols.begin(), it);
}

bool XSQLite::insert() {
  if(state != _inited)
    throw SQLQueryNotInited("insert() called but no sql inited!");
  
  int ret = sqlite3_step(stmt);
  if (ret != SQLITE_DONE)
    return handle_err(ret);

  state = _finished;
  return true;
}

bool XSQLite::init_insert(const std::string& table, const tStringList& cols) {
  state = _ready;
  tbl = table;
  insert_cols = cols;


  string tmp, colstr;
  for(const auto& col : cols) {
    colstr += (col + ",");
    tmp += "?,";
  }
  tmp = tmp.substr(0, tmp.length() - 1); 
  colstr = colstr.substr(0, colstr.length() - 1);
  const string sql = "INSERT INTO " + table + "(" + colstr + ") VALUES (" + tmp + ")";
  int ret = init_raw_query(sql);

  state = _inited;
  return ret;
}

SQLResult XSQLite::select() {
  state = _ready;
  return SQLResult(stmt);
}

bool XSQLite::init_select(const std::string& table, const std::string& what,
    const std::string& join, const std::string& where, 
    const std::string& order_by, const std::string& group_by,
    const std::string& limit) {
  
  insert_cols.clear();
  tbl = table;

  string sql = "SELECT " + what + " FROM " + table;
  if(!join.empty())
    sql += " " + join;
  if(!where.empty()) 
    sql += " WHERE " + where;
  if(!group_by.empty())
    sql += " GROUP_BY " + group_by;
  if(!order_by.empty())
    sql += " ORDER_BY " + order_by;
  if(!limit.empty())
    sql += " LIMIT " + order_by;

  int ret = init_raw_query(sql);
  state = _inited;
  return ret;
}

bool XSQLite::init_raw_query(const std::string& q) {
  int ret = handle_err(sqlite3_prepare_v2(db, q.c_str(), q.length() + 1, &stmt, NULL));

  state = _inited;
  return ret;
}

/*
int main() {

  cout << "FOOO" << endl;
  
  XSQLite db("test.db");
  db.init_insert("t", {"foo", "bla", "blub"});
  db.bind_string(0, "MARKUS");
  db.bind_string(1, "MEISSNER");
  db.bind_int(2, 555);
  cout << db.insert() << endl;

  db.init_select("t");
  SQLResult res = db.select();
  while(res.next()) {
    cout << res.get_string(0) << " " << res.get_string(1) << " " << res.get_int(2) << endl;
  }

  return 0;

}
*/

