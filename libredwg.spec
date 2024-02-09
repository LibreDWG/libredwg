# -*- sh -*-
Name:           libredwg
Version:        0.13.1
Release:        1%{?dist}
Summary:        GNU C library and programs to read and write DWG files

License:        GPLv3+
URL:            https://www.gnu.org/software/libredwg/
#Source0:        https://ftp.gnu.org/gnu/libredwg/libredwg-%%{version}.tar.xz
Source0:        https://github.com/LibreDWG/libredwg/releases/download/%{version}/libredwg-%{version}.tar.xz

BuildRequires:  gcc
BuildRequires:  swig
BuildRequires:  python3-devel
BuildRequires:  perl-devel
BuildRequires:  perl-macros
BuildRequires:  perl(Convert::Binary::C)
BuildRequires:  perl(ExtUtils::Embed)
BuildRequires:  pcre2-devel
BuildRequires:  pslib-devel
BuildRequires:  libasan
BuildRequires:  libubsan
BuildRequires:  texinfo
BuildRequires:  texinfo-tex
# Required for tests.
BuildRequires:  python3-libxml2
BuildRequires:  pcre2
BuildRequires:  pcre2-utf16

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
%make_build PERL=/usr/bin/perl PYTHON=%{python3}
%make_build pdf


%check
# TODO: Figure out how to make tests work inside Mock.
# %%make_build check


%install
%make_install

# Remove static libraries.
find %{buildroot} -name '*.la' -exec rm -f {} ';'

# Remove perllocal.pod and packlist files.
# TODO: Try preventing their generation with something like:
# perl Makefile.PL INSTALLDIRS=vendor NO_PACKLIST=1 NO_PERLLOCAL=1
# Source: https://fedoraproject.org/wiki/Perl/Tips#Best_practices_for_the_latest_Fedora
rm %{buildroot}/%{perl_archlib}/perllocal.pod
rm %{buildroot}/%{perl_vendorarch}/auto/LibreDWG/.packlist

# Remove examples placed directly in /usr/share directory.
rm %{buildroot}/%{_datadir}/load_dwg.py
rm %{buildroot}/%{_datadir}/dwgadd.example

# Remove Info file.
rm %{buildroot}/%{_infodir}/dir

# Perl EUMM sets it read-only, but objcopy needs write access.
chmod u+w %{buildroot}/%{perl_vendorarch}/auto/LibreDWG/LibreDWG.so


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
%{_mandir}/man5/dwg*
%{_infodir}/LibreDWG.info*
%{_sharedir}/libredwg/dwgadd.example

%files devel
%doc TODO
%{_includedir}/dwg.h
%{_includedir}/dwg_api.h
%{_libdir}/libredwg.so
%{_libdir}/pkgconfig/libredwg.pc

%files -n python3-LibreDWG
%{python3_sitelib}/LibreDWG.py
%{python3_sitelib}/__pycache__/LibreDWG.*
%{python3_sitearch}/_LibreDWG.so*

%files -n perl-LibreDWG
%{perl_vendorarch}/LibreDWG.pm
%{perl_vendorarch}/auto/LibreDWG/LibreDWG.so


%changelog
* Fri Feb 9 2024 Reini Urban <reini.urban@gmail.com> 0.13.1-1
- upstream update.

* Fri Feb 4 2024 Reini Urban <reini.urban@gmail.com> 0.13.0-1
- upstream update.

* Fri Feb 4 2022 Reini Urban <reini.urban@gmail.com> 0.12.5-1
- upstream update.

* Thu Jun 10 2021 Tadej Janež <tadej.j@nez.si> 0.12.4-2
- Refactor the Spec file

* Thu Mar 11 2021 Reini Urban <reini.urban@gmail.com> 0.12.4-1
- upstream update. Minor fixes

* Fri Feb 26 2021 Reini Urban <reini.urban@gmail.com> 0.12.3-1
- upstream update. Minor fixes

* Tue Feb 23 2021 Reini Urban <reini.urban@gmail.com> 0.12.2-1
- upstream update. Minor fixes

* Sat Jan 16 2021 Reini Urban <reini.urban@gmail.com> 0.12.1-1
- upstream update. Security fixes

* Thu Dec 31 2020 Reini Urban <reini.urban@gmail.com> 0.12-1
- upstream update. Add dxfadd

* Mon Nov 16 2020 Reini Urban <reini.urban@gmail.com> 0.11.1-1
- upstream update. Add dxfwrite

* Fri Aug 7 2020 Reini Urban <reini.urban@gmail.com> 0.11-1
- upstream update

* Wed Feb 19 2020 Reini Urban <reini.urban@gmail.com> 0.10.1.2915-1
- with dwgfilter and dwgwrite, from github pre-releases

* Mon Feb 17 2020 Reini Urban <reini.urban@gmail.com> 0.10.1-2
- installvendor patch, added pslib

* Sat Feb 15 2020 Reini Urban <reini.urban@gmail.com> 0.10.1-1
- Initial version tested on fc31
