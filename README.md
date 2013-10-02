mmorecpp - a high-lvl, flexible, lightweight C++ utils library
===========================================================================

This library should provide helpful tools for C++. Aiming for the following
goals:

* useful, comfortable C++ style (boost like) utils
* include into project and reduce external dependencies
* modern OO-driven C++ interface
* simple, fast, small footprint
* can be used in parts
* c++11 required (gcc 4.8+ recommended)
* extremly easy and smart interfaces

Currently the following functionalities exist:

*config_manager*
* Configuration manager with declaritive interface
* Config-file parser
* Commandline parser

*exception*
* BaseException - advanced exception
* print_stacktrace() - shows stacktrace
* DEFINE_EXCEPTION macro
* alternative terminate handler
* alternative signal handler

*executor*
* Execute commands and control stdout/-in/-err

*fs*
* Filesystem tools mainly inside the class Path

*io*
* io helper

*mem_tracker*
* trace memory leaks from inside your code
* start/stop memory journaling anywhere inside your code!
* fast/transparent/easy

*template_parser*
* simple string template engine (jinja2/django like)

*testing_suite*
* very might testing suite with extremly small overhead

*threading*
* thread manager/control/abstraction

*xlogger*
* highly comfortable, easy logger 

*xregex*
* regex abstraction 

*xsocket* 
* socket abstraction including multiselect 

*xstring*
* stringhelper methods
* .startswith() .endswith() .subs()

*xtime*
* date/time formating info/output
* measure time
