{
  description = "dev shell with clang, Boost, etc.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { system = system; };
      in with pkgs; {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            clang
            ninja
            cmake
            ccache
            boost.dev
            cppcheck
            doxygen
            lcov
          ];
          shellHook = ''
            export CC=clang
            export CXX=clang++
            export BUILD_TYPE=RelWithDebInfo
          '';
          CMAKE_PREFIX_PATH = "${pkgs.boost.dev}";
          CPLUS_INCLUDE_PATH = "${pkgs.boost.dev}/include";
        };
      });
}