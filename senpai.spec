Name:		senpai
Version:	%VERSION_PLACEHOLDER%
Release:	1
Summary:	Molecular dynamics simulator
License:	GPLv3
URL:		https://senpaimd.org
Vendor:		SENPAI Molecular Dynamics
BuildArch:	x86_64
BuildRequires:	gcc make

%description
The SENPAI molecular dynamics (MD) simulator

%prep
%setup -q -n SENPAI

%build
make

%install
mkdir -p %{buildroot}/usr/bin
install -m 0755 senpai %{buildroot}/usr/bin/senpai

%files
%attr(755,root,root) /usr/bin/senpai

%changelog
* Wed Jun 21 2023 Sasha Emily Chelsea Murgia <mail@chelsea486mhz.fr> 1.1-1
- Modified SPEC file to allow GitHub actions
* Fri Apr 29 2022 Sasha Emily Chelsea Murgia <mail@chelsea486mhz.fr> 1.1-1
- Initial RPM release
