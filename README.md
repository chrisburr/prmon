# Process Monitor (prmon)

[![Build Status][build-img]][build-link]  [![License][license-img]][license-url] [![Codefactor][codefactor-img]][codefactor-url]

[build-img]: https://github.com/HSF/prmon/workflows/CI/badge.svg?branch=master
[build-link]: https://github.com/HSF/prmon/actions?query=workflow%3ACI+branch%3Amaster
[license-img]: https://img.shields.io/github/license/hsf/prmon.svg
[license-url]: https://github.com/hsf/prmon/blob/master/LICENSE
[codefactor-img]: https://www.codefactor.io/repository/github/HSF/prmon/badge
[codefactor-url]: https://www.codefactor.io/repository/github/HSF/prmon

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2554202.svg)](https://doi.org/10.5281/zenodo.2554202)

The PRocess MONitor is a small stand alone program that can monitor
the resource consumption of a process and its children. This is
useful in the context of the WLCG/HSF working group to evaluate
the costs and performance of HEP workflows in WLCG. In a previous
incarnation (MemoryMonitor) it has been used by ATLAS for sometime to
gather data on resource consumption by production jobs. One of its
most useful features is to use smaps to correctly calculate the
*Proportional Set Size* in the group of processes monitored, which
is a much better indication of the true memory consumption of
a group of processes where children share many pages.

prmon currently runs on Linux machines as it requires access to the
`/proc` interface to process statistics.

## Build and Deployment

### Building the project

Building prmon requires a C++ compiler that fully supports C++11,
CMake version 3.3 or
higher and the [Niels Lohmann JSON libraries](https://github.com/nlohmann/json).

Building is usually as simple as

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=<installdir> <path to sources>
    make -j<number of cores on your machine>
    make install

If your installation of JSON is in a non-standard location then
setting `nlohmann_json_DIR` may be required as a hint to CMake.
(e.g. this is necessary on Ubuntu18 when using the standard `nlohmann-json-dev`
package: `-Dnlohmann_json_DIR=/usr/lib/cmake`.)

The option `-DCMAKE_BUILD_TYPE` can switch between all of the standard
build types. The default is `Release`; use `RelWithDebInfo` if you want
debug symbols.

To build a statically linked version of `prmon` set the `BUILD_STATIC`
CMake variable to `ON` (e.g., adding `-DBUILD_STATIC=ON` to the
command line).

Note that in a build environment with CVMFS available the C++ compiler
and CMake can be taken by setting up a recent LCG release.

The tests in prmon now run in Python3. If your environment does not
support Python3 properly it is possible to use Python2 for the tests
by setting `-DPYTHON_TEST=python2`.

### Creating a package with CPack

A cpack based package can be created by invoking

    make package

### Running the tests

To run the tests of the project, first build it and then invoke

    make test

As noted above these will use `python3` by default, but this can
be changed via the `PYTHON_TEST` option to CMake.

## Running

The `prmon` binary is invoked with the following arguments:

```sh
prmon [--pid PPP] [--filename prmon.txt] [--json-summary prmon.json] \
      [--interval 30] [--suppress-hw-info] [--units] [--netdev DEV] \
      [-- prog arg arg ...]
```

* `--pid` the 'mother' PID to monitor (all children in the same process tree are monitored as well)
* `--filename` output file for timestamped monitored values
* `--json-summmary` output file for summary data written in JSON format
* `--interval` time, in seconds, between monitoring snapshots
* `--suppress-hw-info` flag that turns-off hardware information collection
* `--units` add information on units for each metric to JSON file
* `--netdev` restricts network statistics to one (or more) network devices
* `--` after this argument the following arguments are treated as a program to invoke
  and remaining arguments are passed to it; `prmon` will then monitor this process
  instead of being given a PID via `--pid`


## Outputs

In the `filename` output file, plain text with statistics written every
`interval` seconds are written. The first line gives the column names.

In the `json-summmary` file values for the maximum and average statistics
are given in JSON format. This file is rewritten every `interval` seconds
with the current summary values. Use the `--units` option to see exactly
which units are used for each metric (the value of `1` for a unit means
it is a pure number).

Monitoring of CPU, I/O and memory is reliably accurate, at least to within
the sampling time. Monitoring of network I/O is **not reliable** unless the
monitored process is isolated from other processes performing network I/O
(it gives an upper bound on the network activity, but the monitoring is
per network device as Linux does not give per-process network data by
default).

### Visualisation

The `prmon_plot.py` script (Python3) can be used to plot the outputs of prmon from the
timestamped output file (usually `prmon.txt`). Some examples include:

* Memory usage as a function of wall-time:
```sh
prmon_plot.py --input prmon.txt --xvar wtime --yvar vmem,pss,rss,swap --yunit GB
```
![](example-plots/PrMon_wtime_vs_vmem_pss_rss_swap.png)

* Rate of change in memory usage as a function of wall-time:
```sh
prmon_plot.py --input prmon.txt --xvar wtime --yvar vmem,pss,rss,swap --diff --yunit MB
```
![](example-plots/PrMon_wtime_vs_diff_vmem_pss_rss_swap.png)

* Rate of change in CPU usage as a function of wall-time with stacked
user and system utilizations:
```sh
prmon_plot.py --input prmon.txt --xvar wtime --yvar utime,stime --yunit SEC --diff --stacked
```
![](example-plots/PrMon_wtime_vs_diff_utime_stime.png)

The plots above, as well as the input `prmon.txt` file that is used
to produce them, can be found under the `example-plots` folder.

The script allows the user to specify variables, their units, plotting
style (stacked vs overlaid), as well as the format of the output image.
Use `-h` for more information.

## Feedback and Contributions

We're very happy to get feedback on prmon as well as suggestions for future
development. Please have a look at our [Contribution
Guide](doc/CONTRIBUTING.md).


### Profiling

To build prmon with profiling set one of the CMake variables
`PROFILE_GPROF` or `PROFILE_GPERFTOOLS` to `ON`. This enables
GNU prof profiling or gperftools profiling, respectively.
If your gperftools are in a non-standard place, pass a hint
to CMake using `Gperftools_ROOT_DIR`.


# Copyright

Copyright (c) 2018-2020 CERN.
