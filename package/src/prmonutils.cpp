// Copyright (C) 2018-2020 CERN
// License Apache2 - see LICENCE file

#include "prmonutils.h"

#include <stdlib.h>
#include <sys/stat.h>

#include <cstddef>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>

namespace prmon {

bool kernel_proc_pid_test(const pid_t pid) {
  // Return true if the kernel has child PIDs
  // accessible via /proc
  std::stringstream pid_fname{};
  pid_fname << "/proc/" << pid << "/task/" << pid << "/children" << std::ends;
  struct stat stat_test;
  if (stat(pid_fname.str().c_str(), &stat_test)) return false;
  return true;
}

std::vector<pid_t> pstree_pids(const pid_t mother_pid) {
  // This is the old style method to get the list
  // of PIDs, which uses pstree
  //
  // At least this was what was needed on SLC6 (kernel 2.6)
  // so at some point all useful machines will support the new
  // method
  std::vector<pid_t> cpids;
  char smaps_buffer[64];
  snprintf(smaps_buffer, 64, "pstree -A -p %ld | tr \\- \\\\n",
           (long)mother_pid);
  FILE* pipe = popen(smaps_buffer, "r");
  if (pipe == 0) return cpids;

  char buffer[256];
  std::string result = "";
  while (!feof(pipe)) {
    if (fgets(buffer, 256, pipe) != NULL) {
      result += buffer;
      int pos(0);
      while (pos < 256 && buffer[pos] != '\n' && buffer[pos] != '(') {
        pos++;
      }
      if (pos < 256 && buffer[pos] == '(' && pos > 1 &&
          buffer[pos - 1] != '}') {
        pos++;
        pid_t pt(0);
        while (pos < 256 && buffer[pos] != '\n' && buffer[pos] != ')') {
          pt = 10 * pt + buffer[pos] - '0';
          pos++;
        }
        cpids.push_back(pt);
      }
    }
  }
  pclose(pipe);
  return cpids;
}

std::vector<pid_t> offspring_pids(const pid_t mother_pid) {
  // Get child process IDs in the new way, using /proc
  std::vector<pid_t> pid_list{};
  std::deque<pid_t> unprocessed_pids{};

  // Start with the mother PID
  unprocessed_pids.push_back(mother_pid);

  // Now loop over all unprocessed PIDs, querying children
  // and pushing them onto the unprocessed queue, while
  // poping the front onto the final PID list
  while (unprocessed_pids.size() > 0) {
    std::stringstream child_pid_fname{};
    pid_t next_pid;
    child_pid_fname << "/proc/" << unprocessed_pids[0] << "/task/"
                    << unprocessed_pids[0] << "/children" << std::ends;
    std::ifstream proc_children{child_pid_fname.str()};
    while (proc_children) {
      proc_children >> next_pid;
      if (proc_children) unprocessed_pids.push_back(next_pid);
    }
    pid_list.push_back(unprocessed_pids[0]);
    unprocessed_pids.pop_front();
  }
  return pid_list;
}

void SignalCallbackHandler(int /*signal*/) { sigusr1 = true; }

int reap_children() {
  int status;
  int return_code = 0;
  pid_t pid{1};
  while (pid > 0) {
    pid = waitpid((pid_t)-1, &status, WNOHANG);
    if (status && pid > 0) {
      if (WIFEXITED(status))
        return_code = WEXITSTATUS(status);
        std::clog << "Child process " << pid
                  << " had non-zero return value: " << return_code
                  << std::endl;
      else if (WIFSIGNALED(status))
        std::clog << "Child process " << pid << " exited from signal "
                  << WTERMSIG(status) << std::endl;
      else if (WIFSTOPPED(status))
        std::clog << "Child process " << pid << " was stopped by signal"
                  << WSTOPSIG(status) << std::endl;
      else if (WIFCONTINUED(status))
        std::clog << "Child process " << pid << " was continued" << std::endl;
    }
  }
  return return_code;
}

}  // namespace prmon
