#!/usr/bin/env python
import os
import sys

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# Explicitly name godot-cpp environment to avoid tampering with it below
godot_cpp_env = SConscript("godot-cpp/SConstruct")

# Extracted from game-music-emu/gme/CMakeLists.txt
GME_SRC = [ "Blip_Buffer.cpp",
"Blip_Buffer.h",
"Classic_Emu.cpp",
"Classic_Emu.h",
"Data_Reader.cpp",
"Data_Reader.h",
"Dual_Resampler.cpp",
"Dual_Resampler.h",
"Effects_Buffer.cpp",
"Effects_Buffer.h",
"Fir_Resampler.cpp",
"Fir_Resampler.h",
"gme.cpp",
"gme.h",
"gme_types.h",
"Gme_File.cpp",
"Gme_File.h",
"M3u_Playlist.cpp",
"M3u_Playlist.h",
"Multi_Buffer.cpp",
"Multi_Buffer.h",
"Music_Emu.cpp",
"Music_Emu.h",
"blargg_common.h",
"blargg_config.h",
"blargg_endian.h",
"blargg_source.h"]

gme_source = []
libxmp_source.append(["game-music-emu/src/{}".format(f) for f in GME_SRC])

# Use a dedicated environment for libxmp-lite build
# Not cloned from godot-cpp Environment since libxmp does not depend on it
libxmp_env = Environment(CPPPATH=["libxmp/include/"])
libxmp_env.Append(CCFLAGS=["-DLIBXMP_CORE_PLAYER", "-DLIBXMP_STATIC"])
libxmp_obj = [libxmp_env.SharedObject(f) for f in libxmp_source] # Make sure -fPIC is added under Linux
libxmp_lib = libxmp_env.StaticLibrary('libxmp-lite', libxmp_obj)

# GDExtension environment is cloned to avoid altering godot-cpp CPPPATH
env = godot_cpp_env.Clone()
env.Append(CPPPATH=["extension/src/", "libxmp/include/"])
env.Append(CCFLAGS=["-DLIBXMP_STATIC"]) # Avoids a LNK2019 under Windows
env.Append(LIBS=libxmp_lib)
sources = Glob("extension/src/*.cpp")

# Boilerplate
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "game/bin/libgdaudiostreammodule.{}.{}.framework/libgdaudiostreammodule.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "game/bin/libgdaudiostreammodule{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
