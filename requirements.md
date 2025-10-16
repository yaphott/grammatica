# C API Design: Style and Methodology Guide

This guide describes architectural patterns, conventions, and best practices for designing robust C APIs, particularly those wrapping higher-level implementations or providing low-level library interfaces.

---

## Core Design Principles

When designing a C API, follow these principles:

- **Stability**: Maintain API/ABI stability across versions
- **Simplicity**: Provide straightforward procedural interfaces
- **Thread Safety**: Support thread-safe operation through context isolation
- **Clear Ownership**: Explicit memory management with documented ownership semantics
- **Comprehensive Error Handling**: Use return codes and callbacks for error reporting

---

## Context Management

### Opaque Context Handles

Use opaque context handles to maintain state and enable thread-safe operation:

```c
// Public header - incomplete type
typedef struct Context_t *ContextHandle_t;

// Implementation file - full definition
typedef struct Context_t {
    void* internal_state;
    char msg_buffer[1024];
    ErrorHandler error_fn;
    void* error_data;
    NoticeHandler notice_fn;
    void* notice_data;
    int initialized;
} ContextInternal_t;
```

### Initialization Pattern

```c
// Thread-safe: explicit context
ContextHandle_t ctx = lib_init();
lib_set_error_handler(ctx, my_error_fn, user_data);
// ... use context ...
lib_finish(ctx);

// Legacy: global context (avoid in new APIs)
lib_global_init(error_fn, notice_fn);
// ... operations use global state ...
lib_global_finish();
```

**Key Rules:**

- One context per thread - never share contexts across threads
- Context carries all operation state
- First parameter of thread-safe functions is always the context
- Maintain an initialization flag to validate context state

---

## Memory Management

### Ownership Rules

Document and enforce clear ownership semantics:

#### 1. **Caller Owns Return Values**

```c
// Returns newly allocated object - caller must free
Object* obj = lib_create_object(ctx, params);
lib_destroy_object(ctx, obj);
```

#### 2. **Functions Take Ownership of Arguments**

```c
// Component ownership transfers to parent
Component* comp = lib_create_component(ctx);
Object* obj = lib_create_object_with_component(ctx, comp);
// comp now owned by obj - DON'T free comp separately
```

#### 3. **Borrowed References**

```c
// Returns internal reference - caller does NOT own
const Component* comp = lib_get_component(ctx, obj, idx);
// comp freed when obj is freed - DON'T destroy comp
```

#### 4. **Array Ownership**

```c
Object* items[3] = { ... };
Collection* coll = lib_create_collection(ctx, items, 3);
// Ownership of items[0-2] transfers to coll
// Caller retains ownership of the array itself
```

### Destruction Functions

Provide paired create/destroy functions:

```c
void lib_destroy_object(ContextHandle_t ctx, Object* obj);
void lib_destroy_component(ContextHandle_t ctx, Component* comp);
void lib_free_buffer(ContextHandle_t ctx, void* buffer);
```

### Special Cases

```c
// String returns: must be freed with library function
char* str = lib_to_string(ctx, obj);
lib_free_buffer(ctx, str);  // NOT free()!

// Cloning: caller owns the copy
const Component* original = lib_get_component(ctx, obj, 0);  // borrowed
Component* copy = lib_clone_component(ctx, original);        // owned
lib_destroy_component(ctx, copy);

// Error cleanup: document whether args are freed on error
Object* obj = lib_create_complex(ctx, comp1, comp2);
if (!obj) {
    // comp1 and comp2 automatically freed - no double-free
}
```

---

## Error Handling

### Multi-Layered Error Reporting

#### 1. **Return Values**

```c
// Pointers: NULL on error
Object* obj = lib_create(ctx);
if (!obj) { /* error */ }

// Integers: -1 or 0 on error
int count = lib_get_count(ctx, obj);
if (count == -1) { /* error */ }

int success = lib_set_value(ctx, obj, value);
if (!success) { /* error */ }

// Predicates: special value (e.g., 2) indicates error
char result = lib_test_property(ctx, obj);
if (result == 2) { /* error */ }
else if (result == 1) { /* true */ }
else { /* false */ }
```

