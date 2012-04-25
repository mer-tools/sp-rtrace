Name: sp-rtrace
Version: 1.9
Release: 1%{?dist}
Summary:  Resource consumption tracing tools
Group: Development/Tools
License: GPLv2+
URL: http://www.gitorious.org/+maemo-tools-developers/maemo-tools/sp-rtrace
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build
BuildRequires: autoconf, automake, libtool, doxygen, gcc-c++
BuildRequires: binutils-devel, glib2-devel

%description
 This package provides tools for tracing allocation and deallocation of
 various resources (memory, memory transfers etc), and post-processing
 of the resulting traces.

%prep
%setup -q -n sp-rtrace

%build
autoreconf -fvi

%configure

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
rm %{buildroot}/usr/lib/*a
rm %{buildroot}/usr/lib/%{name}/*a


%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/sp-rtrace-postproc
%{_bindir}/sp-rtrace-pagemap
%{_bindir}/sp-rtrace
%{_bindir}/sp-rtrace-resolve
%{_bindir}/rtrace-rename
%{_bindir}/rtrace-sort
%{_bindir}/rtrace-stats
%{_bindir}/rtrace-function-address
%{_libdir}/libsp-rtrace-main.so*
%{_libdir}/sp-rtrace/
%{_mandir}/man1/sp-rtrace.1.gz
%{_mandir}/man1/sp-rtrace-postproc.1.gz
%{_mandir}/man1/sp-rtrace-pagemap.1.gz
%{_mandir}/man1/sp-rtrace-resolve.1.gz
%{_mandir}/man1/rtrace-rename.1.gz
%{_mandir}/man1/rtrace-sort.1.gz
%{_mandir}/man1/rtrace-stats.1.gz
%{_mandir}/man1/rtrace-function-address.1.gz
%doc COPYING.* README

#
# Shared libary (formatter/context support)
#
%package -n lib%{name}
Summary: sp-rtrace formatter/context support library
Group: Development/Libraries

%description -n lib%{name}
 This library allows third party applications to use function allocation
 contexts and API to generate resource consumption tracing (sp-rtrace) text
 format compatible reports.

%files -n lib%{name}
%defattr(-,root,root,-)
%{_libdir}/libsp-rtrace1.so.*

%post -n lib%{name}
/sbin/ldconfig

%postun -n lib%{name}
/sbin/ldconfig

#
# Development package 
#
%package -n lib%{name}-devel
Summary: Development files for using resource consumption tracing support library
Group: Development/Tools
Requires: lib%{name} = %{version}

%description -n lib%{name}-devel
 This package contains headers, manuals and symbolic links to use the resource
 consuption tracing support library (libsp-rtrace1).

%files -n lib%{name}-devel
%defattr(-,root,root,-)
%{_libdir}/libsp-rtrace1.so
%{_includedir}/*.h
%{_mandir}/man3/sp_rtrace_context*.3.gz
%{_mandir}/man3/sp_rtrace_formatter*.3.gz
%{_mandir}/man3/sp_rtrace_print*.3.gz
%{_mandir}/man3/sp_rtrace_filter*.3.gz
%{_mandir}/man3/sp_rtrace_tracker*.3.gz
%{_mandir}/man1/rtrace-module-gen.1.gz
%{_bindir}/rtrace-module-gen


#
# Postprocessing visualization tools
#
%package visualize
Summary: Postprocessing tools for the sp-rtrace report visualization
Group: Development/Tools
Requires: sp-rtrace, python, graphviz
#There's no miminal gnuplot-nox like in Debian
#Requires: gnuplot

%description visualize
 This package contains various visualization (alloc addressmap,
 histogram, timeline, callgraph) utilities for rtrace text format
 reports.

%files visualize
%defattr(-,root,root,-)
%{_bindir}/rtrace-allocmap
%{_bindir}/rtrace-allocmap-animate
%{_bindir}/rtrace-calltree
%{_bindir}/rtrace-from-function
%{_bindir}/rtrace-graphs-function
%{_bindir}/rtrace-graphs-overview
%{_bindir}/rtrace-alloc-sizes
%{_bindir}/sp-rtrace-allocmap
%{_bindir}/sp-rtrace-timeline
%{_mandir}/man1/rtrace-allocmap.1.gz
%{_mandir}/man1/rtrace-allocmap-animate.1.gz
%{_mandir}/man1/rtrace-calltree.1.gz
%{_mandir}/man1/rtrace-from-function.1.gz
%{_mandir}/man1/rtrace-graphs-function.1.gz
%{_mandir}/man1/rtrace-graphs-overview.1.gz
%{_mandir}/man1/rtrace-alloc-sizes.1.gz
%{_mandir}/man1/sp-rtrace-allocmap.1.gz
%{_mandir}/man1/sp-rtrace-timeline.1.gz


%changelog
* Mon Apr 25 2012 Eero Tamminen <eero.tamminen@nokia.com> 1.9
  * Update to 1.9 with many fixes, add new files

* Mon Dec 19 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.8.6
  * fix rpmlint error, add minimal visualize package deps
  * Update to 1.8.6

* Tue Oct 25 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.8.5
  * Force elf resolving mode for sp-rtrace-postproc "-r" option
  * In rtrace-graphs-* helper scripts do callgraph symbol resolving
    using debug symbols instead of ELF symbol table  (debug symbols are
    much slower & memory hungry, but can give much better results)
  * Fix, convert callgraph to SVG (not PS) in rtrace-graphs-function

* Thu Oct 13 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.8.4
  * Fix corrupted log file name when `-r' option is used.

* Mon Oct 10 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.8.3
  * Fix `sp-rtrace-pagemap --help-pages' crash.

* Fri Sep 23 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.8.2
  * Fix use of incorrect options in inline help examples.

* Tue Aug 23 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.8.1
  * Add --quiet option suppressing warning messages to sp-rtrace,
    sp-rtrace-postproc, sp-rtrace-resolve.
  * Add symbolic signal name support for -S option.

* Wed Aug 03 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.8
  * The option `-p' has been changed to `-e' to match functracer option names.
  * Audit module has been removed (`-a' option). sp-rtrace cannot track (even
    exported) function calls well enough due to limitations of the linker
    LD_AUDIT mechanism. Functracer can be used for the same purpose.

* Thu Jun 30 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.7
  * Swap size and type fields in binary protocol to fix crashes.

* Thu Jun 16 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.6.1
  * Fix bfd symbol resolving issues: If sp-rtrace backtraces go through
    certain kind of method/function calls where the call argument is a
    temporary object that gets constructed & destructed during argument
    handling, sp-rtrace-resolve resolves to destructor of that temporary
    object instead of the function/method call on which line it was used.
  * Calculate toggle signal during options parsing: the toggle signal was
    calculated only when sp-trace was used in toggle mode. This caused
    interrupting sp-rtrace with ctrl+c to send the default toggle signal even
    if custom signal was specified.
  * Write pagemap files into start directory if not specified otherwise.

* Wed Jun 01 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.6
  * Improve SIGINT handling: tracing can now be stopped simply by aborting
    sp-rtrace with ctrl+c. If the tracing was enabled from the start (-s
    option), then the target process will be stopped after the data has been
    transferred and pipe closed. Otherwise sp-rtrace collects the data, but
    does not stop the target process.
  * New sp-rtrace option --monitor: Report backtraces only for allocations
    having the specified resource size(s).

* Tue May 24 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.5.2
  * sp-rtrace-timeline --filter-time now allows relative time values.
  * Fix incorrect close() in trace toggling path.

* Mon May 09 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.5.1
  * Do not send toggle signal to a process that is not traced.

* Thu Apr 28 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.5
  * Pagemap post-processing utility for memory fragmentation analysis
  * New --include/--exclude options to sp-rtrace-postproc (for pagemap)
  * Improvements to rtrace-calltree:
    - new "allocation count" callgraph type
    - new options for reducing node (text) sizes, enabled by default
  * sp-rtrace-timeline charts X-axis shows both alloc count & time
  * Speedup for memory allocation tracking for programs doing
    simultenous allocations from many threads

* Fri Mar 04 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.4.1
  * Fix BFD resolving segfault (data freed too early)
  * Fix sp-rtrace-postproc crash with traces containing multiple
    resource types
  * Fix/update rtrace-calltree --trace-args option and rename it
    to --show-args.  Add --include-only-args option to do the same
    thing for callgraph arg nodes as --include-only does for
    function nodes
  * Fix shmposix trace function names (mmap2 & mmap64 instead of mmap)
  * Test suite has now tests for all modules

* Wed Feb 16 2011 Eero Tamminen <eero.tamminen@nokia.com> 1.4
  * Debug package, compiler option, x86 reliability and memory leak fixes
  * "origin" field added to text format header. 
  * HTML and histogram reports removed from rtrace-calltree. Former isn't
    really useful and latter functionality is now in sp-rtrace-timeline
  * Major rtrace-calltree speed improvement for large traces
  * Post-processing tools updated to accept audit traces where
    traced functions have C++ signatures
  * sp-rtrace-timeline --scalex option value is now in percentages
    and resource life-time timeline Y-axis is logarithmic
  * New module and post-processing tools to track which of the process.
    memory pages are dirty and whether they're zeroed.  Added trace file
    format support for references to additional data files
  * New module for tracing POSIX shared memory mappings
  * SYSV shared memory module reports "shmctl(...,IPC_RMID,...)" events
  * Documentation updates and MeeGo compatibility fixes

* Fri Dec 10 2010 Eero Tamminen <eero.tamminen@nokia.com> 1.3
  * C++ rewrite of rtrace-timeline to make it faster
  * Improved support for resource reference counting
  * Binary and ASCII trace file format changes for above
  * Added module generator script, module code template & examples
  * New modules for tracing QObject and Gobject create/reference/free
  * Renamed shmseg module to shmsysv (as it's in functracer)
  * Experimental user specified function tracing support and module
    based on dynamic linker LD_AUDIT framework
  * Put allocmap temp files to current dir, use faster options
    and better error handling in rtrace-allocmap
  * Support new Linux pipe2() function in sp-rtrace file module

* Thu Nov 11 2010 Eero Tamminen <eero.tamminen@nokia.com> 1.2
  * rtrace-timeline script rewrite:
    - total non-freed allocations timeline chart
    - allocation activity timeline chart
    - allocation life-time timeline chart
    - allocation count & size per alloc size histograms
    - possible to generate all charts at the same time
    - huge memory usage reduction
    - options for filtering allocations & deallocations based on
      allocation record, timestamp or size range and scaling chart
  * rtrace-graphs-overview script rewrite:
    - resource file includes information about filtering done on it
    - handle both binary and ascii files
    - input files need now to be specified
    - generate also (all) timeline charts
    - show the generated charts with xdg-open
  * rtrace-callgree updates:
    - ignore deallocation backtraces
    - option to filter allocations based on size range
    - updates to glib/qt function/method ignore shortcuts
  * Experimental support for using libunwind to collect backtraces
  * Binary and ASCII trace file format changes for above
  * Context API library changes needed by functracer
  * Post-processing speed and memory usage improvements
  * Force i486 architecture for x86 (Sbox). 
    (GCC version specific atomic ops issue)
  * Automake&libtool added to build-deps. 

* Thu Sep 23 2010 Eero Tamminen <eero.tamminen@nokia.com> 1.1
  * Function name resolving verifies libbfd provide information from
    the ELF symbol table to get rid of bogus information provided
    by BFD for inline/template methods.  This may also remove useful
    information, for that case one can ask resolving to use just BFD.
  * Fixes to tracing toggling and backtrace generation
  * Modules can trace activity done before process enters main()
  * New file module for tracing file descriptor increase & decrease
  * Backtraces for freed resources are optional (improves performance)
  * Backtrace is output in similar format to one used by Gdb
  * rtrace-calltree:
    - HTML report generation & pylint errors fixed
    - callgraphs handle resource freeing backtraces properly
    - more & updated --ignore-* convenience functions
    - option for seeing first function arguments (e.g. filename) as nodes
  * Preliminary unfreed allocs & allocation activity timeline charts

* Tue Jun 29 2010 Eero Tamminen <eero.tamminen@nokia.com> 1.0
  * Initial Release.
