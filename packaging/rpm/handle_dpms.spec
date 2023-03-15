%global ghurl   https://github.com/felfert/%{name}

Name:           handle_dpms
Version:        0.2
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
gcc %{optflags} -o %{name} %{name}.c %{build_ldflags}

%install
install -D -m 0644 %{name}.sysconfig %{buildroot}%{_sysconfdir}/sysconfig/%{name}
install -D -m 0644 %{name}.service %{buildroot}%{_unitdir}/%{name}.service
install -D -m 0750 dpms-event.sh %{buildroot}%{_libexecdir}/dpms-event.sh
install -D -m 0750 %{name} %{buildroot}%{_libexecdir}/%{name}

%files
%doc README.md
%config(noreplace) %{_sysconfdir}/sysconfig/%{name}
%{_libexecdir}/%{name}
%{_libexecdir}/dpms-event.sh
%{_unitdir}/%{name}.service

%post
%systemd_post %{name}.service

%preun
%systemd_preun %{name}.service

%postun
%systemd_postun_with_restart %{name}.service


%changelog
* Wed Mar 15 2023 Fritz Elfert <fritz-github@fritz-elfert.de> - 0.1-1
- Initial package.
