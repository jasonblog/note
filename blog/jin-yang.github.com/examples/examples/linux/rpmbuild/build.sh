#!/bin/bash

WORKSPACE=/tmp/foobar

echo "0. Prepare dirs"
mkdir -pv ${WORKSPACE}/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS,Workspace/foobar-1.0.0}

echo "1. Generate tar"
cd ${WORKSPACE}/Workspace/foobar-1.0.0
cat << EOF > main.c
#include <stdio.h>
int main(int argc, char *argv)
{
    printf("Hello World!!!\n");
    return 0;
}
EOF
cat << "EOF" > Makefile
all: main.c
	gcc -o foobar $< -Wall
install:
	install -m 755 foobar ${DESTDIR}/usr/bin/foobar
EOF
cd ${WORKSPACE}/Workspace
tar -jcf foobar-1.0.0.tar.bz2 foobar-1.0.0
#[ "${WORKSPACE}/BUILD" != "/" ] && rm -rf "${WORKSPACE}/BUILD/"
rm -rf "${WORKSPACE}/SOURCES/foobar-1.0.0.tar.bz2"
mv foobar-1.0.0.tar.bz2 ${WORKSPACE}/SOURCES/

echo "2. Prepare spec-file"
cd ${WORKSPACE}
cat << EOF > SPECS/foobar-1.0.0.spec
### 0. The define section
%global _topdir ${WORKSPACE}

### 1.The introduction section
Name:           foobar
Version:        1.0.0
Release:        1%{?dist}
Summary:        Just a RPM example
License:        GPLv2+ and BSD
BuildRequires:  gcc,make
Source0:        %{name}-%{version}.tar.bz2
BuildRoot:      %_topdir/BUILDROOT

%description
It is a RPM example.

#--- 2.The Prep section
%prep
%setup -q
#%patch0 -p1

#--- 3.The Build Section
%build
make %{?_smp_mflags}

#--- 4.Install section
%install
#/usr/bin/
install -d -m 0751 %{buildroot}/%{_bindir}
make install DESTDIR=%{buildroot}
#--- 4.1 scripts section
%pre
echo "pre" >> /tmp/foobar.txt
%post
echo "post" >> /tmp/foobar.txt
%preun
echo "preun" >> /tmp/foobar.txt
%postun
echo "postun" >> /tmp/foobar.txt

#--- 5.clean section
%clean
#rm -rf %{buildroot}

#--- 6.file section
%files
%defattr(-,root,root,-)
%attr(755, root, root) %{_bindir}/foobar

#--- 7.chagelog section
%changelog
* Fri Dec 29 2012 foobar foobar@kidding.com - 1.0.0-1
- Initial version
EOF

echo "3. Perform rpmbuild"
cd ${WORKSPACE}
#rpmbuild --clean --define '_topdir /tmp/foobar' -ba SPECS/foobar-1.0.0.spec
#rpmbuild --define '_topdir /tmp/foobar' -ba SPECS/foobar-1.0.0.spec
rpmbuild --define '_topdir /tmp/foobar' -ba SPECS/foobar-1.0.0.spec
