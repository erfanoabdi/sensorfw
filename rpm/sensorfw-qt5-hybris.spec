Name: hybris-libsensorfw-qt5-hal
Provides: hybris-libsensorfw-qt5 = %{version}-%{release}
Conflicts: hybris-libsensorfw-qt5-binder
Obsoletes: hybris-libsensorfw-qt5 < %{version}-%{release}

BuildRequires:  pkgconfig(libhardware)
BuildRequires:  pkgconfig(android-headers)

# additional parameters for qmake
%define qmake_extra_parameters %{nil}

%include rpm/sensorfw-qt5-hybris.inc
