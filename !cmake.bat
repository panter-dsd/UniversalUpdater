mkdir build_win
cd build_win
cmake -D CMAKE_BUILD_TYPE=Debug -D QTSINGLEAPPLICATION_DIR=c:/panter/QtSingleApplication/ -G "MinGW Makefiles" ..
cd ..

mkdir build_win_release
cd build_win_release
set QTDIR=c:\panter\qt\4.6.3
set PATH=c:\panter\qt\4.6.3\bin;%PATH%
cmake -D CMAKE_BUILD_TYPE=Release -D QTSINGLEAPPLICATION_DIR=c:/panter/QtSingleApplication/ -G "MinGW Makefiles" ..
cd ..

