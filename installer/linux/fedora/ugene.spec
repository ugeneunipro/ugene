Name:    ugene
Summary: Integrated bioinformatics toolkit
Version: 1.28.0
Release: 7%{?dist}
#The entire source code is GPLv2+ except:
#file src/libs_3rdparty/qtbindings_core/src/qtscriptconcurrent.h which is GPLv2
#files in src/plugins_3rdparty/script_debuger/src/qtscriptdebug/ which are GPLv2
License: GPLv2+ and GPLv2
Group:   Applications/Engineering
URL:     http://ugene.net
Source0: http://ugene.net/downloads/%{name}-%{version}.tar.gz

BuildRequires: desktop-file-utils
BuildRequires: mesa-libGLU-devel
BuildRequires: qt5-qtbase-devel qt5-qtbase-private-devel
BuildRequires: qt5-qtbase-mysql
BuildRequires: qt5-qtmultimedia-devel
BuildRequires: qt5-qtscript-devel
BuildRequires: qt5-qtsensors-devel
BuildRequires: qt5-qtsvg-devel
BuildRequires: qt5-qttools-devel
BuildRequires: qt5-qtwebchannel-devel
BuildRequires: qt5-qtwebkit-devel
BuildRequires: qt5-qtxmlpatterns-devel
BuildRequires: zlib-devel

BuildConflicts: qt-devel

#We need strict versions of qt for correct work of src/libs_3rdparty/qtbindings_*
%{?_qt5:Requires: %{_qt5}%{?_isa} = %{_qt5_version}}

Provides: bundled(sqlite)
Provides: bundled(samtools)
ExclusiveArch: %{ix86} x86_64

%description
Unipro UGENE is a cross-platform visual environment for DNA and protein
sequence analysis. UGENE integrates the most important bioinformatics
computational algorithms and provides an easy-to-use GUI for performing
complex analysis of the genomic data. One of the main features of UGENE
is a designer for custom bioinformatics workflows.

%prep
%setup -q

%build
%{qmake_qt5} -r \
        INSTALL_BINDIR=%{_bindir} \
        INSTALL_LIBDIR=%{_libdir} \
        INSTALL_DATADIR=%{_datadir} \
        INSTALL_MANDIR=%{_mandir} \
%if 0%{?_ugene_with_non_free}
        UGENE_WITHOUT_NON_FREE=0 \
%else
        UGENE_WITHOUT_NON_FREE=1 \
%endif
        UGENE_EXCLUDE_LIST_ENABLED=1


make %{?_smp_mflags}

%install
make install INSTALL_ROOT=%{buildroot}
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop

%post
touch --no-create %{_datadir}/icons/hicolor &> /dev/null || :
touch --no-create %{_datadir}/mime/packages &> /dev/null || :

%posttrans
gtk-update-icon-cache %{_datadir}/icons/hicolor &> /dev/null || :
update-desktop-database -q &> /dev/null ||:
update-mime-database %{?fedora:-n} %{_datadir}/mime &> /dev/null || :

%postun
if [ $1 -eq 0 ] ; then
touch --no-create %{_datadir}/icons/hicolor &> /dev/null || :
gtk-update-icon-cache %{_datadir}/icons/hicolor &> /dev/null || :
update-desktop-database -q &> /dev/null ||:
touch --no-create %{_datadir}/mime/packages &> /dev/null || :
update-mime-database %{?fedora:-n} %{_datadir}/mime &> /dev/null || :
fi

%files
%{!?_licensedir:%global license %%doc}
%license COPYRIGHT LICENSE.txt LICENSE.3rd_party.txt
%{_bindir}/*
%{_libdir}/%{name}/
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/ugene.*
%{_datadir}/icons/hicolor/*/*/*
%{_datadir}/mime/packages/*.xml
%{_datadir}/%{name}/
%{_mandir}/man1/*

%changelog
* Mon Aug 28 2017 Yuliya Algaer <yalgaer@fedoraproject.org> - 1.27.0-7
- New upstream release

* Thu Aug 03 2017 Fedora Release Engineering <releng@fedoraproject.org> - 1.26.3-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_27_Binutils_Mass_Rebuild
