{
  description = "Star";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            cmake
            just
            ninja
            gdb
            libarchive
            qt6.qtbase
            qt6.qtdeclarative
            qt6.qttools
            pkg-config
          ];

          shellHook = ''
            export QT_QPA_PLATFORM_PLUGIN_PATH="${pkgs.qt6.qtbase}/lib/qt-6/plugins/platforms"
          '';
        };
      });
}
