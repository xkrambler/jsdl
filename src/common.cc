#include "common.h"

// extracts a C string from a V8 Utf8Value
const char* C::cstr(const v8::String::Utf8Value& value) {
	return (*value?*value:"<cstr:fail>");
}

//v8::String::Utf8Value str(args[0]); title=C::cstr(str);
string C::v8string(const v8::Local<v8::Value>& value) {
	v8::String::Utf8Value str(value);
	return (*str?*str:"<v8string:fail>");
}

// convierte un puntero a un manejador
unsigned long C::pointerToHandle(void *pointer) {
	return (unsigned long) pointer;
}

// convierte un manejador a un puntero
void* C::handleToPointer(const v8::Local<v8::Value>& value) {
	return (void*) value->IntegerValue();
}
