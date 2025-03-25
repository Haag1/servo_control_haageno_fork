# Servo control

Skeleton project for setting up a virtual camera in threepp and display content in OpenCV.

### OpenCV dependency

This project requires OpenCV. See below.

#### Manually fetch OpenCV (preferred)
1. Download from https://opencv.org/releases/
2. Run self-extracting archive and extract to a suitable location
3. Create an environmental variable named 'OpenCV_DIR' pointing to `/build`
4. Add `/bin`  folder to PATH (e.g. `.../build/x64/vc16/bin`)

#### Utilize vcpkg (using manifest mode)
Call CMake with `-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake`

>Under MinGW you'll need to specify the vcpkg triplet:
>
>-DVCPKG_TARGET_TRIPLET=x64-mingw-[static|dynamic] # choose either static or dynamic.
>-DVCPKG_HOST_TRIPLET=x64-mingw-[static|dynamic] # <-- needed only if MSVC cannot be found.
