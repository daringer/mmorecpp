mmorecpp - a high-lvl, flexible, lightweight C++ utils library
===========================================================================

**This library provides helpful tools for your everyday C++ challenges. 
Aiming for the following goals:**

* useful, comfortable C++ style (boost like) utils
* include into project and reduce external dependencies
* modern OO-driven C++ interface
* simple, fast, small footprint
* can be used in parts
* c++11 required (clang++ (>=3.4) [recommended], g++ (>=4.8))
  * support for older compilers in progress
  * support for a higher platform independance in progress
* selectivly disable features during compile-time 
* extremly easy and smart interfaces
* slim OO-interfaces for tedious plain C interfaces (e.g., sqlite3, popen,
  date/time, threading/forking, locking, sockets, ncurses)
* .... or maybe you're just lookin' for some "inspiration" for you current
  project, come in and (may)be get inspired ;)

**Where is the documentation?**

* currently the code-basis serves as documentation
* ...check `tests/*.cpp` and `.h` files for alot info/examples

**What are the modules/features of this so-called 'mmorecpp' library?**

* `ConfigManager` Setup/Parse/Write/Manage configuration options (including
  cmdline-args parsing) 
* `TemplateParser` Fast/Light/Simple string template renderer (Jinja2 alike -
  but far less features)
* `XSQLite` aims to simplify sql-db acces in C++ without the usual dependency
  load (still in early devel only insert/select atm)
* `Executor` Execute command and (if needed) take full control of strout/err/in
* `FS::Path` Filesystem helper class to search/manipulate paths/files/dirs
* `XTime`, `XDateTime`, `XUnit` Charmingly easy OO-interfaces for time-measuring and
  date/time handling
* `XString` provides high-lvl string manipulation (split, strip, join,
  substitute, lower, upper, startswith, endswith) and derives from std::string
  to maintain a maximum of compatibility
* `UNIT_TEST::*` provides a fixture-based (unit-)testing framework aiming for
  minimum of coding overhead to maintain and write tests
* `BaseProcess` `BaseThread` provide threading/forking functionality through a
  consistent interface `BaseParallel`. To realize a parallel process/thread
  simply derive from either `BaseProcess` or `BaseThread` and overwrite
  `::worker()`.
* `Mutex`, `MessageQueue` and `MessageQueueServer` provide easy to use IPC
  mechanisms
* `MutexVariable` is an insanely easy thread-safe-variable(-template-class) of
  any type, just `MutexVariable<int> safe_var;` and be thread-safe by using
  `::set()` and `::get()` to access the variable!
* `SocketStream` `StreamSelecter` provide easy, stream-based, asynchronous
  (tcp-)socket interfaces
* `Xncurses` is a widget-like toolkit for ncurses (in early dev stage)
* `XRegex` wraps the C regex implementation in a OO-way, but c++11 provides a
  similar module, thus it is marked as (obsolete)
* `integer` `real` `str` are template-driven, highly versatile conversion
  functions, doing it the (secure) C++ way, while still providing a decent
  performance (renamming of these incoming)

**WoopWoop, sounds awesome. Any chance to help me with debugging/logging/measuring my code?**

* `XLogger` solves all your logging needs at once! Stream-based logging without
  parenthesis, multiple loglevels, multiple parallel *logging backends*,
  logging-hooks (actions), built to be as unintrusive (and fancy) as possible,
  extensive meta-data loggable: line-no, function/method-name, ...
* `DEFINE_EXCEPTION` reduces the time needed to introduce a new exception to
  seconds
* `BaseException` a multi-purpose to-be-derived-from exception, which unleashes
  its full potential paired with the macro, i.e.,
  `DEFINE_EXCEPTION(MyOhGodSomethingHappendError, TOOLS::BaseException);` 
* `print_stacktrace()` outputs the current stacktrace providing useful insight
  where needed
* Combining some of the previous with `TOOLS::tools_lib_exception_handler()` as
  the default terminate handler using `set_terminate()` enables you to see the
  stacktrace once a unhandled exception was catched - in other words, if your
  software crashes you will get stacktrace information directly!
