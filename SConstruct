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
GME_SRC = ["gme/Ay_Apu.cpp",
"gme/Ay_Cpu.cpp",
"gme/Ay_Emu.cpp",
"gme/Blip_Buffer.cpp",
"gme/Classic_Emu.cpp",
"gme/Data_Reader.cpp",
"gme/Dual_Resampler.cpp",
"gme/Effects_Buffer.cpp",
"gme/Fir_Resampler.cpp",
"gme/Gb_Apu.cpp",
"gme/Gb_Cpu.cpp",
"gme/Gb_Oscs.cpp",
"gme/Gbs_Emu.cpp",
"gme/Gme_File.cpp",
"gme/Gym_Emu.cpp",
"gme/Hes_Apu.cpp",
"gme/Hes_Cpu.cpp",
"gme/Hes_Emu.cpp",
"gme/Kss_Cpu.cpp",
"gme/Kss_Emu.cpp",
"gme/Kss_Scc_Apu.cpp",
"gme/M3u_Playlist.cpp",
"gme/Multi_Buffer.cpp",
"gme/Music_Emu.cpp",
"gme/Nes_Apu.cpp",
"gme/Nes_Cpu.cpp",
"gme/Nes_Fme7_Apu.cpp",
"gme/Nes_Namco_Apu.cpp",
"gme/Nes_Oscs.cpp",
"gme/Nes_Vrc6_Apu.cpp",
"gme/Nes_Fds_Apu.cpp",
"gme/Nes_Vrc7_Apu.cpp",
"gme/Nsf_Emu.cpp",
"gme/Nsfe_Emu.cpp",
"gme/Sap_Apu.cpp",
"gme/Sap_Cpu.cpp",
"gme/Sap_Emu.cpp",
"gme/Sms_Apu.cpp",
"gme/Snes_Spc.cpp",
"gme/Spc_Cpu.cpp",
"gme/Spc_Dsp.cpp",
"gme/Spc_Emu.cpp",
"gme/Spc_Filter.cpp",
"gme/Vgm_Emu.cpp",
"gme/Vgm_Emu_Impl.cpp",
"gme/Ym2413_Emu.cpp",
"gme/Ym2612_Nuked.cpp",
"gme/Ym2612_GENS.cpp",
"gme/Ym2612_MAME.cpp",
"gme/ext/emu2413.c",
"gme/ext/panning.c",
"gme/gme.cpp"]

gme_source = []
gme_source.append(["game-music-emu/{}".format(f) for f in GME_SRC])

# Use a dedicated environment for gme build
# Not cloned from godot-cpp Environment since gme does not depend on it
gme_env = Environment(CPPPATH=["game-music-emu/gme/"])
gme_env.Append(CPP_FEATURES=["exceptions"])
gme_env.Append(CCFLAGS=["-DBLARGG_LITTLE_ENDIAN=1", "-DBLARGG_BUILD_DLL", "-DLIBGME_VISIBILITY", "-DVGM_YM2612_NUKED"])
gme_env.Append(CXXFLAGS=["-std=c++11", "-fvisibility-inlines-hidden"])

gme_obj = [gme_env.SharedObject(f) for f in gme_source] # Make sure -fPIC is added under Linux
gme_lib = gme_env.StaticLibrary('gme', gme_obj)

# GDExtension environment is cloned to avoid altering godot-cpp CPPPATH
env = godot_cpp_env.Clone()
env.Append(CPPPATH=["extension/src/", "game-music-emu/gme/"])
env.Append(LIBS=gme_lib)
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
