#include "xncurses.h"

#include <ncurses.h>
#include <unistd.h>

using namespace std;
using namespace TOOLS;

tCoords TOOLS::make_coords(const uint& x, const uint& y) {
  tCoords out;
  out.x = x;
  out.y = y;
  return out;
}

XNCBaseWindow::XNCBaseWindow(const tCoords& pos, const tCoords& size)
    : pos(pos),
      size(size),
      _win(newwin(size.y, size.x, pos.y, pos.x)),
      layout(nullptr) {

  layout = new tCharList({'|', '|', '-', '-', '+', '+', '+', '+'});
}

XNCBaseWindow::~XNCBaseWindow() {}

void XNCBaseWindow::set_layout() {  // const tCharList& cl) {
  wborder(_win, layout->at(0), layout->at(1), layout->at(2), layout->at(3),
          layout->at(4), layout->at(5), layout->at(6), layout->at(7));
  update();
  /* The parameters taken are
   * 1. win: the window on which to operate
   * 2. ls: character to be used for the left side of the window
   * 3. rs: character to be used for the right side of the window
   * 4. ts: character to be used for the top side of the window
   * 5. bs: character to be used for the bottom side of the window
   * 6. tl: character to be used for the top left corner of the window
   * 7. tr: character to be used for the top right corner of the window
   * 8. bl: character to be used for the bottom left corner of the window
   * 9. br: character to be used for the bottom right corner of the window
   */
}

void XNCBaseWindow::clear() { wclear(_win); }

void XNCBaseWindow::set_formatting(attr_t attrs) {
  // then here the color and attr shit
  // set color-pairs
  // init_pair(1, COLOR_RED, COLOR_BLUE);
  // activate format/style and color-pair now!
  // attron(A_UNDERLINE | COLOR_PAIR(1));
  // attron(A_BOLD);
  // switch if off again
  // attroff(A_UNDERLINE | COLOR_PAIR(1));

  set_layout();
  wattron(_win, attrs);
  update();
}

void XNCBaseWindow::change_size(const tCoords& new_pos,
                                const tCoords& new_size) {
  wresize(_win, new_size.y, new_size.x);
  mvderwin(_win, new_pos.x - pos.x, new_pos.y - pos.y);

  pos = pos;
  size = size;

  update();
}

const tCoords& XNCBaseWindow::get_pos() { return pos; }

const tCoords& XNCBaseWindow::get_size() { return size; }

XNCHeadline::XNCHeadline(const string& text)
    : XNCBaseWindow(make_coords(0, 0), make_coords(COLS, 3)),
      cur_headline(text) {

  set_formatting(A_BOLD);
  update();
}

XNCHeadline::~XNCHeadline() {}

void XNCHeadline::set_headline(const string& title) {
  cur_headline = title;
  update();
}

void XNCHeadline::update() {
  mvwaddstr(_win, 1, (COLS - cur_headline.length()) / 2, cur_headline.c_str());
  wrefresh(_win);
}

XNCTable::XNCTable(const uint& cols, const tCoords& pos, const tCoords& size)
    : XNCBaseWindow(pos, size), content_cols(cols) {

  data.clear();  // data[ROWIDX][COLIDX]
}

XNCTable::~XNCTable() {}

void XNCTable::set_headers(const tStringList& sl) { head = sl; }

void XNCTable::set_data(const uint& idx, const tStringList& sl) {
  data.insert(data.begin() + idx, sl);
}

void XNCTable::set_data(const tStringStringList& ssl) { data = ssl; }

void XNCTable::push_data(const tStringList& sl) {
  if (sl.size() == content_cols)
    data.push_back(sl);
  else
    throw ContentLengthNotMatching(sl.size());
}

void XNCTable::del_data(const uint& idx) {
  data.erase(data.begin() + idx);
  update();
}

void XNCTable::update() {
  // set_layout();
  map<uint, uint> who_wide, col_pos;
  for (uint x = 0; x < head.size(); ++x)
    who_wide[x] = head[x].length();

  // first, find widest col-members
  for (uint y = 0; y < data.size(); ++y)
    for (uint x = 0; x < data[y].size(); ++x)
      if (data[y][x].length() > who_wide[x])
        who_wide[x] = data[y][x].length();

  // set col positions
  col_pos[0] = 0;
  for (uint x = 1; x < who_wide.size(); ++x)
    col_pos[x] = col_pos[x - 1] + who_wide[x - 1] + 1;

  // write headers first
  for (uint i = 0; i < head.size(); ++i)
    mvwaddstr(_win, 0, col_pos[i], head[i].c_str());

  // now write that shit
  for (uint y = 0; y < data.size(); ++y)
    for (uint x = 0; x < data[y].size(); ++x)
      mvwaddstr(_win, y + 1, col_pos[x], data[y][x].c_str());

  wrefresh(_win);
}

