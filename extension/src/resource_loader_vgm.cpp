#include <algorithm>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "resource_loader_vgm.h"
#include "audiostream_vgm.h"

using namespace godot;

// Forward declarations (because C++)
PackedByteArray load_file(const String &path);
String decode_godot_error(int error);

// Must be defined before the call (in the constructor) to avoid confusing the compiler
Dictionary getSupportedExtensions() {
    Dictionary result;
    Variant dummy = Variant(0);
    result["ay"] = dummy;
    result["gbs"] = dummy;
    result["gym"] = dummy;
    result["hes"] = dummy;
    result["kss"] = dummy;
    result["nsf"] = dummy;
    result["nsfe"] = dummy;
    result["sap"] = dummy;
    result["spc"] = dummy;
    result["rsn"] = dummy;
    result["vgm"] = dummy;
    result["vgz"] = dummy;
    return result;
}

ResourceFormatLoaderVGM::ResourceFormatLoaderVGM()
    : RESOURCE_NAME("AudioStreamVGM"), SUPPORTED_EXTENSIONS(getSupportedExtensions()) {
}

PackedStringArray ResourceFormatLoaderVGM::_get_recognized_extensions() const {
    return PackedStringArray(SUPPORTED_EXTENSIONS.keys());
    //ERR_FAIL_V_MSG(PackedStringArray(), "Not implemented");
}

bool ResourceFormatLoaderVGM::_handles_type(const StringName &type) const {
    // Second alternative is required when reloading a scene referencing an AudioStreamVGM
    // (see ResourceFormatLoader::get_recognized_extensions_for_type)
    return type == StringName(RESOURCE_NAME) || type == StringName("AudioStream");
    //ERR_FAIL_V_MSG(false, "Not implemented");
}

String ResourceFormatLoaderVGM::_get_resource_type(const String &path) const {
    if (SUPPORTED_EXTENSIONS.has(path.get_extension())) {
        //UtilityFunctions::print("This is a vgm: ", path);
        return RESOURCE_NAME;
    }

    return "";
}

bool ResourceFormatLoaderVGM::_exists(const String &path) const {
    return FileAccess::file_exists(path);
    //ERR_FAIL_V_MSG(false, "Not implemented");
}

Variant ResourceFormatLoaderVGM::_load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const {
    Ref<AudioStreamVGM> stream;
    stream.instantiate();
    stream->set_data(load_file(path));
    return stream;
    //ERR_FAIL_V_MSG(0, "Not implemented");
}

// Load file at the provided path, check for any error and return its contents as a PackedByteArray
PackedByteArray load_file(const String &path) {
    PackedByteArray result = FileAccess::get_file_as_bytes(path);
    ERR_FAIL_COND_V_MSG(result.is_empty(), result, String("Error while opening file {0}: {1}").format(Array::make(path, decode_godot_error(FileAccess::get_open_error()))));
    return result;
}

// Decode the Error godot enum as a human-readable String
String decode_godot_error(int error) {
    // See https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#enum-globalscope-error
    switch (error) {
        case 0:
            return "OK (no error)";
        case 7:
            return "file not found";
        case 48:
            return "printer on fire";
        default:
            // TODO If we get there, an entry is missing above
            return String("unknown ({0})").format(Array::make(error));
    }
}

void ResourceFormatLoaderVGM::_bind_methods() {
    // Required by GDCLASS macro
}