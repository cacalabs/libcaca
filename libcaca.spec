%define name  libcaca
%define version 0.5
%define release 1

Summary: render images on text terminals
Name: %{name}
Version: %{version}
Release: %{release}
Source0: %{name}-%{version}.tar.bz2
Copyright: GPL
Group: System Environment/Libs
BuildRoot: %{_tmppath}/%{name}-buildroot
Prefix: %{_prefix}
#URL: 


%description

library and viewer to render images on text terminals

%prep

case "${RPM_COMMAND:-all}" in
all)
%setup -q
;;esac

%build

case "${RPM_COMMAND:-all}" in
all)
#export CFLAGS="$RPM_OPT_FLAGS -O1 -g" 
#./configure --prefix=%{prefix}

%configure  --enable-x11 --enable-imlib

;;esac

make 

%install

#install -d $RPM_BUILD_ROOT%{prefix}/bin
#install -d $RPM_BUILD_ROOT%{prefix}/man/man1
#make prefix=$RPM_BUILD_ROOT%{prefix} install 

%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc COPYING README TODO
%{_prefix}/*

%changelog
* Mon Dec 29 2003 Richard Zidlicky <rz@linux-m68k.org> 0.5-1
- created specfile

