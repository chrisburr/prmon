// Copyright (C) 2018-2020 CERN
// License Apache2 - see LICENCE file

// A few utilities handling PID operations, signals
// and child process functions, used by the main prmon binary

#ifndef PRMON_UTIL_H
#define PRMON_UTIL_H 1

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <vector>

namespace prmon {

// PID related utilities
bool kernel_proc_pid_test(const pid_t pid);
std::vector<pid_t> pstree_pids(const pid_t mother_pid);
std::vector<pid_t> offspring_pids(const pid_t mother_pid);

// Signal handlers
extern bool sigusr1;
void SignalCallbackHandler(int);

// Child process reaper
int reap_children();

// Precision specifier for average output, to truncate to an integer
// for anything >avg_precision and round the fraction to
// essentially 1/avg_precision (thus 1000 = 3 decimal places) for
// anything smaller
const long avg_precision = 1000;

}  // namespace prmon

#endif  // PRMON_UTIL_H