#### 2. **Message Handlers**

```c
typedef void (*ErrorHandler)(const char* message, void* userdata);

void my_error_handler(const char* msg, void* data) {
    fprintf(stderr, "Error: %s\n", msg);
}

lib_set_error_handler(ctx, my_error_handler, user_data);
lib_set_notice_handler(ctx, my_notice_handler, user_data);
```

#### 3. **Exception Wrapper Pattern**

Wrap C++ or error-prone code:

```c
template<typename F>
auto execute(ContextHandle_t ctx, ErrorValue err_val, F&& func) {
    if (!ctx) return err_val;
    try {
        return func();
    } catch (const std::exception& e) {
        ctx->error_fn(e.what(), ctx->error_data);
    } catch (...) {
        ctx->error_fn("Unknown error", ctx->error_data);
    }
    return err_val;
}
```

---

## Type System

### Opaque Types

```c
// Hide implementation details
typedef struct Object_t Object;
typedef struct Component_t Component;
```

### Type Differentiation

Use runtime type identification:

```c
enum ObjectType {
    TYPE_SIMPLE,
    TYPE_COMPLEX,
    TYPE_COLLECTION,
};

int type = lib_get_type(ctx, obj);
switch (type) {
    case TYPE_SIMPLE: /* handle */ break;
    case TYPE_COMPLEX: /* handle */ break;
    case TYPE_COLLECTION: /* handle */ break;
}

// Or string-based
char* type_name = lib_get_type_name(ctx, obj);
lib_free_buffer(ctx, type_name);
```

---

## API Patterns

### Reentrant vs Non-Reentrant

Provide thread-safe variants with `_r` suffix:

```c
// Non-reentrant (legacy, uses global state)
lib_global_init(error_fn, notice_fn);
Object* obj = lib_create_object(params);
lib_global_finish();

// Reentrant (recommended, explicit context)
ContextHandle_t ctx = lib_init_r();
Object* obj = lib_create_object_r(ctx, params);
lib_finish_r(ctx);
```

### Parameter Order

```c
RetType function_r(
    ContextHandle_t ctx,  // Always first for _r functions
    const Object* input,  // Input objects (const if not modified)
    ...,                  // Operation-specific parameters
    Type* output          // Output parameters last
);
```

### Const Correctness

```c
// Input not modified
Object* lib_transform(ContextHandle_t ctx, const Object* input);

// Object modified in-place
int lib_normalize(ContextHandle_t ctx, Object* obj);
```

---

## Naming Conventions

### Types

```c
typedef struct Handle_t *Handle_t;  // Opaque handles
typedef struct Object_t Object;     // Opaque objects
```

### Enums

```c
enum LibConstants {
    LIB_VALUE_ONE,
    LIB_VALUE_TWO,
};
```

### Functions

```c
lib_init()            // General functions
lib_object_create()   // Module-specific (object, component, etc.)
lib_object_destroy()
lib_is_valid()        // Predicates with "is_"
lib_get_property()    // Accessors with "get_"
lib_set_property()    // Mutators with "set_"
```

---

## Thread Safety

### Context Isolation

```c
void* thread_func(void* arg) {
    ContextHandle_t ctx = lib_init_r();
    // All operations isolated to this thread
    Object* obj = lib_create_r(ctx);
    lib_operation_r(ctx, obj);
    lib_destroy_r(ctx, obj);
    lib_finish_r(ctx);
    return NULL;
}
```

**Rules:**

- One context per thread
- No global state in `_r` functions
- Objects tied to creating context

---

## Summary

Key patterns for robust C API design:

1. **Opaque Types**: Hide implementation details
2. **Context Handles**: Thread-local state for safety
3. **Clear Ownership**: Explicit documentation
4. **Consistent Naming**: Predictable patterns
5. **Return Value Conventions**: Standard error signaling
6. **Callback Errors**: Flexible reporting without exceptions
7. **Reentrant Design**: Thread-safe `_r` variants
8. **Resource Management**: Paired create/destroy
9. **Const Correctness**: Clear mutation semantics
10. **Comprehensive Documentation**: Ownership and error behavior
