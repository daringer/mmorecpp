#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <vector>

#include "../core/exception.h"

namespace MM_NAMESPACE() {

  DEFINE_EXCEPTION(CommandNotFound, BaseException)
  DEFINE_EXCEPTION(CommunicationNotInited, BaseException)

#define CMD_LEN 2048
#define MAX_ARGS 256

  class Executor {
   public:
    int pid;
    int result;
    bool verbose;

    std::string command;
    std::vector<std::string> args;

    Executor(const std::string& cmd, bool use_path = true);
    ~Executor();

    int run();
    void communicate();

    int check_for_exit(bool blocking = false);
    void set_buffer_size(int buf_size);

    std::string read_stdout();
    std::string read_stderr();
    void write_stdin(const std::string& input, bool newline = true);

   private:
    bool use_path;
    char** cargs;

    int fdin[2], fdout[2], fderr[2];

    int BUF_SIZE;
    char* buffer;
    bool init_done;

    int execute();
    std::string read_from_fd(int fd);
  };
}
