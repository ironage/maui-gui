configure -prefix %CD%\build -release -static -static-runtime -target xp -platform win32-msvc2013 -accessibility -no-opengl -no-icu -no-sql-sqlite -no-qml-debug -nomake examples -nomake tests -skip qtactiveqt -skip qtenginio -skip qtlocation -skip qtmultimedia -skip qtserialport -skip qtquickcontrols -skip qtsensors  -skip qtwebsockets -skip qtxmlpatterns -skip qt3d -skip qtwebview -skip qtgraphicaleffects -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-freetype -opengl desktop -qt-sql-sqlite -no-openssl -opensource -confirm-license

**Place/update all files in maui-gui\qt-src\installer\packages\MAUI\data
**Increment version number in packages/config

** If the dependencies change or Qt is upgraded, gather shared library dependencies:
"D:\Qt\5.12.5\msvc2017_64\bin\windeployqt.exe" --qmldir ../qt-src/ ../build-maui-gui-Desktop_Qt_5_12_5_MSVC2017_64bit-Release/release/maui-gui.exe -core -qml -quick -multimedia -network
The version of openssl that needs to be deployed has to match the one used by Qt.Find out with this:
qDebug() << "requires ssl: " << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionNumber() << QSslSocket::sslLibraryVersionString();
currently is: requires ssl:  true 269488175 "OpenSSL 1.1.1c  28 May 2019"


**Generate the online repository:
cd maui-gui/qt-src/installer
repogen.exe -p packages maui-gui-repo 
**OR
repogen.exe --update-new-components -p packages maui-gui-repo
**upload to http://www.hedgehogmedical.com/downloads/maui-gui-repo
scp -r maui-gui-repo hedgehp2@server.hedgehogmedical.com:~/public_html/downloads/

binarycreator.exe --online-only -c config\config.xml -p packages maui-installer.exe

[for offline only installers use: 
../../../../tools/build-installerfw-static64msvc2013-Release/bin/binarycreator.exe --offline-only -c config\config.xml -p packages maui-installer.exe
]

**See http://doc.qt.io/qtinstallerframework/qt-installer-framework-online-example.html

RELEASE PROCESS:
Increase CURRENT_VERSION in MRemoteInterface.cpp
Add CONFIG += console in maui-gui.pro, clean, build, copy the exe to installer/packages/maui/data as maui-gui-console.exe
Take out console from maui-gui.pro, clean, build, copy the exe to installer/packages/maui/data as maui-gui.exe
Change installer/packages/MAUI/meta/package.xml, increase version (this can be different, nobody sees it) and change release date
Rebuild repo: repogen.exe --update-new-components -p packages maui-gui-repo
copy to server: scp -r maui-gui-repo hedgehp2@server.hedgehogmedical.com:~/public_html/downloads/
modify ~/djdev/maui-server/mauisky/welcome/views.py change version to CURRENT_VERSION of GUI
touch ~/public_html/cgi-bin/maui.fcgi to get the server to reload the python files.
test upgrade locally

