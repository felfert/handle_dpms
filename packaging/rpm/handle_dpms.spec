%global ghurl   https://github.com/felfert/%{name}

Name:           handle_dpms
Version:        0.1
Release:        1%{?dist}
Summary:        Handle DPMS events

License:        MIT

Buildrequires:  coreutils
Buildrequires:  gcc
Buildrequires:  systemd-rpm-macros
Source:         %{ghurl}/archive/refs/tags/%{version}.tar.gz?fn=%{name}-%{version}.tar.gz
URL:            %{ghurl}

Requires: systemd
Requires: acpid

%description
This package provides a handler for DPMS plug and unplug events

%prep
%setup -q

%build
gcc -g -Wall -o %{name} %{name}.c

%install
install -D -m 0640 %{name}.service %{buildroot}%{_unitdir}/%{name}.service
install -D -m 0750 dpms-event.sh %{buildroot}%{_libexecdir}/dpms-event.sh
install -D -m 0750 %{name} %{buildroot}%{_bindir}/%{name}

%files
%doc README.md
%{_bindir}/%{name}
%{_unitdir}/%{name}.service
%{_libexecdir}/dpms-event.sh

%post
%systemd_post %{name}.service

%preun
%systemd_preun %{name}.service

%postun
%systemd_postun_with_restart %{name}.service


%changelog
* Wed Mar 15 2023 Fritz Elfert <fritz-github@fritz-elfert.de> - 0.1-1
- Initial package.
