{
  description = "Development environment & packaging for Intel linux-npu-driver";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in rec {
    # NOTE: To run it: $ nix run .#apps.npu-umd-test -- --config=validation/umd-test/configs/basic.yaml
    apps.npu-umd-test = {
      type = "app";
      program = "${self.packages.default}/bin/npu-umd-test";
    };

    # NOTE: To run it as root: $ sudo nix run .#apps.npu-kmd-test
    apps.npu-kmd-test = {
      type = "app";
      program = "${self.packages.default}/bin/npu-kmd-test";
    };

    checks.npu_shared_tests = self.packages.default; # NOTE: Runs unit tests on: $ nix build ".#checks.npu_shared_tests" --print-build-logs
    checks.ze_intel_npu_tests = pkgs.runCommand "npu-kmd-test" { buildInputs = [ self.packages.default ]; } ''
      ze_intel_npu_tests -m models/add_abc/add_abc.bin -b models/add_abc/add_abc.xml -c validation/umd-test/configs/basic.yaml
    '';
    checks.test-outputs = self.packages.default.passthru.tests.test-outputs;

    # NOTE: In future we can remove this derivation:
    packages.level-zero-npu-extensions = pkgs.stdenv.mkDerivation (finalAttrs: rec {
      name = "level-zero-npu-extensions";

      # NOTE: Please move the code in this repository below to "linux-npu-driver" repository
      src = pkgs.fetchFromGitHub {
        owner = "intel";
        repo = "level-zero-npu-extensions";
        rev = "8cf113bd4a4568f6555d81f316504d7ac3b82ee8"; # NOTE: Actual revision of the git submodule
        hash = "sha256-nPt74rP7yxvi7nmGs6GMORyCQmSveP5TFWLr7ub46vk=";
      };

      dontConfigure = true;
      dontBuild = true;

      postPatch = ''
        substituteInPlace ze_graph_ext.h --replace-fail \
        "#include \"ze_api.h\"" \
        "#include <level_zero/ze_api.h>"
      '';

      installPhase = ''
        mkdir -p $out/include/level_zero
        mkdir -p $out/lib/pkgconfig

        cp -r *.h $out/include/level_zero/

        cat > $out/lib/pkgconfig/level_zero_npu_extensions.pc <<EOF
        prefix=$out
        includedir=$out/include

        Name: level_zero_npu_extensions
        Description: Intel Level Zero NPU Extensions (headers only)
        Version: unstable-2025-08-19
        Cflags: -I$out/include
        Libs:
        EOF
      '';
    });

    # NOTE: In future we can remove this derivation:
    packages.npu-plugin-elf = pkgs.stdenv.mkDerivation (finalAttrs: rec {
      name = "npu_plugin_elf";

      # NOTE: This repo needs a flake.nix as well, then we can import it above on "inputs"
      # NOTE: Does this really need to be a separate repository?
      src = pkgs.fetchFromGitHub {
        owner = "openvinotoolkit";
        repo = "npu_plugin_elf";
        rev = "7e8651735be77a877d2bfa04c7355136836def0f"; # NOTE: Actual revision of the git submodule
        hash = "sha256-ABozoJLc1tvrpSPyV3AYZxA3SQB8eGYCKb8WhachDLg=";
      };

      dontConfigure = true;
      dontBuild = true;

      installPhase = ''
        mkdir -p $out
        cp -r $PWD/* $out/
      '';
    });

    packages.x86_64-linux.default = self.packages.default; # NOTE: Enables: $ nix build ".#"
    packages.default = pkgs.stdenv.mkDerivation (finalAttrs: rec {
      name = "intel-npu-driver";

      nativeBuildInputs = [ pkgs.cmake pkgs.gtest ];

      buildInputs = [
        pkgs.level-zero
        pkgs.yaml-cpp
        pkgs.gtest
        self.packages.level-zero-npu-extensions
        self.packages.npu-plugin-elf
      ];

      src = ./.;

      cmakeFlags = [
        "-DENABLE_NPU_COMPILER_BUILD=OFF" # TODO: Turn it on later
        "-DENABLE_NPU_PERFETTO_BUILD=OFF" # TODO: Turn it on later
        "-DENABLE_OPENVINO_PACKAGE=OFF" # TODO: Turn it on later
      ];

      # NOTE: nix builds need to be hermetic & reproducable so we can't fetch the third_party submodules, they need to be nix packaged
      postPatch = ''
        rm -rf third_party/level-zero
        rm -rf third_party/level-zero-npu-extensions
        rm third_party/cmake/level-zero.cmake
        rm third_party/cmake/FindLevelZero.cmake

        substituteInPlace third_party/CMakeLists.txt --replace-fail \
          "include(cmake/level-zero.cmake)" \
          ""
        substituteInPlace third_party/cmake/vpux_elf.cmake --replace-fail \
          "add_subdirectory(vpux_elf/vpux_elf)" \
          "add_subdirectory(${self.packages.npu-plugin-elf}/vpux_elf ${self.packages.npu-plugin-elf}/..)"

        substituteInPlace third_party/CMakeLists.txt --replace-fail \
          "include(cmake/level-zero-npu-extensions.cmake)" \
          ""
        substituteInPlace third_party/CMakeLists.txt --replace-fail \
          "add_subdirectory(googletest EXCLUDE_FROM_ALL)" \
          ""
        substituteInPlace third_party/CMakeLists.txt --replace-fail \
          "add_subdirectory(yaml-cpp EXCLUDE_FROM_ALL)" \
          ""

        substituteInPlace firmware/CMakeLists.txt --replace-fail \
          "DESTINATION /lib/firmware/updates/intel/vpu/" \
          "DESTINATION $out/lib/firmware/intel/vpu/"
      '';

      doCheck = true;

      checkPhase = ''
        ./bin/npu_shared_tests
      '';

      installPhase = ''
        cmake --install . --component level-zero-npu
        cmake --install . --component validation-npu
        cmake --install . --component fw-npu
        cp ./bin/ze_intel_npu_tests $out/bin/ze_intel_npu_tests
      '';

      passthru.tests = with pkgs; {
        test-outputs = pkgs.testers.nixosTest {
          name = "test-outputs";
          nodes.machine = {
            environment.systemPackages = [
              self.packages.default
            ];

            hardware.firmware = [ self.packages.default ];
            hardware.graphics.enable = true;
            hardware.graphics.extraPackages = [ 
              self.packages.default 
            ];
          };

          testScript = ''
            start_all()
            machine.succeed("which npu-umd-test")
            machine.succeed("which npu-kmd-test")
          '';
        };
      };
    });

    # NOTE: I can document how to install this driver on NixOS from this flake, if needed:
    nixosModules.default = { pkgs, lib, config, ... }: {
      options.hardware.cpu.intel.npu.enable = lib.mkEnableOption "Intel NPU driver";
      config = lib.mkIf config.hardware.cpu.intel.npu.enable {
        hardware.firmware = [ self.packages.default ];
        environment.systemPackages = [ self.packages.default ];
        hardware.graphics.enable = true;
        hardware.graphics.extraPackages = [ self.packages.default ];
      };
    };

    devShells.default = pkgs.mkShell {
      nativeBuildInputs = [
        pkgs.pkg-config
        self.packages.default.nativeBuildInputs
        self.packages.default.buildInputs
      ];

      doCheck = false; # Disables automatically running tests for `$ nix develop` and direnv

      shellHook = ''
        export ZDOTDIR=$(mktemp -d)
        cat > "$ZDOTDIR/.zshrc" << 'EOF'
          source ~/.zshrc # Source the original ~/.zshrc, required.

          function parse_git_branch {
            git branch --no-color 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\ ->\ \1/'
          }

          function display_jobs_count_if_needed {
            local job_count=$(jobs -s | wc -l | tr -d " ")

            if [ $job_count -gt 0 ]; then
              echo "%B%F{yellow}%j| ";
            fi
          }

          # NOTE: Custom prompt with a snowflake: signals we are in `$ nix develop` shell
          PROMPT="%F{blue}$(date +%H:%M:%S) $(display_jobs_count_if_needed)%B%F{green}%n %F{blue}%~%F{cyan} ❄%F{yellow}$(parse_git_branch) %f%{$reset_color%}"
        EOF

        if [ -z "$DIRENV_IN_ENVRC" ]; then # This makes `$ nix develop` universally working with direnv without infinite loop
          exec ${pkgs.zsh}/bin/zsh -i
        fi
      '';
    };
  };
}
