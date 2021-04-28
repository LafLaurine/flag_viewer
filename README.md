# Flag viewer

A flag viewer made by Laurine LAFONTAINE for a master class at Université Gustave Eiffel, IGM

## How to run the program ?

Ideally, it is better to use VSCode and import the project directly. Indeed, everything is configured to work with VSCode. Don't forget to configure Cmake. When VSCode is launched, you need to compile (F7) and run the program (F5).

Otherwise, you can compile and launch the project by hand following these lines:

```git clone https://github.com/LafLaurine/flag_viewer
cd flag_viewer
mkdir build
cd build
cmake ..
make -j
./bin/flag-viewer viewer
```

## Commands

Hold the **middle click** and move your mouse to turn around the scene\
Use the **up and down arrow keys** to move the camera up and down\
Use **Z,S** to zoom in or out in the scene\
Use **A,E** to rotate the camera\
Use **Q,D** to move left or right
