{
  description = "Intel® NPU driver";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs, ... } @ inputs:
  with builtins;
  let
    lib = nixpkgs.lib;
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
    extract-version = match "set\\(STACK_VERSION ([\\.0-9]+) .+\\)";
    lines = lib.splitString "\n" (readFile ./CMakeLists.txt);
    stack-version = elemAt (
      lib.findFirst isList null (map extract-version lines)
    ) 0;
  in {
    firmware = pkgs.callPackage ./firmware/default.nix { inherit stack-version; };
  };
}
