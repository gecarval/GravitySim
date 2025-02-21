<h2 align="center">
	REPOSITORY STATS
</h2>

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/gecarval/GravitySim?color=lightblue" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/gecarval/GravitySim?color=yellow" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/gecarval/GravitySim?color=blue" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/gecarval/GravitySim?color=green" />
</p>

# Gravity Simulator
This is a project written in C using the MiniLibX as the API wich simulates Gravity and Colissions using the Burns-Hut algorithm for gravity aproximation and quadtree for collision detection.

# Burnes-Hut algorithm

# Installation
The project uses the [MiniLibX](https://harm-smits.github.io/42docs/libs/minilibx) using the X11 Window system in the Linux Operating System, after the instalation of all dependencies you should do as required:

- Clone the repository;
``` sh
git clone https://github.com/gecarval/GravitySim.git
```
- Move to the folder;
``` sh
cd GravitySim
```
- Compile the project;
``` sh
make
```
- execute the program;
``` sh
./render [Put Window Width] [Put Window Height]
```
- execution examples:
``` sh
./render 800 600
```
``` sh
./render 1600 900
```
``` sh
./render 1900 1000
```
## Controls
|BUTTON|ACTION|
|---|---|
|`T`| Reset Positions|
|`Y`| Show Quadtree|
|`U`| Show HashTable|
|`LMB`| activate/deactivate brush|
|`Esc`| exit|
|`[`| increase speed|
|`]`| decrease speed|
|`1`| on/off Gravity Sim|
|`Space`| clear screen|
