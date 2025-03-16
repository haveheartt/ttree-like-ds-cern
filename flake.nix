{
  description = "Development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        pythonEnv = pkgs.python311.withPackages (ps: with ps; [
          jupyterlab
          numpy
          pandas
          pyarrow
          matplotlib
          scikit-learn
        ]);

      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            cmake
            gnumake 
            root 
            arrow-cpp
            pythonEnv
          ];

          shellHook = ''
            export JUPYTER_CONFIG_DIR="$HOME/.jupyter"
            export JUPYTER_DATA_DIR="$HOME/.local/share/jupyter"
            mkdir -p "$JUPYTER_CONFIG_DIR"
            mkdir -p "$JUPYTER_DATA_DIR"
            echo "Welcome to the development environment!"
          '';
        };
      });
}
