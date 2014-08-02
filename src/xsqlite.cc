#include <string>
#include <iostream>
#include <fstream>

#include "general.h"
#include "xstring.h"
#include "xsqlite.h"

using namespace std;
using namespace TOOLS;

SQLResult::SQLResult(sqlite3_stmt* used_stmt)
    : done(false), started(false), cur_row(0), stmt(used_stmt) {}

SQLResult::~SQLResult() { }

bool SQLResult::next() {
  int ret = sqlite3_step(stmt);
  if (ret == SQLITE_ROW) {
    cur_row++;
    started = true;
    done = false;
    return true;
  } else if (ret == SQLITE_DONE) {
    done = true;
    return false;
  } else
    throw SQLResultError(ret);
}

int SQLResult::get_int(uint col) {
  assert(started);
  return sqlite3_column_int(stmt, col);
}

std::string SQLResult::get_string(uint col) {
  assert(started);
  std::string s = TOOLS::str(sqlite3_column_text(stmt, col));
  return s;
}

long SQLResult::get_long(uint col) {
  assert(started);
  return sqlite3_column_int64(stmt, col);
}

double SQLResult::get_double(uint col) {
  assert(started);
  return sqlite3_column_double(stmt, col);
}

XSQLite::XSQLite(const std::string& db_fn)
    : next_idx(0), last_insert_id(-1),
      state(_ready), db_fn(db_fn), stmt(nullptr), 
      db(load_db(db_fn)) { }

XSQLite::~XSQLite() {
  // i think this _may_ fail, but destruction never "fails" ;D
  sqlite3_close(db);
}

bool XSQLite::pragma(const std::string& key, const std::string& val) {
  reset();

  // init query
  const string sql = "PRAGMA " + key + " = " + val + "";
  last_query = sql;
  handle_err(init_raw_query(sql) == SQLITE_OK);
  
  // do it!
  int ret = sqlite3_step(stmt);

  state = _finished;
  handle_err(sqlite3_finalize(stmt));

  if (ret != SQLITE_DONE)
    return handle_err(ret);
  return true;
}

bool XSQLite::bind() {
  return handle_err(sqlite3_bind_null(stmt, next_idx++ + 1));
}

bool XSQLite::init_update(const std::string& table, 
    const tStringList& cols, const std::string& where) {
  reset();

  tbl = table;
  columns = cols;

  string kv_pairs = "";
  for(const string& col : cols) 
    kv_pairs += col + " = ?, ";
  kv_pairs = kv_pairs.substr(0, kv_pairs.length() - 2);
  string sql = "UPDATE " + table + " SET " + kv_pairs;
  if(!where.empty())
    sql += (" WHERE " + where);
  
  int ret = init_raw_query(sql);
  state = _inited;
  return ret;
}

bool XSQLite::update() {
  if (state != _inited)
    throw SQLQueryNotInited("update() called, but no sql inited!");

  int ret = sqlite3_step(stmt);
  if (ret != SQLITE_DONE)
    return handle_err(ret);

  state = _finished;
  handle_err(sqlite3_finalize(stmt));
  return true;
}

bool XSQLite::handle_err(int err_code) {
  if (err_code != SQLITE_OK) {
    const char* msg = sqlite3_errmsg(db);
    if(last_query.empty())
      throw SQLiteError(msg);
    else
      throw SQLiteError(last_query + " err: " + msg);
  }
  return true;
}

const uint XSQLite::col2idx(const std::string& name) {
  auto it = std::find(columns.begin(), columns.end(), name);

  if (it == columns.end())
    throw NoSuchColumnFound(name);

  return std::distance(columns.begin(), it);
}

bool XSQLite::insert() {
  if (state != _inited)
    throw SQLQueryNotInited("insert() called but no sql inited!");

  int ret = sqlite3_step(stmt);
  if (ret != SQLITE_DONE)
    return handle_err(ret);

  last_insert_id = sqlite3_last_insert_rowid(db);

  state = _finished;
  handle_err(sqlite3_finalize(stmt));

  return true;
}

bool XSQLite::init_insert(const std::string& table, const tStringList& cols) {
  reset();

  tbl = table;
  columns = cols;

  string tmp, colstr;
  for (const auto& col : cols) {
    colstr += (col + ",");
    tmp += "?,";
  }
  tmp = tmp.substr(0, tmp.length() - 1);
  colstr = colstr.substr(0, colstr.length() - 1);
  const string sql =
      "INSERT INTO " + table + "(" + colstr + ") VALUES (" + tmp + ")";

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
                          const std::string& order_by,
                          const std::string& group_by,
                          const std::string& limit) {
  reset();

  tbl = table;

  string sql = "SELECT " + what + " FROM " + table;
  if (!join.empty())
    sql += " " + join;
  if (!where.empty())
    sql += " WHERE " + where;
  if (!group_by.empty())
    sql += " GROUP_BY " + group_by;
  if (!order_by.empty())
    sql += " ORDER_BY " + order_by;
  if (!limit.empty())
    sql += " LIMIT " + order_by;

  int ret = init_raw_query(sql);
  state = _inited;
  handle_err(sqlite3_finalize(stmt));
  return ret;
}

void XSQLite::reset() {
  columns.clear();
  tbl = "";
  state = _ready;
  next_idx = 0;
}

bool XSQLite::init_raw_query(const std::string& q) {
  last_query = q;
  int ret = handle_err(
      sqlite3_prepare_v2(db, q.c_str(), q.length() + 1, &stmt, NULL));
  state = _inited;
  return ret;
}

sqlite3* XSQLite::load_db(const std::string& fn) {
  sqlite3* mydb;
  int ret = sqlite3_open(fn.c_str(), &mydb);
  handle_err(ret);
  return mydb;
}

bool XSQLite::save_db(const std::string& fn) {
  sqlite3* to_db = load_db(fn);
  sqlite3_backup* bak = sqlite3_backup_init(to_db, "main", db, "main");
  if(bak) {
    sqlite3_backup_step(bak, -1);
    sqlite3_backup_finish(bak);
  }
  return handle_err(sqlite3_errcode(to_db));
}

bool XSQLite::exec_sql_fn(const std::string& fn) {
  //string q = get_file_contents("database_scheme.sql");
  ifstream sql_fd(fn);
  string q((istreambuf_iterator<char>(sql_fd)), (istreambuf_iterator<char>()));
  int ret = sqlite3_exec(db, q.c_str(), NULL, NULL, NULL);
  return handle_err(ret);
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
    cout << res.get_string(0) << " " << res.get_string(1) << " " <<
res.get_int(2) << endl;
  }

  return 0;

}
*/
