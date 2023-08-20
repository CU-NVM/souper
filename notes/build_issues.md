# Building Souper

The commands you need to build souper are all on the README, and if everything goes smoothly you won’t need the information in this document.

# Issues Encountered

## windows(WSL 2) with vscode

Make sure that sufficient memory space is allocated before trying to build in WSL. Try allocating more than 12GB for building the initial project as it requires to build the entire llvm-project, Z3 and Klee inside the third_party folder.
Make sure that correct versions of the tools are installed in the third_party folder and wait for completion of the build. You can check the versions that would be installed inside the build_deps.sh file.


## macOS (M1)

### Issue 1: Alive2

Souper uses an outdated version of Alive2 that isn’t compatible with M1. I needed to update Alive2 myself with the following changes:
build_deps.sh changes:
		line 32: alive_repo=https://github.com/AliveToolkit/alive2
		
		line 59: git clone $alive_repo $alivedir

lib/Infer/AliveDriver.cpp 
line 132: std::make_unique<IR::Assume>(*std::move(V),IR::Assume::Kind::AndNonPoison);

This change got souper to build, but it lost some functionality with alive2 (which seems to be optional anyways).

### Issue 2: zstd

First I got:  `ld: library not found for -lzstd.`
This problem came with a warning: ` ld: warning: ignoring file /Users/Liam/Desktop/Projects/research/souper/third_party/hiredis-install/lib/libhiredis.a, building for macOS-x86_64 but attempting to link with file built for macOS-arm64`

It was a problem where cmake was generating a make file with CXX flags set to x86.
I redownloaded cmake here: https://cmake.org/install/

I downloaded that and ran: `sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install`

After this, running build_deps.sh and rebuilding, I got a clang error for symbols not found in arm64, related to zstd. My fix for this isn’t well documented, but it seems like I replaced my old libzstd,dylib file with a new one.
Issue 3: other

Make sure your xcode command line tools, clang, and OS are up to date.


