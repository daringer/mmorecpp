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

Executor::Executor(const string& cmd, bool use_path) : 
  pid(-1), result(-1), verbose(false), use_path(use_path), cargs(NULL), 
  BUF_SIZE(16384), buffer(NULL), init_done(false) {
//  org_flags_stdin(0), org_flags_stderr(0) {

    fdin[0] = -1;
    fdin[1] = -1;
    fdout[0] = -1;
    fdout[1] = -1;
    fderr[0] = -1;
    fderr[1] = -1;

    set_buffer_size(BUF_SIZE);

    tStringList raw = XString(cmd).split(" ");
    for(tStringIter i=raw.begin(); i!=raw.end(); ++i) 
      if(i == raw.begin())
        command = *i;
      else
        args.push_back(*i);

    if (!use_path && !FS::Path(command).exists())
      throw CommandNotFound(command);
}

void Executor::set_buffer_size(int buf_size) {
  BUF_SIZE = buf_size;
  
  if(buffer != NULL)
    delete [] buffer;

  buffer = new char[buf_size];
  bzero(buffer, sizeof(char)*buf_size);
}

void Executor::communicate() {
    if(verbose)
      cout << "[i] Starting control of child!" << endl;

    pipe(fdin);
    pipe(fdout);
    pipe(fderr);

    /*int oldin = dup(STDIN_FILENO);
    int oldout = dup(STDOUT_FILENO);
    int olderr = dup(STDERR_FILENO);*/
   
    //int org_flags;
    //org_flags = fcntl(fdout[0], F_GETFL);
    //fcntl(fdout[0], F_SETFL, org_flags | O_NONBLOCK);

    /*org_flags = fcntl(fdin[1], F_GETFL);
    fcntl(fdin[1], F_SETFL, org_flags | O_NONBLOCK);

    org_flags = fcntl(fderr[1], F_GETFL);
    fcntl(fderr[1], F_SETFL, org_flags | O_NONBLOCK);*/

/*    org_flags_stdin = fcntl(fdin[0], F_GETFL);
    if(fcntl(fdin[0], F_SETFL, org_flags_stdin | O_NONBLOCK) == -1)
	    perror("fnctl1");
    org_flags_stderr = fcntl(fderr[0], F_GETFL);
    if(fcntl(fderr[0], F_SETFL, org_flags_stderr | O_NONBLOCK) == -1)
	    perror("fnctl2");
*/
    pid = fork();
    if(pid < 0) {
	perror("Could not fork: ");
	exit(1);

    } else if(pid == 0) {
	/*close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);*/

	if(dup2(fdout[0], STDIN_FILENO) == -1)
	  perror("dup2-1");
	if(dup2(fdin[1], STDOUT_FILENO) == -1)
	  perror("dup2-1");
	if(dup2(fderr[1], STDERR_FILENO) == -1)
	  perror("dup2-1");
    
	if(close(fdout[0]) == -1)
	  perror("close-fork-1");
	if(close(fdout[1]) == -1)
	  perror("close-fork-2");
	if(close(fdin[0]) == -1)
	  perror("close-fork-3");
	if(close(fdin[1]) == -1)
	  perror("close-fork-4");
	if(close(fderr[0]) == -1)
	  perror("close-fork-5");
	if(close(fderr[1]) == -1)
	  perror("close-fork-6");

	int ret = execute();
        exit(ret);
    } else {
      /*dup2(STDOUT_FILENO, fdout[0]); 
      dup2(STDIN_FILENO, fdin[1]);
      dup2(STDERR_FILENO, fderr[1]);*/
//cout << "inside fork elese" << endl;
      if (close(fderr[1]) == -1)
	      perror("close(fderr[1])");
      if (close(fdin[1]) == -1)
	      perror("close(fderr[1])");
      if (close(fdout[0]) == -1)
	      perror("close(fdout[0])");
//cout << "ENDEND - inside fork elese" << endl;
      /*dup2(STDIN_FILENO, oldin);
      dup2(STDOUT_FILENO, oldout);
      dup2(STDERR_FILENO, olderr);*/
    }
  //sleep(1);
  init_done = true;
}

