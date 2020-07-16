pushd %~dp0
git submodule update --init --recursive
pushd vcpkg
call bootstrap-vcpkg.bat
call vcpkg install qt5-base[core]:x64-windows
call vcpkg install qt5-svg:x64-windows
call vcpkg install azure-storage-cpp:x64-windows
popd
popd