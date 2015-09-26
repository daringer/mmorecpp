#pragma once

#include <curses.h>

#include <vector>

#include "../core/general.h"
#include "../core/exception.h"
#include "../core/xstring.h"

namespace MM_NAMESPACE() {

DEFINE_EXCEPTION(ColumnIndexOutOfRange, BaseException);
DEFINE_EXCEPTION(ContentLengthNotMatching, BaseException);

class XNCBaseWindow;

typedef std::map<std::string, XNCBaseWindow*> tWindowsStorage;
typedef std::vector<char> tCharList;

typedef struct {
  uint x, y;
} tCoords;

typedef std::vector<tStringList> tStringStringList;

// pair-create-helper
tCoords make_coords(const uint& x, const uint& y);

// Abstract NCurses Window/Widget,
// all others should derive from it!
class XNCBaseWindow {
 public:
  XNCBaseWindow(const tCoords& origin, const tCoords& size);
  virtual ~XNCBaseWindow();

  virtual void clear();
  virtual void update() = 0;
  virtual void set_formatting(attr_t attrs);
  virtual void set_layout();  // const tCharList& cl);

  virtual void change_size(const tCoords& new_pos, const tCoords& new_size);

  virtual const tCoords& get_pos();
  virtual const tCoords& get_size();

 protected:
  tCoords pos;
  tCoords size;

  WINDOW* _win;

  tCharList* layout;
};

// headline-content is fully static
class XNCHeadline : public XNCBaseWindow {
 public:
  XNCHeadline(const std::string& text);
  virtual ~XNCHeadline();

  virtual void update();

  void set_headline(const std::string& title);

 private:
  std::string cur_headline;
};

// statusbar-content is ordered by an idx
class XNCStatusBar : public XNCBaseWindow {
 public:
  XNCStatusBar(const uint& cols, const tCoords& pos, const tCoords& size);
  virtual ~XNCStatusBar();

  virtual void update();

  void set_content(const uint& idx, const std::string& s);
  void set_content(const tStringList& sl);

 private:
  tStringList data;
  uint content_cols;
};

// table with direct cell access
class XNCTable : public XNCBaseWindow {
 public:
  XNCTable(const uint& cols, const tCoords& pos, const tCoords& size);
  virtual ~XNCTable();

  virtual void update();

  // each idx here means a LINE! internally stored like that: data[y][x]
  void set_headers(const tStringList& sl);
  void set_data(const uint& idx, const tStringList& sl);
  void set_data(const tStringStringList& ssl);

  void push_data(const tStringList& sl);
  void del_data(const uint& idx);

 private:
  tStringList head;
  tStringStringList data;
  uint content_cols;
};

// window that scrolls down, keeping the most recent at top
// as simple as possible, also for debugging
class XNCScrolling : public XNCBaseWindow {
 public:
  XNCScrolling(const uint& lines, const tCoords& pos, const tCoords& size);
  virtual ~XNCScrolling();

  virtual void update();

  void push_row(const std::string& s);
  // void set_row(const std::string& data, const uint& y);
  // void del_row(const uint& y);

 private:
  tStringList data;
  uint lines;
};

// something that moves ... mmmh
class XNCActivityIndicator : public XNCBaseWindow {
 public:
  XNCActivityIndicator(const tCoords& pos);
  virtual ~XNCActivityIndicator();

  virtual void update();
};

// XNCurses - a "widget" set for curses
class XNC {
 public:
  XNC();
  virtual ~XNC();

  void add_window(const std::string& id, XNCBaseWindow* obj);
  void update();

 private:
  tWindowsStorage name2window;
};
}

