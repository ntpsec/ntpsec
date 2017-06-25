#
# spec file for package ntpsec
#
# Copyright (c) 2017 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (c) 2016 Malcolm J Lewis <malcolmlewis@opensuse.org>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


%if 0%{?suse_version} >= 1320 || 0%{?is_opensuse}
%bcond_without libbsd
%else
%bcond_with libbsd
%endif
Name:           ntpsec
Version:        0.9.7+git20170607.7c0008c91
Release:        0
Summary:        Improved implementation of Network Time Protocol
License:        BSD-2-Clause and NTP and BSD-3-Clause and MIT
Group:          System/Base
Url:            https://www.ntpsec.org/
Source0:        ntpsec-%{version}.tar.xz
Source3:        %{name}.changes
Source4:        logrotate.ntp
Source7:        ntp.firewall
Source8:        ntp.conf
BuildRequires:  asciidoc
BuildRequires:  avahi-compat-mDNSResponder-devel
BuildRequires:  bison
BuildRequires:  fdupes
# Needed for waf init in the git snapshot
BuildRequires:  git-core
BuildRequires:  libcap-devel
BuildRequires:  libxslt-tools
# Required for tests to pass
BuildRequires:  netcfg
BuildRequires:  openssl-devel
BuildRequires:  pkgconfig
BuildRequires:  pps-tools-devel
BuildRequires:  pkgconfig(libevent)
BuildRequires:  pkgconfig(libseccomp)
BuildRequires:  pkgconfig(python)
Requires:       netcfg
Requires:       ntpsec-utils
Requires(pre):  pwdutils
Recommends:     logrotate
# For ntpleapfetch
Recommends:     wget
Conflicts:      otherproviders(ntp)
Provides:       ntp = 4.2.9
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
%if %{with libbsd}
BuildRequires:  pkgconfig(libbsd)
%endif

%description
A more secure implementation of NTP, derived from NTP Classic, Dave
Mills’s original.

%package -n python-ntp
Summary:        Python ntpsec bindings
Group:          Development/Languages/Python

%description -n python-ntp
The ntpsec python bindings used by various ntp utilities.

%package utils
Summary:        Utilities and commands for ntp
Group:          System/Base
Requires:       %{name} = %{version}
# For ntploggps
Requires:       python-gpsd
Requires:       python-ntp
# For ntpviz
Recommends:     python-psutil
Conflicts:      ntp < 4.2.9

%description utils
The ntpsec utilities relying on the python module of ntp

%prep
%setup -q
# Fix python shebangs
sed -i -e 's:#!%{_bindir}/env python:#!%{_bindir}/python2:' \
    ntpclients/*

%build
# We use the date from the changes file
epoch=`date --date "@\`stat --format %%Y %{SOURCE3}\`" +"%%s"`

export CFLAGS="%{optflags}"
export CCFLAGS="%{optflags}"
./waf configure \
    --build-epoch="$epoch" \
    --enable-debug \
    --prefix=%{_prefix} \
    --mandir="%{_mandir}" \
    --python=%{_bindir}/python2 \
    --pythonarchdir=%{python_sitearch} \
    --pythondir=%{python_sitearch} \
    --enable-seccomp \
    --enable-debug-gdb \
    --enable-early-droproot \
    --enable-leap-smear \
    --enable-mssntp \
    --enable-lockclock \
    --refclock=all
./waf build --verbose %{?_smp_mflags}

%install
./waf install --destdir=%{buildroot}

# Use correct path in unit file
sed -i "s|bin|sbin|g" etc/ntpd.service

# FIXME: As long as systemdenable switch for install is broken
install -m 0644 -D etc/ntpd.service %{buildroot}/%{_unitdir}/ntpd.service
install -m 0644 -D etc/ntp-wait.service %{buildroot}/%{_unitdir}/ntp-wait.service
ln -s service %{buildroot}%{_sbindir}/rcntpd
ln -s service %{buildroot}%{_sbindir}/rcntp-wait

install -m 0644 -D %{SOURCE4} %{buildroot}%{_sysconfdir}/logrotate.d/ntp
install -m 0644 -D %{SOURCE7} %{buildroot}%{_sysconfdir}/sysconfig/SuSEfirewall2.d/services/ntp
install -m 0644 -D %{SOURCE8} %{buildroot}%{_sysconfdir}/ntp.conf

%fdupes -s %{buildroot}

%check
./waf check --verbose %{?_smp_mflags}

%pre
getent group ntp >/dev/null || groupadd -r ntp
getent passwd ntp >/dev/null || useradd -u 74 -r -g ntp -d %{_localstatedir}/lib/ntp -s /sbin/nologin -c "NTP daemon" ntp
%service_add_pre ntp.service ntpd.service
exit 0

%pre utils
%service_add_pre ntp-wait.service

%post
%service_add_post ntpd.service

%post utils
%service_add_post ntp-wait.service

%preun
%service_del_preun ntpd.service

%preun utils
%service_del_preun ntp-wait.service

%postun
%service_del_postun ntpd.service

%postun utils
%service_del_postun ntp-wait.service

%files -n python-ntp
%defattr(-,root,root)
%{python_sitearch}/ntp

%files utils
%defattr(-,root,root)
%{_bindir}/ntploggps
%{_bindir}/ntpdig
%{_bindir}/ntpkeygen
%{_bindir}/ntpmon
%{_bindir}/ntpq
%{_bindir}/ntpsweep
%{_bindir}/ntptrace
%{_bindir}/ntpviz
%{_bindir}/ntpwait
%{_bindir}/ntplogtemp
%{_mandir}/man1/ntploggps.1%{?ext_man}
%{_mandir}/man1/ntpdig.1%{?ext_man}
%{_mandir}/man8/ntpkeygen.8%{?ext_man}
%{_mandir}/man1/ntpmon.1%{?ext_man}
%{_mandir}/man1/ntpq.1%{?ext_man}
%{_mandir}/man1/ntpsweep.1%{?ext_man}
%{_mandir}/man1/ntptrace.1%{?ext_man}
%{_mandir}/man1/ntpviz.1%{?ext_man}
%{_mandir}/man8/ntpwait.8%{?ext_man}
%{_mandir}/man1/ntplogtemp.1%{?ext_man}
%{_sbindir}/rcntp-wait
%{_unitdir}/ntp-wait.service

%files
%defattr(-,root,root)
%doc NEWS README LICENSE
%config(noreplace) %{_sysconfdir}/ntp.conf
%{_sbindir}/rcntpd
%{_bindir}/ntpfrob
%{_bindir}/ntpleapfetch
%{_bindir}/ntptime
%{_sbindir}/ntpd
%{_mandir}/man5/ntp.conf.5%{?ext_man}
%{_mandir}/man5/ntp.keys.5%{?ext_man}
%{_mandir}/man8/ntpd.8%{?ext_man}
%{_mandir}/man8/ntpfrob.8%{?ext_man}
%{_mandir}/man8/ntpleapfetch.8%{?ext_man}
%{_mandir}/man8/ntptime.8%{?ext_man}
%config %{_sysconfdir}/logrotate.d/ntp
%config %{_sysconfdir}/sysconfig/SuSEfirewall2.d/services/ntp
%{_unitdir}/ntpd.service

%changelog
