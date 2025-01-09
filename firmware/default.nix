{ lib, stdenv, stack-version, ... }:
stdenv.mkDerivation {
  pname = "intel-npu-linux-firmware";
  version = stack-version;

  src = with lib.fileset; toSource {
    root = ./.;
    fileset = ./bin;
  };

  meta = {
    homepage = "https://github.com/intel/linux-npu-driver";
    description = "Intel NPU (Neural Processing Unit) firmware";
    platforms = [ "x86_64-linux" ];
    license = lib.licenses.mit;
  };

  installPhase = ''
    mkdir -p $out/lib/firmware/intel/vpu
    cp -P bin/*.bin $out/lib/firmware/intel/vpu
  '';
}