int Executor::run() {
  if(verbose)
    cout << "[i] Running child!" << endl;
  
  pid = fork();
  int out = -1;
  if(pid < 0) {
    perror("Could not fork");
  } else if(pid == 0) {
    execute();
    exit(1);
  } else 
    out = check_for_exit(true);
  
  init_done = false;
  return out;
}

string Executor::read_stderr() {
  if(verbose)
    cout << "[i] trying to read from child's stderr" << endl;

  return read_from_fd(fderr[0]);
}

string Executor::read_stdout() {
  if(verbose)
    cout << "[i] trying to read from child's stdout" << endl;

    return read_from_fd(fdin[0]);
}

string Executor::read_from_fd(int fd) {
  if(!init_done)
    throw CommunicationNotInited("Tunnel not initilized");

  string out = "";
  //int len = 0;
    
  //int org_flags = fcntl(fd, F_GETFL);
  //fcntl(fd, F_SETFL, org_flags | O_NONBLOCK);

//  cout << "PERROR BEFORE WE ENTER!!!" << endl;
//     perror("afterread: ");
    //errno = 0;
//    bzero(buffer, BUF_SIZE);
//    size_t len = 0;
//    cout << "NOW ENTERING WHILE" << endl;
//    cout << "fd:  "  << fd << " filedesc" << endl;
//    while((len = read(fd, buffer, BUF_SIZE-1)) > 0) {
    while(read(fd, buffer, BUF_SIZE-1) > 0) {
//	usleep(10000);
//	cerr << "foo das is doch schwachsinn?!" << endl;
//     perror("afterread: ");
//      cout << "len: " << len << endl;
      out += buffer;
      bzero(buffer, BUF_SIZE);
//      if(len < BUF_SIZE-1)
//        break;
    }
//    cout << out.length() << endl;
  //fcntl(fd, F_SETFL, org_flags);
  return out;
}

void Executor::write_stdin(const string& input, bool newline) {
  if(!init_done)
    throw CommunicationNotInited("Tunnel not initilized");

  write(fdout[1], input.c_str(), input.length());
  if(newline)
    write(fdout[1], "\n", 1);
}
 
int Executor::execute() {
  int out = 0;
  //cargs = new char * [args.size()+2];
  cargs = new char * [MAX_ARGS];
  bzero(cargs, sizeof(char*) * MAX_ARGS);
  //cargs[0] = new char [command.length()+1];
  cargs[0] = new char [CMD_LEN];
  bzero(cargs[0], sizeof(char) * CMD_LEN);
  strncpy(cargs[0], command.c_str(), CMD_LEN);
  for(uint i=0; i<args.size(); ++i) {
    //cargs[i+1] = new char [args[i].length()+1];
    cargs[i+1] = new char [CMD_LEN];
    bzero(cargs[i+1], sizeof(char) * CMD_LEN);
    strncpy(cargs[i+1], args[i].c_str(), CMD_LEN);
  }
  cargs[args.size()+1] = (char *) NULL;
  if(use_path)
    out = execvp(cargs[0], cargs);
  else
    out = execv(cargs[0], cargs);

  for(uint i=0; args.size()+1; ++i)
    delete [] cargs[i];
  delete [] cargs;

  return out;
}

int Executor::check_for_exit(bool blocking) {
    if (verbose)
      cout << "[i] Checking for child (pid: " << pid << ") to exit" << endl;

    int flags = 0;
    if(!blocking)
      flags |= WNOHANG;
    pid_t retpid = waitpid(pid, &result, flags);

    //sleep(1);

    /*cout << "RETURNED pid: " << retpid << endl;
    cout << "RETURNED RESULT: " << result << endl;
    cout << "strerror: " << strerror(result) << endl;
    cout << "wexitstatus: " << WEXITSTATUS(result) << endl;
    cout << "wexitstatus_str: " << strerror(WEXITSTATUS(result)) << endl;*/

    if(retpid == -1)
      perror("waitpid() failed: ");


    if(WIFEXITED(result) != 0 && verbose)
      cout << "[i] Child (pid: " << pid << ") returned: " << result << endl;
    else if(!blocking && verbose)
      cout << "[i] Child (pid: " << pid << ") still running..." << endl;
    else if(verbose)
      cout << "[E] Error during wait for: " << pid << endl;

    if(!blocking && WIFEXITED(result) == 0)
      return result;
    else
      return WEXITSTATUS(result);
}

