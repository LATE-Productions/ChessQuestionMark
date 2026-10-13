#include "extension_entrypoint.hpp"

#include <gdextension_interface.h>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/godot.hpp>

#include "main.hpp"
#include "nodes/FrameSource.hpp"
#include "singletons/VideoSource.hpp"

namespace chess {
void initialize_gdextension_types(godot::ModuleInitializationLevel level) {
  if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  GDREGISTER_CLASS(Main);
  GDREGISTER_CLASS(VideoSource);
  GDREGISTER_CLASS(FrameSource);

#if 0
	
0  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  godot::Engine::get_singleton()->register_singleton("VideoSource",
                                                     memnew(VideoSource));
#endif
}

void uninitialize_gdextension_types(godot::ModuleInitializationLevel level) {
  if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

#if 0
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  godot::Object* singleton =
      godot::Engine::get_singleton()->get_singleton("VideoSource");

  godot::Engine::get_singleton()->unregister_singleton("VideoSource");
  if (singleton) memdelete(singleton);
#endif
}

extern "C" {
GDExtensionBool GDE_EXPORT gdextension_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization* r_initialization
) {
  godot::GDExtensionBinding::InitObject init_obj(
      p_get_proc_address, p_library, r_initialization
  );

  init_obj.register_initializer(initialize_gdextension_types);
  init_obj.register_terminator(uninitialize_gdextension_types);
  init_obj.set_minimum_library_initialization_level(
      godot::MODULE_INITIALIZATION_LEVEL_SCENE
  );

  return init_obj.init();
}
}
}  // namespace chess
