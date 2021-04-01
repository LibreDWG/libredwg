# -*- sh -*-
Name:           libredwg
Version:        0.12.4
Release:        1%{?dist}
Summary:        GNU C library and programs to read and write DWG files

License:        GPLv3+
URL:            https://www.gnu.org/software/libredwg/
#Source0:        https://ftp.gnu.org/gnu/libredwg/libredwg-%%{version}.tar.xz
Source0:        https://github.com/LibreDWG/libredwg/releases/download/%{version}/libredwg-%{version}.tar.xz

BuildRequires:  gcc, texinfo-tex, texinfo, pcre2-devel, swig, python3-devel
BuildRequires:  python3-libxml2, perl-devel, perl-Convert-Binary-C, pslib-devel
Requires:       pcre2, pcre2-utf16
# no big-endian. s390 untested
ExcludeArch:    sparc alpha ppc64 ppc s390

%description
LibreDWG is a free C library to read and write DWG files. At the moment
our decoder (i.e. reader) is done, just some very advanced R2010+ and
preR13 entities fail to read and are skipped over. The writer is good
enough for R2000.  As programs we provide a reader, a writer, a
re-writer (i.e. SaveAS), an initial basic SVG and Postscript
conversion, experimental dxf and json converters, dwggrep to search
for text, dwglayer to print the list of layers, and dwgfilter to use
JQ expressions to query or change a DWG.

%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries, perl5 and python bindings
and header files for developing applications that use %{name}.
For more serious development use the git repository, and add parallel,
timeout and potion.

%package -n     python3-LibreDWG
Summary:        Python bindings for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}, python

%description -n python3-LibreDWG
The python3-LibreDWG package contains the python bindings for developing
applications that use %{name}.

%package -n     perl-LibreDWG
Summary:        Perl bindings for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}, perl

%description -n perl-LibreDWG
The perl-LibreDWG package contains the perl bindings for developing
applications that use %{name}.

%prep
%autosetup

%build
%configure --disable-static --with-perl-install=vendor
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool
%make_build PERL=/usr/bin/perl PYTHON=/usr/bin/python
%make_build pdf

%check
%make_build check

%install
rm -rf $RPM_BUILD_ROOT
%make_install
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'
rm $RPM_BUILD_ROOT%{_libdir}/perl5/perllocal.pod
rm $RPM_BUILD_ROOT%{perl_vendorarch}/auto/LibreDWG/.packlist
#perl EUMM sets it read-only, objcopy needs write
chmod u+w $RPM_BUILD_ROOT%{perl_vendorarch}/auto/LibreDWG/LibreDWG.so

%post
/sbin/ldconfig
/sbin/install-info %{_infodir}/LibreDWG.info %{_infodir}/dir || :

%preun
if [ $1 = 0 ] ; then
/sbin/install-info --delete %{_infodir}/LibreDWG.info %{_infodir}/dir || :
fi

%ldconfig_postun

%files
%license COPYING
%doc README AUTHORS NEWS doc/LibreDWG.pdf
%{_bindir}/dwg2SVG
%{_bindir}/dwg2dxf
%{_bindir}/dwg2ps
%{_bindir}/dwgbmp
%{_bindir}/dwgadd
%{_bindir}/dwgfilter
%{_bindir}/dwggrep
%{_bindir}/dwglayers
%{_bindir}/dwgread
%{_bindir}/dwgrewrite
%{_bindir}/dwgwrite
%{_bindir}/dxf2dwg
%{_bindir}/dxfwrite
%{_libdir}/libredwg.so.0
%{_libdir}/libredwg.so.0.0.12
%{_mandir}/man1/dwg2SVG.1.gz
%{_mandir}/man1/dwg2dxf.1.gz
%{_mandir}/man1/dwg2ps.1.gz
%{_mandir}/man1/dwgadd.1.gz
%{_mandir}/man1/dwgadd.5.gz
%{_mandir}/man1/dwgbmp.1.gz
%{_mandir}/man1/dwgfilter.1.gz
%{_mandir}/man1/dwggrep.1.gz
%{_mandir}/man1/dwglayers.1.gz
%{_mandir}/man1/dwgread.1.gz
%{_mandir}/man1/dwgrewrite.1.gz
%{_mandir}/man1/dwgwrite.1.gz
%{_mandir}/man1/dxf2dwg.1.gz
%{_mandir}/man1/dxfwrite.1.gz
%{_infodir}/LibreDWG.info.gz
%{_sharedir}/dwgadd.example

%files devel
%doc HACKING TODO
%{_includedir}/dwg.h
%{_includedir}/dwg_api.h
%{_libdir}/libredwg.so
%{_libdir}/pkgconfig/libredwg.pc

%files -n python3-LibreDWG
%{python_sitelib}/LibreDWG.py
%{python_sitelib}/__pycache__/LibreDWG.*
%{python_sitearch}/_LibreDWG.so*

%files -n perl-LibreDWG
%{perl_vendorarch}/LibreDWG.pm
%{perl_vendorarch}/auto/LibreDWG/LibreDWG.so
#TODO add to {_libdir}/perl5/perllocal.pod

%changelog
* Thu 11 Mar 2021 Reini Urban <reini.urban@gmail.com> 0.12.4-1
- upstream update. Minor fixes

* Fri 26 Feb 2021 Reini Urban <reini.urban@gmail.com> 0.12.3-1
- upstream update. Minor fixes

* Tue 23 Feb 2021 Reini Urban <reini.urban@gmail.com> 0.12.2-1
- upstream update. Minor fixes

* Sat 16 Jan 2021 Reini Urban <reini.urban@gmail.com> 0.12.1-1
- upstream update. Security fixes

* Thu 31 Dec 2020 Reini Urban <reini.urban@gmail.com> 0.12-1
- upstream update. Add dxfadd

* Mon 16 Nov 2020 Reini Urban <reini.urban@gmail.com> 0.11.1-1
- upstream update. Add dxfwrite

* Fri Aug 7 2020 Reini Urban <reini.urban@gmail.com> 0.11-1
- upstream update

* Wed Feb 19 2020 Reini Urban <reini.urban@gmail.com> 0.10.1.2915-1
- with dwgfilter and dwgwrite, from github pre-releases

* Mon Feb 17 2020 Reini Urban <reini.urban@gmail.com> 0.10.1-2
- installvendor patch, added pslib

* Sat Feb 15 2020 Reini Urban <reini.urban@gmail.com> 0.10.1-1
- Initial version tested on fc31
