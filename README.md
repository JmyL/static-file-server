# Static File Server

This project aims to build a static file server that serves files over HTTP.
The server will be designed to handle a large number of requests efficiently and can be easily deployed on various platforms.

See [PLAN.md](PLAN.md) for the detailed plan and progress tracking.

## Prerequisites

- direnv
- Install [nix](https://nixos.org/download/) and create `~/.config/nix/nix.conf` with `experimental-features = nix-command flakes` as it's contents.

All prerequisites will be installed by nix package manager.
You can configure this by editing `flake.nix` file.
Build type is set as RelWithDebInfo by default.
You can set the `BUILD_TYPE` environment variable manually in the shell, as shown below:

```{bash}
export BUILD_TYPE=Debug
```

## Configure

```{bash}
cmake -B $BUILD_TYPE -DCMAKE_BUILD_TYPE=$BUILD_TYPE
```