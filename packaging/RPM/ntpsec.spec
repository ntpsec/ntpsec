Name:		ntpsec
Version:	0.9.7
Release:	1%{?dist}
Summary:	A secure, hardened, and improved implementation of Network Time Protocol derived from NTP Classic, Dave Mills’s original.

License:	BSD
URL:		https://www.ntpsec.org/
Source0:	ftp://ftp.ntpsec.org/pub/releases/ntpsec-0.9.7.tar.gz

%description
NTPsec, as its name implies, is a more secure NTP. Our goal is to deliver code that can be used with confidence in deployments with the most stringent security, availability, and assurance requirements.

Towards that end we apply best practices and state-of-the art technology in code auditing, verification, and testing. We begin with the most important best practice: true open-source code review. The NTPsec code is available in a public git repository. One of our goals is to support broader community participation.


BuildRequires:  bison openssl-devel libcap-devel libseccomp-devel pps-tools-devel libcap openssl-libs pps-tools avahi-compat-libdns_sd-devel

%global debug_package %{nil}

%prep
%setup -q


%build
# %%configure
./waf configure


%install
# %%make_install
./waf install


%files
%doc



