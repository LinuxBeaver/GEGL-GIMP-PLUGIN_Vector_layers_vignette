Download binaries here. (FOLLOW INSTRUCTIONS)
[https://github.com/LinuxBeaver/GEGL-glossy-balloon-text-styling/releases](https://github.com/LinuxBeaver/Vector_Layers_in_GIMP_via_vignette/releases)

# Vector layers in GIMP rendered by GEGL via exploiting vignette
This plugin calls gegl:vignette and my invert transparency and ssg plugin together to render basic squares, circles and more. Cocidentially it ships with plugins "stroke shadow glow" and "invert transparency" as bonus filters. You can learn more about bonus filters here

https://github.com/LinuxBeaver/GEGL-SSG-Stroke-Shadow-Glow-/

![image](https://github.com/LinuxBeaver/Vector_Layers_in_GIMP_via_vignette/assets/78667207/52676d65-8f4d-48e3-9d9c-186b7af94813)


## OS specific location to put GEGL Filter binaries 

**Windows**
C:\Users\USERNAME\AppData\Local\gegl-0.4\plug-ins
 
** Linux **
 /home/(USERNAME)/.local/share/gegl-0.4/plug-ins
 
 **Linux (Flatpak)**
 /home/(USERNAME)/.var/app/org.gimp.GIMP/data/gegl-0.4/plug-ins

![image](https://github.com/LinuxBeaver/GEGL-glossy-balloon-text-styling/assets/78667207/f15fb5eb-c8d7-4c08-bbac-97048864e657)


## Compiling and Installing
**Linux**

To compile and install you will need the GEGL header files (libgegl-dev on Debian based distributions or gegl on Arch Linux) and meson (meson on most distributions).

meson setup --buildtype=release build
ninja -C build


If you have an older version of gegl you may need to copy to ~/.local/share/gegl-0.3/plug-ins instead (on Ubuntu 18.04 for example).

**Windows**

The easiest way to compile this project on Windows is by using msys2. Download and install it from here: https://www.msys2.org/

Open a msys2 terminal with C:\msys64\mingw64.exe. Run the following to install required build dependencies:

pacman --noconfirm -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-meson mingw-w64-x86_64-gegl

Then build the same way you would on Linux:

meson setup --buildtype=release build
ninja -C build

## Extra preview of this based Gimp plugin

![image](https://github.com/LinuxBeaver/Vector_Layers_in_GIMP_via_vignette/assets/78667207/7e7c497b-953c-4824-b035-4d73fb87e38b)

![image](https://github.com/LinuxBeaver/Vector_Layers_in_GIMP_via_vignette/assets/78667207/c18535ff-a086-410a-81f4-4b0ce30725e9)

![image](https://github.com/LinuxBeaver/Vector_Layers_in_GIMP_via_vignette/assets/78667207/3904d200-1bf8-415a-963e-8aa5f4fba571)


## Enjoy
