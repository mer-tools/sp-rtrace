Name: sp-rtrace
Version: 2.0.1
Release: 1
Summary:  Resource consumption tracing tools
Group: Development/Tools
License: GPLv2+
URL: http://www.gitorious.org/+maemo-tools-developers/maemo-tools/sp-rtrace
Source: %{name}-%{version}.tar.gz
BuildRequires: autoconf
BuildRequires: automake
BuildRequires: libtool
BuildRequires: doxygen
BuildRequires: gcc-c++
BuildRequires: binutils-devel
BuildRequires: glib2-devel

%description
 This package provides tools for tracing allocation and deallocation of
 various resources (memory, memory transfers etc), and post-processing
 of the resulting traces.

%prep
# Adjusting %%setup since git-pkg unpacks to src/
# %%setup -q
%setup -q -n src

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
%{_bindir}/rtrace-start
%{_libdir}/libsp-rtrace-main.so*
%{_libdir}/sp-rtrace/
%{_mandir}/man1/sp-rtrace.1.gz
%{_mandir}/man1/sp-rtrace-postproc.1.gz
%{_mandir}/man1/sp-rtrace-pagemap.1.gz
%{_mandir}/man1/sp-rtrace-resolve.1.gz
%{_mandir}/man1/rtrace-rename.1.gz
%{_mandir}/man1/rtrace-sort.1.gz
%{_mandir}/man1/rtrace-stats.1.gz
%{_mandir}/man1/rtrace-start.1.gz
%{_mandir}/man1/rtrace-function-address.1.gz
%doc COPYING.* README

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

#
# Shared libary (formatter/context support)
#
%package -n lib%{name}
Summary: Sp-rtrace formatter/context support library
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

%post -n lib%{name}-devel
/sbin/ldconfig

%postun -n lib%{name}-devel
/sbin/ldconfig

#
# Postprocessing visualization tools
#
%package visualize
Summary: Postprocessing tools for the sp-rtrace report visualization
Group: Development/Tools
Requires: sp-rtrace
Requires: python

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
