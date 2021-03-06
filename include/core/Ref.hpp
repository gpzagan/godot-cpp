#ifndef REF_H
#define REF_H

#include "Variant.hpp"
#include "GodotGlobal.hpp"
#include "../Reference.hpp"

namespace godot {

// Replicates Godot's Ref<T> behavior
// Rewritten from f5234e70be7dec4930c2d5a0e829ff480d044b1d.
template <class T>
class Ref {

	T *reference = NULL;

	void ref(const Ref &p_from) {

		if (p_from.reference == reference)
			return;

		unref();

		reference = p_from.reference;
		if (reference)
			reference->reference();
	}

	void ref_pointer(T *p_ref) {

		ERR_FAIL_COND(!p_ref);

		if (p_ref->init_ref())
			reference = p_ref;
	}

public:
	inline bool operator<(const Ref<T> &p_r) const {

		return reference < p_r.reference;
	}
	inline bool operator==(const Ref<T> &p_r) const {

		return reference == p_r.reference;
	}
	inline bool operator!=(const Ref<T> &p_r) const {

		return reference != p_r.reference;
	}

	inline T *operator->() {

		return reference;
	}

	inline T *operator*() {

		return reference;
	}

	inline const T *operator->() const {

		return reference;
	}

	inline const T *ptr() const {

		return reference;
	}
	inline T *ptr() {

		return reference;
	}

	inline const T *operator*() const {

		return reference;
	}

	operator Variant() const {
		// Note: the C API handles the cases where the object is a Reference,
		// so the Variant will be correctly constructed with a RefPtr engine-side
		return Variant((Object*)reference);
	}

	void operator=(const Ref &p_from) {

		ref(p_from);
	}

	template <class T_Other>
	void operator=(const Ref<T_Other> &p_from) {

		// TODO We need a safe cast
		Reference *refb = const_cast<Reference *>(static_cast<const Reference *>(p_from.ptr()));
		if (!refb) {
			unref();
			return;
		}
		Ref r;
		//r.reference = Object::cast_to<T>(refb);
		r.reference = (T*)refb;
		ref(r);
		r.reference = NULL;
	}

	void operator=(const Variant &p_variant) {

		// TODO We need a safe cast
		Reference *refb = (Reference *) (Object *) p_variant;
		if (!refb) {
			unref();
			return;
		}
		Ref r;
		// TODO We need a safe cast
		//r.reference = Object::cast_to<T>(refb);
		r.reference = (T *)refb;
		ref(r);
		r.reference = NULL;
	}

	Ref(const Ref &p_from) {

		reference = NULL;
		ref(p_from);
	}

	template <class T_Other>
	Ref(const Ref<T_Other> &p_from) {

		reference = NULL;
		// TODO We need a safe cast
		Reference *refb = const_cast<Reference *>(static_cast<const Reference *>(p_from.ptr()));
		if (!refb) {
			unref();
			return;
		}
		Ref r;
		// TODO We need a safe cast
		//r.reference = Object::cast_to<T>(refb);
		r.reference = (T *)refb;
		ref(r);
		r.reference = NULL;
	}

	Ref(T *p_reference) {

		if (p_reference)
			ref_pointer(p_reference);
		else
			reference = NULL;
	}

	Ref(const Variant &p_variant) {

		reference = NULL;
		// TODO We need a safe cast
		Reference *refb = (Reference *) (Object *) p_variant;
		if (!refb) {
			unref();
			return;
		}
		Ref r;
		// TODO We need a safe cast
		//r.reference = Object::cast_to<T>(refb);
		r.reference = (T *)refb;
		ref(r);
		r.reference = NULL;
	}

	inline bool is_valid() const { return reference != NULL; }
	inline bool is_null() const { return reference == NULL; }

	void unref() {
		//TODO this should be moved to mutexes, since this engine does not really
		// do a lot of referencing on references and stuff
		// mutexes will avoid more crashes?

		if (reference && reference->unreference()) {

			//memdelete(reference);
			delete reference;
		}
		reference = NULL;
	}

	void instance() {
		//ref(memnew(T));
		ref(new T);
	}

	Ref() {

		reference = NULL;
	}

	~Ref() {

		unref();
	}

	// Used exclusively in the bindings to recreate the Ref Godot encapsulates in return values,
	// without adding to the refcount.
	inline static Ref<T> __internal_constructor(Object *obj)
	{
		Ref<T> r;
		r.reference = (T*)obj;
		return r;
	}
};

}

#endif
