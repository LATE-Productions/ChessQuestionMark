#include "extension_entrypoint.hpp"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "test.hpp"

void initialize_gdextension_types(godot::ModuleInitializationLevel level) {
  if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  GDREGISTER_CLASS(ExampleClass)
}

void uninitialize_gdextension_types(godot::ModuleInitializationLevel level) {
  if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C" {
GDExtensionBool GDE_EXPORT
gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                 GDExtensionClassLibraryPtr p_library,
                 GDExtensionInitialization* r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
                                                 r_initialization);
  init_obj.register_initializer(initialize_gdextension_types);
  init_obj.register_terminator(uninitialize_gdextension_types);
  init_obj.set_minimum_library_initialization_level(
      godot::MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}
