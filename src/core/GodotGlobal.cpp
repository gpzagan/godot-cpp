#include "GodotGlobal.hpp"

#include "String.hpp"

static GDCALLINGCONV void *wrapper_create(void *data, godot_object *instance)
{
	void *wrapper_memory = godot::api->godot_alloc(sizeof(instance));

	if (!wrapper_memory)
		return NULL;

	godot_object **wrapper = (godot_object **) wrapper_memory;

	*wrapper = instance;

	return wrapper;
}

static GDCALLINGCONV void wrapper_destroy(void *data, void *wrapper)
{
	if (wrapper)
		godot::api->godot_free(wrapper);
}

namespace godot {

void *_RegisterState::nativescript_handle;
int _RegisterState::language_index;
const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;
const godot_gdnative_ext_nativescript_1_1_api_struct *nativescript_1_1_api = NULL;

const void *gdnlib = NULL;

void Godot::print(const String& message)
{
	godot::api->godot_print((godot_string *) &message);
}

void Godot::print_warning(const String& description, const String& function, const String& file, int line)
{
	int len;

	char * c_desc = description.alloc_c_string();
	char * c_func = function.alloc_c_string();
	char * c_file = file.alloc_c_string();

	if (c_desc != NULL && c_func !=NULL && c_file != NULL) {
		godot::api->godot_print_warning(c_desc, c_func, c_file, line);
	};

	if (c_desc != NULL) godot::api->godot_free(c_desc);
	if (c_func != NULL) godot::api->godot_free(c_func);
	if (c_file != NULL) godot::api->godot_free(c_file);
}

void Godot::print_error(const String& description, const String& function, const String& file, int line)
{
	int len;

	char * c_desc = description.alloc_c_string();
	char * c_func = function.alloc_c_string();
	char * c_file = file.alloc_c_string();

	if (c_desc != NULL && c_func !=NULL && c_file != NULL) {
		godot::api->godot_print_error(c_desc, c_func, c_file, line);
	};

	if (c_desc != NULL) godot::api->godot_free(c_desc);
	if (c_func != NULL) godot::api->godot_free(c_func);
	if (c_file != NULL) godot::api->godot_free(c_file);
}

void Godot::gdnative_init(godot_gdnative_init_options *options)
{
	godot::api = options->api_struct;
	godot::gdnlib = options->gd_native_library;

	// now find our extensions
	for (int i = 0; i < godot::api->num_extensions; i++) {
		switch (godot::api->extensions[i]->type) {
			case GDNATIVE_EXT_NATIVESCRIPT: {
				godot::nativescript_api = (const godot_gdnative_ext_nativescript_api_struct *)godot::api->extensions[i];

				const godot_gdnative_api_struct *extension = godot::nativescript_api->next;

				while (extension) {
					if (extension->version.major == 1 && extension->version.minor == 1) {
						godot::nativescript_1_1_api = (const godot_gdnative_ext_nativescript_1_1_api_struct *) extension;
					}

					extension = extension->next;
				}
			}; break;
			default: break;
		};
	};
}

void Godot::gdnative_terminate(godot_gdnative_terminate_options *options)
{
	// reserved for future use.
}

void Godot::nativescript_init(void *handle)
{
	godot::_RegisterState::nativescript_handle = handle;

	godot_instance_binding_functions binding_funcs = {};
	binding_funcs.alloc_instance_binding_data = wrapper_create;
	binding_funcs.free_instance_binding_data = wrapper_destroy;

	godot::_RegisterState::language_index = godot::nativescript_1_1_api->godot_nativescript_register_instance_binding_data_functions(binding_funcs);

}

void Godot::nativescript_terminate(void *handle)
{
	godot::nativescript_1_1_api->godot_nativescript_unregister_instance_binding_data_functions(godot::_RegisterState::language_index);
}

}