XNCStatusBar::~XNCStatusBar() {}

XNCStatusBar::XNCStatusBar(const uint& cols, const tCoords& pos,
                           const tCoords& size)
    : XNCBaseWindow(pos, size), content_cols(cols) {}

void XNCStatusBar::update() {
  // only align the middle field(s)
  // right and left is aligned
  uint left_col = data[0].length() + 3;
  uint right_col = size.x - data.back().length() - 3;
  //uint middel_space = size.x - left_col - right_col;
  uint colwidth = size.x / (content_cols - 2);

  mvwaddstr(_win, 0, 2, data[0].c_str());
  mvwaddstr(_win, 0, right_col, data.back().c_str());

  uint i = 1;
  for (tStringIter s = data.begin(); (s + 1) < data.end(); ++s, ++i)
    mvwaddstr(_win, 0, i * colwidth, (*s).c_str());

  wrefresh(_win);
}

void XNCStatusBar::set_content(const uint& idx, const string& s) {
  if (data.size() <= idx)
    data[idx] = s;
  else {
    while (data.size() <= idx)
      data.push_back("");
    data.push_back(s);
  }
  update();
}

void XNCStatusBar::set_content(const tStringList& sl) {
  if (sl.size() != content_cols) {
    cerr << "Statusbar content length: " << content_cols << endl;
    cerr << "tStringList has length of: " << sl.size() << endl;
    throw ContentLengthNotMatching(sl.size());
  }
  data = sl;
  update();
}

XNCScrolling::XNCScrolling(const uint& lines, const tCoords& pos,
                           const tCoords& size)
    : XNCBaseWindow(pos, size), lines(lines) {}

void XNCScrolling::update() {
  clear();
  while (data.size() > lines)
    data.pop_back();

  for (uint i = 0; i < data.size(); ++i)
    mvwaddstr(_win, i, 0, data[i].c_str());
  wrefresh(_win);
}

void XNCScrolling::push_row(const std::string& s) {
  data.insert(data.begin(), s);
}

XNCScrolling::~XNCScrolling() {}

XNC::XNC() {
  // init
  initscr();
  // cursor invisible
  curs_set(0);
  // activate colors
  start_color();
  // do not show inputs in console
  noecho();
  refresh();
}

void XNC::add_window(const std::string& id, XNCBaseWindow* obj) {
  name2window[id] = obj;
}

XNC::~XNC() { endwin(); }

void XNC::update() {
  for (auto& n2w : name2window)
    n2w.second->update();
}

int main() {

  XNC x;

  XNCHeadline h("FAATS - Synthesis Status Monitor");
  XNCStatusBar s(3, make_coords(0, LINES - 2), make_coords(COLS, LINES));
  XNCTable t(4, make_coords(4, 4), make_coords(40, 10));
  XNCScrolling sc(8, make_coords(45, 5), make_coords(25, 15));

  s.set_content({"abc", "--", "15:44:01 16.02.2014"});

  x.add_window("head", &h);
  x.add_window("status", &s);
  x.add_window("tbl", &t);
  x.add_window("scr", &sc);

  t.set_headers({"Laeuft seit", "Typ", "Schaltung", "Kekse"});
  t.push_data({"12m 44s", "Maple", "bi0_0op0_4", "check"});
  t.push_data({"1h", "WiCkeD", "bi0_0op34_12", "check"});

  sc.push_row("Jo ich kann was!");
  sc.push_row("Jo ich kann was!2");
  sc.push_row("Jo ich kann was!3");

  // refresh();
  // make getch() non-blocking
  nodelay(stdscr, true);

  /* while(1) {
     char key = getch();

     if(key == KEY_RESIZE)
       x.refresh();
     else if(key == 'x')
       break;


     usleep(0.33e6);
   }*/
  while (getch() != 'q') {
    usleep(0.330e6);
    // refresh();
    x.update();
  }
  return 0;
}
