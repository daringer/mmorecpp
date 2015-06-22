#pragma once 


// An include-include file to ease 'mmorecpp's inclusion:

// core is always included, but may be excluded
//#ifndef NO_CORE

#include "core/general.h"
#include "core/exception.h"
#include "core/converter.h"
#include "core/xstring.h"
#include "core/io.h"
#include "core/xtime.h"
#include "core/xunit.h"

//#endif

//#ifdef OOAPI

#include "ooapi/config_manager.h"
#include "ooapi/template_parser.h"
#include "ooapi/testing_suite.h"
#include "ooapi/xregex.h"
#include "ooapi/xsqlite.h"

//#endif 

//#ifdef PLATFORM

#include "platform/executor.h"
#include "platform/fs.h"
#include "platform/threading.h"
#include "platform/xlogger.h"
#include "platform/xsocket.h"

//#endif 

//#ifdef experimental
//#include "experimental/xncurses.h"
//#endif
