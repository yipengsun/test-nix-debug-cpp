{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    flake-parts.url = "github:hercules-ci/flake-parts";
    flake-parts.inputs.nixpkgs-lib.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, flake-parts } @ inputs:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];

      perSystem =
        { system, inputs', pkgs', config, lib, ... }:
        {
          _module.args.pkgs' = import self.inputs.nixpkgs {
            inherit system;
          };

          devShells.default = pkgs'.mkShell {
            name = "test-nix-debug-cpp";

            # FIXME: workaround for https://github.com/NixOS/nixpkgs/issues/273875
            nativeBuildInputs = with pkgs'; [
              cmake
              ninja
              llvmPackages.clang-tools
            ];

            buildInputs =
              let
                helperB = pkgs'.writeShellScriptBin "B" ''
                  cmake --preset debug && cmake --build build/Debug
                '';
                helperD = pkgs'.writeShellScriptBin "D" ''
                  cmake --preset debug
                  ${pkgs'.compdb}/bin/compdb -p build/Debug/ list > compile_commands.json
                '';

                debugTools = (with pkgs'; if stdenv.isLinux then [ gdb cgdb ] else [ lldb ]);
              in
              [
                helperB
                helperD
              ] ++ debugTools;

            hardeningDisable = [ "fortify" ];

            shellHook = ''
              export PATH=$(pwd)/build/Debug:$PATH
            '';
          };
        };
    };
}
