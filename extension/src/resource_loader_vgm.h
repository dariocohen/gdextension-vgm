#ifndef RESOURCE_LOADER_VGM
#define RESOURCE_LOADER_VGM

#include <godot_cpp/classes/resource_format_loader.hpp>

using namespace godot;

// For an example of what is required, see https://github.com/DeleteSystem32/godot-flac-extension
class ResourceFormatLoaderVGM : public ResourceFormatLoader {
    GDCLASS(ResourceFormatLoaderVGM, ResourceFormatLoader);

    private:
        // Those cannot be static due to https://github.com/godotengine/godot/issues/59688)
        const String RESOURCE_NAME;
        const Dictionary SUPPORTED_EXTENSIONS;

    public:
        ResourceFormatLoaderVGM();
        virtual Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
        virtual PackedStringArray _get_recognized_extensions() const override;
        virtual bool _handles_type(const StringName &type) const override;
        virtual String _get_resource_type(const String &path) const override;
        virtual bool _exists(const String &path) const;

    protected:
        static void _bind_methods();    
};

#endif // RESOURCE_LOADER_VGM