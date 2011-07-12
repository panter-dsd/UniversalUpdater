#mkdir build_unix_release
#cd build_unix_release
#cmake -D QTSINGLEAPPLICATION_DIR=/mnt/work/program/qtsingleapplication/ -D CMAKE_BUILD_TYPE=Release ..
#cd ..

#mkdir build_unix_debug
#cd build_unix_debug
#cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -D QTSINGLEAPPLICATION_DIR=/mnt/work/program/qtsingleapplication/ -D CMAKE_BUILD_TYPE=Debug ..
#cd ..

cd /var/tmp
mkdir programming
cd programming
mkdir UU
cd UU

mkdir build_unix_release
cd build_unix_release
cmake -D QTSINGLEAPPLICATION_DIR=/mnt/work/program/qtsingleapplication/ -D CMAKE_BUILD_TYPE=Release /mnt/work/program/UniversalUpdater/
cd ..

mkdir build_unix_debug
cd build_unix_debug
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -D QTSINGLEAPPLICATION_DIR=/mnt/work/program/qtsingleapplication/ -D CMAKE_BUILD_TYPE=Debug /mnt/work/program/UniversalUpdater/
cd ..
