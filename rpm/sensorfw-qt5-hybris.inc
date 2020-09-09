Summary:    Sensor Framework Qt5
License:    LGPLv2+
URL:        https://git.sailfishos.org/mer-core/sensorfw
Source0:    %{name}-%{version}.tar.bz2
# Eventhough this is not really needed we have this here
# as this is added to packaging dir by tar_git.
Source2:    sensorfwd.service
Source3:    sensorfw-qt5-hybris.inc
Requires:   sensord-qt5
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(sensord-qt5)
Obsoletes: %{name}-configs <= 0.8.17

%description
Sensor Framework provides an interface to hardware sensor drivers through logical sensors. This package contains sensor framework daemon and required libraries.

%package devel
Summary:    Sensor framework daemon libraries development headers
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qtcore-devel
Requires:   qt5-qtdbus-devel
Requires:   qt5-qtnetwork-devel

%description devel
Development headers for sensor framework daemon and libraries.

%prep
%autosetup -n %{name}-%{version}

%build
unset LD_AS_NEEDED
export LD_RUN_PATH=%{_libdir}/sensord-qt5/

%qmake5 CONFIG+=hybris %{qmake_extra_parameters} PC_VERSION=`echo %{version} | sed 's/+.*//'`

%make_build

%install
%qmake5_install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root,-)
%license COPYING
%{_libdir}/libhybrissensorfw*.so.*
%{_libdir}/libhybrissensorfw*.so
%{_libdir}/sensord-qt5/*.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/sensord-qt5/*
