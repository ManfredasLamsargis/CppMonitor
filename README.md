# Build project
After cloning project go to the project's root directory and manually create `build` directory.

## Linux

To build a project run build script (requires CMake).
```bash
./scripts/linux/build.sh
```

Binaries are in the build directory:

```bash
./build/Monitor
```

Examples are in `build/example` directory.

## Windows

Go to script directory:

```cmd
cd scripts\win
```

Run the script:

```cmd
build.cmd
```
Open `Monitor.sln` file and build Monitor project.