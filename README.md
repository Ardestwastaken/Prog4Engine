# Prog4Engine

A custom 2D game engine built on top of [SDL3](https://www.libsdl.org/) and [glm](https://github.com/g-truc/glm), developed as an assignment for the **Programming 4** course at [DAE (Digital Arts & Entertainment)](https://www.digitalartsandentertainment.be/) at Howest.

[![Build Status](https://github.com/avadae/minigin/actions/workflows/cmake.yml/badge.svg)](https://github.com/avadae/cmake/actions)
[![Build Status](https://github.com/avadae/minigin/actions/workflows/emscripten.yml/badge.svg)](https://github.com/avadae/emscripten/actions)
[![GitHub Release](https://img.shields.io/github/v/release/avadae/minigin?logo=github&sort=semver)](https://github.com/avadae/minigin/releases/latest)

---

## About the Project

This project is a game engine built from the ground up (on top of a bare-bones SDL3 scaffold called **Minigin**) over the course of a semester. The goal is to implement various game programming patterns discussed in class — using [Game Programming Patterns](https://gameprogrammingpatterns.com/) by Robert Nystrom as reading material — and end up with an engine capable of running a fully playable game.

The game I was assigned to recreate is **Q\*bert** (1982).

---

## About Q\*bert

Q\*bert is a classic isometric arcade game originally developed by Gottlieb and released in arcades in 1982. The player controls Q\*bert, an orange creature with a large nose, who hops around on a pyramid of cubes viewed from an isometric perspective.

![Q*bert arcade game](https://upload.wikimedia.org/wikipedia/en/5/5e/Qbert.png)

### How it plays

- The pyramid consists of 28 cubes arranged in a triangular grid, with 7 cubes on the bottom row tapering up to 1 at the top.
- Q\*bert starts at the top of the pyramid and must hop down, landing on each cube to change its color. Once every cube has been changed to the target color, the level is complete.
- On later levels, cubes may need to be jumped on multiple times to reach the target color, adding complexity.
- Several enemies roam the pyramid and will kill Q\*bert on contact, including:
  - **Coily** — a snake that hatches from a ball and actively chases Q\*bert.
  - **Ugg and Wrong-Way** — creatures that travel along the sides of the cubes.
  - **Slick and Sam** — enemies that revert cube colors back, undoing your progress.
- Colored discs float on either side of the pyramid. If Q\*bert jumps off the edge onto one of these discs, he rides it back to the top — and if Coily is in pursuit, Coily will fall off the edge trying to follow.
- The game features a scoring system rewarding cube completions, catching enemies with discs, and completing levels quickly.

Q\*bert was notable for its use of synthesized speech and gibberish exclamations (represented as "@!#?@!" in a speech bubble) whenever Q\*bert falls off the pyramid — a design that became iconic.

---

## Engine Features

Built on top of the [Minigin](https://github.com/avadae/minigin) start project, this engine *currently* implements the following systems:

- **Game Loop** — fixed timestep update with delta-time rendering
- **Scene Management** — scenes with GameObjects and Components
- **Component System** — attach/detach components to GameObjects, parent-child transforms
- **Input System** — keyboard and Xbox controller support (up to 4 controllers) via the **Command pattern**
  - XInput on Windows, SDL Gamepad API for Emscripten/web builds (Pimpl pattern used to hide platform details)
- **Resource Management** — texture and font loading
- **Rendering** — SDL3-based 2D renderer with texture and text support
- **ImGui integration** — for in-engine debugging and tooling

More systems will be added throughout the semester as the course progresses.

---

## Building the Project

### Windows

Either open the root folder in **Visual Studio 2022+** (recognized as a CMake project automatically), or:

```
cmake -B build
cmake --build build
```

### Emscripten (Web)

#### Windows (using [Chocolatey](https://chocolatey.org/))

```
choco install -y cmake emscripten ninja python
```

Then in the root folder:

```
mkdir build_web
cd build_web
emcmake cmake ..
emmake ninja
```

Start a local server to view it:

```
python -m http.server
```

Then browse to http://localhost:8000.

#### macOS (using [Homebrew](https://brew.sh/))

```
brew install cmake emscripten python
```

```
mkdir build_web
cd build_web
emcmake cmake .. -DCMAKE_OSX_ARCHITECTURES=""
emmake make
```

```
python3 -m http.server
```

Then browse to http://localhost:8000.

---

## GitHub Actions

This project uses two GitHub Actions workflows:

- **CMake workflow** — builds the project in Debug and Release for Windows.
- **Emscripten workflow** — builds the web version and publishes it as a GitHub Page at:
  `https://<username>.github.io/<repository>/`

You can embed the web build in any webpage with:

```html
<iframe style="position: absolute; top: 0px; left: 0px; width: 1024px; height: 576px;"
  src="https://<username>.github.io/<repository>/" loading="lazy"></iframe>
```

---

## Course

**Programming 4** — [DAE, Howest University of Applied Sciences](https://www.digitalartsandentertainment.be/)

Taught using *Game Programming Patterns* by Robert Nystrom — available free online at [gameprogrammingpatterns.com](https://gameprogrammingpatterns.com/).