Executor::~Executor() {
    delete [] buffer;

   /* if(fcntl(fdin[0], F_SETFL, org_flags_stdin) == -1)
	    perror("fnctl-DESTRUKTOR1");

    if(fcntl(fderr[0], F_SETFL, org_flags_stderr) == -1)
	    perror("fnctl-DESTRUKTOR2");*/


    if(fdin[0] != -1)
      close(fdin[0]);
    if(fdin[1] != -1)
      close(fdin[1]);
    if(fdout[0] != -1)
      close(fdout[0]);
    if(fdout[1] != -1)
      close(fdout[1]);
    if(fderr[0] != -1)
      close(fderr[0]);
    if(fderr[1] != -1)
      close(fderr[1]);

    /*if(close(fdin[0]) == -1)
      perror("DESTRUKTOR-1");
    if(close(fdin[1]) == -1)
      perror("DESTRUKTOR-2");
    if(close(fdout[1]) == -1)
      perror("DESTRUKTOR-3");
    if(close(fdout[0]) == -1)
      perror("DESTRUKTOR-4");
    if(close(fderr[0]) == -1)
      perror("DESTRUKTOR-5");
    if(close(fderr[1]) == -1)
      perror("DESTRUKTOR-6");*/
}
/*
#include <iostream>

int main() {
	//Executor e("/opt2/linux/Maple_10.0/bin/maple"); 
	//Executor e("bash");
	//Executor e("/opt2/linux/Cadence_6.13/IC610/tools/dfII/bin/ocean");
	{	
	  Executor e("cat /proc/cpuinfo");//"ls /tmp/MozillaMailnews");

	  e.communicate();
	  cout << "got: " << e.read_stdout().length() << endl;
	  int ret = e.check_for_exit(true);
	  cout << "RETURN: " << ret << endl;

	}
	{
	perror("before 2 MAIN");
        Executor e2("cat /tmp/foo1");
        //Executor e2("cat /proc/cpuinfo");
	e2.communicate();
	cout << "got: " << e2.read_stdout().length() << endl;

	//int ret = e2.check_for_exit(true);
	  int ret;
        while ((ret = e2.check_for_exit()) == -1) {
	  cout << "waiting for child to exit" << endl;
	  cout << "return: " << ret << endl;
	  usleep(100*1000);
	}
	  cout << "RETURN: " << ret << endl;
	}
	{
	perror("before 3 MAIN");
        Executor e2("cat /proc/cpuinfo");
	e2.communicate();
	cout << "got: " << e2.read_stdout().length() << endl;

	int ret = e2.check_for_exit(true);
	  cout << "RETURN: " << ret << endl;

	}
	{	
	  Executor e("cat /etc/pacman.d/mirrorlist");//"ls /tmp/MozillaMailnews");

	  e.communicate();
	  cout << "got: " << e.read_stdout().length() << endl;
	  int ret = e.check_for_exit(true);
	  cout << "RETURN: " << ret << endl;
	}
		{
	perror("before LAST MAIN");
        Executor e2("cat /proc/cpuinfo");
	e2.communicate();
	cout << "got: " << e2.read_stdout().length() << endl;

	int ret = e2.check_for_exit(true);
	  cout << "RETURN: " << ret << endl;
	}

	{	
	  Executor e("cat /etc/X11/xorg.conf");//"ls /tmp/MozillaMailnews");
	  
	  e.communicate();
	  cout << "got: " << e.read_stdout().length() << endl;
	  int ret = e.check_for_exit(true);
	  cout << "RETURN: " << ret << endl;
	}
{	
	  Executor e("bash");//"ls /tmp/MozillaMailnews");
	  
	  e.communicate();
	  e << "ls /tmp";
	  e << "exit";

	  cout << "got: " << e.read_stdout() << endl;
	  cout << "got: " << e.read_stderr() << endl;
	  int ret = e.check_for_exit(true);
	  cout << "RETURN: " << ret << endl;
	}
	return 0;
}*/
