#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "audiostream_vgm.h"
#include "resource_loader_vgm.h"

using namespace godot;

static Ref<ResourceFormatLoaderVGM> vgm_loader;

void initialize_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<AudioStreamVGM>();
	ClassDB::register_class<AudioStreamPlaybackVGM>();

	ClassDB::register_class<ResourceFormatLoaderVGM>();

	vgm_loader.instantiate();

	ResourceLoader::get_singleton()->add_resource_format_loader(vgm_loader);
}

void uninitialize_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C"
{

	// Initialization.

	GDExtensionBool GDE_EXPORT library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(initialize_types);
		init_obj.register_terminator(uninitialize_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}