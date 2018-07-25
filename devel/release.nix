# NTPsec derivation for Nix (https://nixos.org). To hack NTPsec on
# NixOS, use `nix-shell -A build.x86_64-linux devel/release.nix`.
{
  ntpsecSrc ? { outPath = ../.; revCount = 1234; shortRev = "abcdef"; },
  officialRelease ? false
}:
let
  pkgs = import <nixpkgs> {};
  lib = pkgs.lib;
  derivationOptions = rec {
    version = lib.removeSuffix "\n" (builtins.readFile ../VERSION);
    versionSuffix =
      lib.optionalString (!officialRelease)
      "pre${builtins.toString ntpsecSrc.revCount}-${ntpsecSrc.shortRev}"
    ;
    enableParallelBuilding = true;
    doCheck = false; #XXX: set this to 'true' once we have a test suite
    configureFlags = "--enable-ipv6";
  };
in with derivationOptions; rec {
  tarball = pkgs.releaseTools.sourceTarball (derivationOptions // rec {
    name = "ntpsec-tarball";
    src = ntpsecSrc;
    buildPhases = [ "unpackPhase" "patchPhase" "distPhase" ];

    buildInputs = with pkgs; [ python27 ];

    distPhase = ''
      runHook preDist

      echo "dist flags: $distFlags ''${distFlagsArray[@]}"
      python waf dist $distFlags "''${distFlagsArray[@]}"

      if [ "$dontCopyDist" != 1 ]; then       
        mkdir -p "$out/tarballs"
        cp -pvd ''${tarballs:-*.tar.gz} $out/tarballs
      fi

      runHook postDist
    '';
  });

  build = lib.genAttrs [ "x86_64-linux" "i686-linux" "x86_64-darwin" ] (system:
    let pkgs = (import <nixpkgs> { inherit system; }); in
    with pkgs; with derivationOptions;
    releaseTools.nixBuild (derivationOptions // rec {
      name = "ntpsec";
      src = tarball;

      buildInputs = with pkgs; [ python27 bison openssl asciidoc ];

      configurePhase = ''
        runHook preConfigure

        if [ -z "$dontAddPrefix" ]; then
          configureFlags="''${prefixKey:---prefix=}$prefix $configureFlags"
        fi

        echo "configure flags: $configureFlags ''${configureFlagsArray[@]}"
        python waf configure $configureFlags "''${configureFlagsArray[@]}"

        runHook postConfigure
      '';

      buildPhase = ''
        runHook preBuild

        echo "build flags: $makeFlags ''${makeFlagsArray[@]} $buildFlags ''${buildFlagsArray[@]}"
        python waf build \
          ''${enableParallelBuilding:+-j$NIX_BUILD_CORES} \
          $makeFlags "''${makeFlagsArray[@]}" \
          $buildFlags "''${buildFlagsArray[@]}"

        runHook postBuild
      '';

      checkPhase = ''
        if [ -n "$doCheck" ]; then
          runHook preCheck

          echo "check flags: $makeFlags ''${makeFlagsArray[@]} ''${checkFlags:+-v} ''${checkFlagsArray[@]}"
          python waf check \
            ''${enableParallelBuilding:+-j$NIX_BUILD_CORES} \
            $makeFlags "''${makeFlagsArray[@]}" \
            ''${checkFlags:+-v} "''${checkFlagsArray[@]}"

          runHook postCheck
        fi
      '';

      installPhase = ''
        runHook preInstall

        mkdir -p "$prefix"

        echo "install flags: $makeFlags ''${makeFlagsArray[@]} ''${installFlags:+-v} ''${installFlagsArray[@]}"
        python waf install \
          $makeFlags "''${makeFlagsArray[@]}" \
          ''${installFlags:+-v} "''${installFlagsArray[@]}"

        runHook postInstall
      '';
    })
  );
}
