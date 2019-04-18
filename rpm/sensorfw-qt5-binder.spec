Name: hybris-libsensorfw-qt5-binder
Provides: hybris-libsensorfw-qt5 = %{version}-%{release}
Conflicts: hybris-libsensorfw-qt5-hal
Obsoletes: hybris-libsensorfw-qt5 < %{version}-%{release}

BuildRequires: pkgconfig(libgbinder)

# additional parameters for qmake
%define qmake_extra_parameters CONFIG+=binder

%include rpm/sensorfw-qt5-hybris.inc
