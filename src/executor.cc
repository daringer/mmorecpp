#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h>

#include "general.h"
#include "xstring.h"
#include "fs.h"

#include "executor.h"

using namespace std;
using namespace TOOLS;

Executor::Executor(const string& cmd, bool use_path)
    : pid(-1),
      result(-1),
      verbose(false),
      use_path(use_path),
      cargs(NULL),
      BUF_SIZE(16384),
      buffer(NULL),
      init_done(false) {

  set_buffer_size(BUF_SIZE);

  tStringList raw = XString(cmd).split(" ");
  for (tStringIter i = raw.begin(); i != raw.end(); ++i)
    if (i == raw.begin())
      command = *i;
    else
      args.push_back(*i);

  if (!use_path && !FS::Path(command).exists()) throw CommandNotFound(command);
}

void Executor::set_buffer_size(int buf_size) {
  BUF_SIZE = buf_size;

  if (buffer != NULL) delete[] buffer;

  buffer = new char[buf_size];
  bzero(buffer, sizeof(char) * buf_size);
}

void Executor::communicate() {
  if (verbose) cout << "[i] Starting control of child!" << endl;

  pipe(fdin);
  pipe(fdout);
  pipe(fderr);

  pid = fork();
  if (pid < 0) {
    perror("Could not fork: ");
    exit(1);

  } else if (pid == 0) {

    if (dup2(fdout[0], STDIN_FILENO) == -1) perror("dup2-1");
    if (dup2(fdin[1], STDOUT_FILENO) == -1) perror("dup2-1");
    if (dup2(fderr[1], STDERR_FILENO) == -1) perror("dup2-1");

    if (close(fdout[0]) == -1) perror("close-fork-1");
    if (close(fdout[1]) == -1) perror("close-fork-2");
    if (close(fdin[0]) == -1) perror("close-fork-3");
    if (close(fdin[1]) == -1) perror("close-fork-4");
    if (close(fderr[0]) == -1) perror("close-fork-5");
    if (close(fderr[1]) == -1) perror("close-fork-6");

    int ret = execute();
    exit(ret);
  } else {
    if (close(fderr[1]) == -1) perror("close(fderr[1])");
    if (close(fdin[1]) == -1) perror("close(fderr[1])");
    if (close(fdout[0]) == -1) perror("close(fdout[0])");
  }
  init_done = true;
}

int Executor::run() {
  if (verbose) cout << "[i] Running child!" << endl;

  pid = fork();
  int out = -1;
  if (pid < 0) {
    perror("Could not fork");
  } else if (pid == 0) {
    execute();
    exit(1);
  } else
    out = check_for_exit(true);

  init_done = false;
  return out;
}

string Executor::read_stderr() {
  if (verbose) cout << "[i] trying to read from child's stderr" << endl;

  return read_from_fd(fderr[0]);
}

string Executor::read_stdout() {
  if (verbose) cout << "[i] trying to read from child's stdout" << endl;

  return read_from_fd(fdin[0]);
}

string Executor::read_from_fd(int fd) {
  if (!init_done) throw CommunicationNotInited("Tunnel not initilized");

  string out = "";
  while (read(fd, buffer, BUF_SIZE - 1) > 0) {
    out += buffer;
    bzero(buffer, BUF_SIZE);
  }
  return out;
}

void Executor::write_stdin(const string& input, bool newline) {
  if (!init_done) throw CommunicationNotInited("Tunnel not initilized");

  write(fdout[1], input.c_str(), input.length());
  if (newline) write(fdout[1], "\n", 1);
}

int Executor::execute() {
  int out = 0;
  cargs = new char* [MAX_ARGS];
  bzero(cargs, sizeof(char*) * MAX_ARGS);

  cargs[0] = new char[CMD_LEN];
  bzero(cargs[0], sizeof(char) * CMD_LEN);
  strncpy(cargs[0], command.c_str(), CMD_LEN);

  for (uint i = 0; i < args.size(); ++i) {
    cargs[i + 1] = new char[CMD_LEN];
    bzero(cargs[i + 1], sizeof(char) * CMD_LEN);
    strncpy(cargs[i + 1], args[i].c_str(), CMD_LEN);
  }

  cargs[args.size() + 1] = (char*)NULL;

  if (use_path)
    out = execvp(cargs[0], cargs);
  else
    out = execv(cargs[0], cargs);

  for (uint i = 0; args.size() + 1; ++i) delete[] cargs[i];
  delete[] cargs;

  return out;
}

int Executor::check_for_exit(bool blocking) {
  if (verbose)
    cout << "[i] Checking for child (pid: " << pid << ") to exit" << endl;

  int flags = 0;
  if (!blocking) flags |= WNOHANG;
  pid_t retpid = waitpid(pid, &result, flags);

  if (retpid == -1) perror("waitpid() failed: ");

  if (WIFEXITED(result) != 0 && verbose)
    cout << "[i] Child (pid: " << pid << ") returned: " << result << endl;
  else if (!blocking && verbose)
    cout << "[i] Child (pid: " << pid << ") still running..." << endl;
  else if (verbose)
    cout << "[E] Error during wait for: " << pid << endl;

  if (!blocking && WIFEXITED(result) == 0)
    return result;
  else
    return WEXITSTATUS(result);
}

Executor::~Executor() { delete[] buffer; }
