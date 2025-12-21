{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { nixpkgs, ... }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-darwin"
      ];

      forAllSystems =
        function:
        nixpkgs.lib.genAttrs supportedSystems (system: function (import nixpkgs { inherit system; }));
    in
    {
      devShells = forAllSystems (
        pkgs:
        let
          inherit (pkgs) lib;
        in
        {
          default = pkgs.mkShell {
            nativeBuildInputs = with pkgs; [
              cmake
              ninja
              llvmPackages.clang-tools # need clang-scan-deps
            ];

            buildInputs =
              let
                helperB = pkgs.writeShellScriptBin "B" ''
                  cmake --preset debug && cmake --build build/Debug
                '';

                debugTools = (
                  with pkgs;
                  if stdenv.isLinux then
                    [
                      gdb
                      cgdb
                    ]
                  else
                    [ lldb ]
                );
              in
              [
                helperB
              ]
              ++ debugTools;

            hardeningDisable = [ "fortify" ];

            shellHook = ''
              export PATH=$(pwd)/build/Debug:$PATH

              ${lib.optionalString pkgs.stdenv.cc.isClang ''
                export NIX_CFLAGS_COMPILE="-isystem ${lib.getDev pkgs.libcxx}/include/c++/v1 $NIX_CFLAGS_COMPILE"
              ''}
            '';
          };
        }
      );
    };
}
