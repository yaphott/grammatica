# LibGEOS C API: Style and Methodology Guide

This document describes the architectural patterns, conventions, and methodologies used throughout the LibGEOS C API. It serves as a comprehensive reference for developers creating similar C libraries.

---

## Table of Contents

1. [Overview](#overview)
2. [Context Management](#context-management)
3. [Geometry Creation and Management](#geometry-creation-and-management)
4. [Memory Management](#memory-management)
5. [Type System and Differentiation](#type-system-and-differentiation)
6. [Error Handling](#error-handling)
7. [API Patterns and Conventions](#api-patterns-and-conventions)
8. [Thread Safety](#thread-safety)
9. [Naming Conventions](#naming-conventions)

---

## Overview

LibGEOS provides a stable C API wrapper around an underlying C++ geometry engine. The C API is designed with the following principles:

- **Stability**: The C API remains stable across versions while the C++ API may change
- **Simplicity**: Provides a straightforward procedural interface
- **Thread Safety**: Offers both thread-safe (reentrant) and non-thread-safe versions
- **Clear Ownership**: Explicit memory management with clear ownership semantics
- **Comprehensive Error Handling**: Uses return codes and callbacks for error reporting

---

## Context Management

### Context Handle Structure

The GEOS C API uses an opaque context handle (`GEOSContextHandle_t`) to maintain state and provide thread-safe operation.

```c
typedef struct GEOSContextHandle_HS *GEOSContextHandle_t;
```

### Context Internals

The actual context structure (defined in implementation files) contains:

```c
typedef struct GEOSContextHandle_HS {
    const GeometryFactory* geomFactory;      // C++ geometry factory
    char msgBuffer[1024];                     // Message formatting buffer
    GEOSMessageHandler noticeMessageOld;      // Legacy notice handler
    GEOSMessageHandler_r noticeMessageNew;    // New notice handler
    void* noticeData;                         // User data for notice handler
    GEOSMessageHandler errorMessageOld;       // Legacy error handler
    GEOSMessageHandler_r errorMessageNew;     // New error handler
    void* errorData;                          // User data for error handler
    GEOSContextInterruptCallback* interrupt_cb;
    void* interrupt_cb_data;
    uint8_t WKBOutputDims;
    int WKBByteOrder;
    int initialized;
    std::unique_ptr<Point> point2d;
} GEOSContextHandleInternal_t;
```

### Initialization and Cleanup

**Reentrant (Thread-Safe) API:**
```c
// Initialize a new context (thread-safe)
GEOSContextHandle_t handle = GEOS_init_r();

// Set message handlers
GEOSContext_setNoticeHandler_r(handle, my_notice_handler);
GEOSContext_setErrorHandler_r(handle, my_error_handler);

// Clean up when done
GEOS_finish_r(handle);
```

**Legacy (Non-Reentrant) API:**
```c
// Initialize global context
initGEOS(notice_function, error_function);

// Clean up global context
finishGEOS();
```

### Key Points

- **One context per thread**: Contexts are NOT thread-safe and must only be used by a single thread
- **Context carries state**: Each context maintains its own geometry factory, precision model, and error handlers
- **First parameter**: All `_r` functions take a context handle as their first parameter
- **Initialization check**: The context maintains an `initialized` flag that is checked before operations

---

## Geometry Creation and Management

### Geometry Types

Geometries are represented by the opaque type:

```c
typedef struct GEOSGeom_t GEOSGeometry;
```

In the implementation, this is mapped to the C++ `geos::geom::Geometry` class:

```c
#define GEOSGeometry geos::geom::Geometry
```

### Type Enumeration

Geometry types are differentiated using an enum:

```c
enum GEOSGeomTypes {
    GEOS_POINT,
    GEOS_LINESTRING,
    GEOS_LINEARRING,
    GEOS_POLYGON,
    GEOS_MULTIPOINT,
    GEOS_MULTILINESTRING,
    GEOS_MULTIPOLYGON,
    GEOS_GEOMETRYCOLLECTION,
    GEOS_CIRCULARSTRING,
    GEOS_COMPOUNDCURVE,
    GEOS_CURVEPOLYGON,
    GEOS_MULTICURVE,
    GEOS_MULTISURFACE,
};
```

### Creation Patterns

#### 1. Point Creation

```c
// Create from coordinate sequence
GEOSCoordSequence* coords = GEOSCoordSeq_create_r(handle, 1, 2);
GEOSCoordSeq_setXY_r(handle, coords, 0, 10.0, 20.0);
GEOSGeometry* point = GEOSGeom_createPoint_r(handle, coords);
// Ownership of coords transferred to point

// Create from XY coordinates
GEOSGeometry* point2 = GEOSGeom_createPointFromXY_r(handle, 10.0, 20.0);

// Create empty
GEOSGeometry* empty_point = GEOSGeom_createEmptyPoint_r(handle);
```

#### 2. LineString Creation

```c
// Create from coordinate sequence
GEOSCoordSequence* coords = GEOSCoordSeq_create_r(handle, 3, 2);
GEOSCoordSeq_setXY_r(handle, coords, 0, 0.0, 0.0);
GEOSCoordSeq_setXY_r(handle, coords, 1, 1.0, 1.0);
GEOSCoordSeq_setXY_r(handle, coords, 2, 2.0, 2.0);
GEOSGeometry* line = GEOSGeom_createLineString_r(handle, coords);
// Ownership of coords transferred to line

// Create empty
GEOSGeometry* empty_line = GEOSGeom_createEmptyLineString_r(handle);
```

#### 3. Polygon Creation

```c
// Create shell (must be a LinearRing)
GEOSCoordSequence* shell_coords = GEOSCoordSeq_create_r(handle, 5, 2);
// ... set coordinates to form a closed ring
GEOSGeometry* shell = GEOSGeom_createLinearRing_r(handle, shell_coords);

// Create holes (array of LinearRings)
GEOSGeometry* holes[1];
GEOSCoordSequence* hole_coords = GEOSCoordSeq_create_r(handle, 5, 2);
// ... set coordinates to form a closed ring
holes[0] = GEOSGeom_createLinearRing_r(handle, hole_coords);

// Create polygon (ownership of shell and holes transferred)
GEOSGeometry* polygon = GEOSGeom_createPolygon_r(handle, shell, holes, 1);

// Create empty
GEOSGeometry* empty_poly = GEOSGeom_createEmptyPolygon_r(handle);
```

#### 4. Collection Creation

```c
// Create geometries for collection
GEOSGeometry* geoms[3];
geoms[0] = GEOSGeom_createPointFromXY_r(handle, 0.0, 0.0);
geoms[1] = GEOSGeom_createPointFromXY_r(handle, 1.0, 1.0);
geoms[2] = GEOSGeom_createPointFromXY_r(handle, 2.0, 2.0);

// Create collection (ownership of geoms transferred)
GEOSGeometry* multipoint = GEOSGeom_createCollection_r(
    handle, GEOS_MULTIPOINT, geoms, 3
);

// Create empty collection
GEOSGeometry* empty_coll = GEOSGeom_createEmptyCollection_r(
    handle, GEOS_GEOMETRYCOLLECTION
);
```

### Coordinate Sequences

Coordinate sequences are the building blocks of geometries:

```c
typedef struct GEOSCoordSeq_t GEOSCoordSequence;
```

#### Creation Options

```c
// Legacy: Create with size and dimensions (2, 3, or 4)
GEOSCoordSequence* cs1 = GEOSCoordSeq_create_r(handle, size, 2);

// Modern: Create with explicit Z/M flags
GEOSCoordSequence* cs2 = GEOSCoordSeq_createWithDimensions_r(
    handle, size, hasZ, hasM
);

// Create from buffer
double buffer[] = {0.0, 0.0, 1.0, 1.0, 2.0, 2.0};
GEOSCoordSequence* cs3 = GEOSCoordSeq_copyFromBuffer_r(
    handle, buffer, 3, 0, 0  // 3 points, no Z, no M
);

// Create from separate arrays
double x[] = {0.0, 1.0, 2.0};
double y[] = {0.0, 1.0, 2.0};
GEOSCoordSequence* cs4 = GEOSCoordSeq_copyFromArrays_r(
    handle, x, y, NULL, NULL, 3
);
```

#### Modification

```c
// Set individual ordinates
GEOSCoordSeq_setX_r(handle, cs, idx, value);
GEOSCoordSeq_setY_r(handle, cs, idx, value);
GEOSCoordSeq_setZ_r(handle, cs, idx, value);
GEOSCoordSeq_setM_r(handle, cs, idx, value);

// Set coordinates
GEOSCoordSeq_setXY_r(handle, cs, idx, x, y);
GEOSCoordSeq_setXYZ_r(handle, cs, idx, x, y, z);

// Generic ordinate access
GEOSCoordSeq_setOrdinate_r(handle, cs, idx, dim, value);
```

#### Querying

```c
// Get ordinates
double x, y, z, m;
GEOSCoordSeq_getX_r(handle, cs, idx, &x);
GEOSCoordSeq_getY_r(handle, cs, idx, &y);
GEOSCoordSeq_getXY_r(handle, cs, idx, &x, &y);

// Get metadata
unsigned int size, dims;
GEOSCoordSeq_getSize_r(handle, cs, &size);
GEOSCoordSeq_getDimensions_r(handle, cs, &dims);
char hasZ = GEOSCoordSeq_hasZ_r(handle, cs);
char hasM = GEOSCoordSeq_hasM_r(handle, cs);
```

---

## Memory Management

### Ownership Model

LibGEOS follows clear ownership rules:

#### 1. **Caller Owns Return Values**

Most functions that return newly created geometries transfer ownership to the caller:

```c
// Caller must free the result
GEOSGeometry* result = GEOSBuffer_r(handle, geom, distance, segments);
GEOSGeom_destroy_r(handle, result);
```

Documentation explicitly states: *"Caller is responsible for freeing with GEOSGeom_destroy()"*

#### 2. **Functions Take Ownership of Arguments**

When creating geometries, ownership of components is transferred:

```c
GEOSCoordSequence* coords = GEOSCoordSeq_create_r(handle, 1, 2);
GEOSGeometry* point = GEOSGeom_createPoint_r(handle, coords);
// coords is now owned by point - do NOT free coords separately
```

Documentation states: *"ownership passes to the geometry"*

#### 3. **Borrowed References**

Some functions return pointers to internal structures that remain owned by the parent:

```c
// Returns internal reference - do NOT destroy
const GEOSGeometry* ring = GEOSGetInteriorRingN_r(handle, polygon, 0);
// ring is freed when polygon is freed
```

Documentation warns: *"it must NOT be destroyed directly"* and *"Owned by parent geometry, do not free"*

#### 4. **Array Ownership**

When passing arrays of geometries:

```c
GEOSGeometry* geoms[3] = { ... };
GEOSGeometry* collection = GEOSGeom_createCollection_r(
    handle, GEOS_MULTIPOINT, geoms, 3
);
// Ownership of geoms[0], geoms[1], geoms[2] transferred to collection
// The caller retains ownership of the array itself
```

Documentation states: *"The caller **retains ownership** of the containing array, but the ownership of the pointed-to objects is transferred"*

### Destruction Functions

```c
// Destroy geometry
void GEOSGeom_destroy_r(GEOSContextHandle_t handle, GEOSGeometry* g);

// Destroy coordinate sequence
void GEOSCoordSeq_destroy_r(GEOSContextHandle_t handle, GEOSCoordSequence* s);

// Destroy prepared geometry
void GEOSPreparedGeom_destroy_r(GEOSContextHandle_t handle, 
                                const GEOSPreparedGeometry* g);

// Free string/buffer results
void GEOSFree_r(GEOSContextHandle_t handle, void* buffer);
```

### Special Cases

#### 1. **String Returns**

Functions returning `char*` allocate memory that must be freed:

```c
char* wkt = GEOSGeomToWKT_r(handle, geom);
// Use wkt...
GEOSFree_r(handle, wkt);  // Not free()!
```

#### 2. **WKB/HEX Buffers**

Binary output requires freeing:

```c
size_t size;
unsigned char* wkb = GEOSGeomToWKB_buf_r(handle, geom, &size);
// Use wkb...
GEOSFree_r(handle, wkb);
```

#### 3. **Coordinate Sequence Cloning**

```c
const GEOSCoordSequence* original = GEOSGeom_getCoordSeq_r(handle, geom);
// original is borrowed - don't destroy

GEOSCoordSequence* copy = GEOSCoordSeq_clone_r(handle, original);
// copy is owned - must destroy
GEOSCoordSeq_destroy_r(handle, copy);
```

#### 4. **Error Cleanup**

Functions document cleanup behavior on error. For example, `GEOSGeom_createPolygon_r`:

```c
// "Contract for GEOSGeom_createPolygon is to take ownership of arguments
//  which implies freeing them on exception"

// On error, shell and holes are automatically freed
GEOSGeometry* poly = GEOSGeom_createPolygon_r(handle, shell, holes, nholes);
if (!poly) {
    // shell and holes already freed - don't double-free
}
```

---

## Type System and Differentiation

### Type Identification

Geometries are differentiated by type IDs obtained at runtime:

```c
// Get type ID
int type = GEOSGeomTypeId_r(handle, geom);

switch(type) {
    case GEOS_POINT:
        // Handle point
        break;
    case GEOS_LINESTRING:
        // Handle linestring
        break;
    case GEOS_POLYGON:
        // Handle polygon
        break;
    // ... etc
}
```

```c
// Get type as string
char* type_name = GEOSGeomType_r(handle, geom);
// Returns "Point", "LineString", "Polygon", etc.
GEOSFree_r(handle, type_name);
```

### Type-Specific Operations

Many operations require specific geometry types and return errors for invalid types:

```c
// Only valid for Polygon
int num_rings = GEOSGetNumInteriorRings_r(handle, polygon);
// Returns -1 on error (e.g., if geom is not a Polygon)

// Only valid for LineString
GEOSGeometry* point = GEOSGeomGetPointN_r(handle, linestring, index);
// Returns NULL on error
```

### Collection Handling

```c
// Get number of sub-geometries (1 for non-collections)
int num = GEOSGetNumGeometries_r(handle, geom);

// Get sub-geometry (borrowed reference)
for (int i = 0; i < num; i++) {
    const GEOSGeometry* sub = GEOSGetGeometryN_r(handle, geom, i);
    // Use sub (don't destroy it)
}
```

### Dimension Queries

```c
// Get planar dimensionality (0=point, 1=line, 2=polygon)
int dim = GEOSGeom_getDimensions_r(handle, geom);

// Get coordinate dimensionality (2=XY, 3=XYZ)
int coord_dim = GEOSGeom_getCoordinateDimension_r(handle, geom);

// Check for Z/M coordinates
char has_z = GEOSHasZ_r(handle, geom);
char has_m = GEOSHasM_r(handle, geom);
```

---

## Error Handling

### Error Reporting Mechanisms

GEOS uses a multi-layered error handling approach:

#### 1. **Return Values**

Functions indicate errors through their return values:

```c
// Pointer returns: NULL on error
GEOSGeometry* result = GEOSBuffer_r(handle, geom, dist, segs);
if (!result) {
    // Error occurred
}

// Integer returns: Specific error value
int num = GEOSGetNumGeometries_r(handle, geom);
if (num == -1) {
    // Error occurred
}

// Boolean/char returns: 2 indicates error
char result = GEOSIntersects_r(handle, g1, g2);
if (result == 2) {
    // Error occurred
} else if (result == 1) {
    // True
} else {
    // False (0)
}

// Success/failure: 0 for error, 1 for success
int success = GEOSCoordSeq_getX_r(handle, cs, idx, &x);
if (!success) {
    // Error occurred
}
```

#### 2. **Message Handlers**

Error and notice messages are delivered via callbacks:

```c
// Message handler signature
typedef void (*GEOSMessageHandler)(const char *fmt, ...);
typedef void (*GEOSMessageHandler_r)(const char *message, void *userdata);

// Set handlers
void my_error_handler(const char* message, void* data) {
    fprintf(stderr, "GEOS Error: %s\n", message);
}

GEOSContext_setErrorMessageHandler_r(handle, my_error_handler, user_data);
GEOSContext_setNoticeMessageHandler_r(handle, my_notice_handler, user_data);
```

#### 3. **Execute Wrapper Pattern**

Internally, all operations are wrapped in exception-handling code:

```c
// Internal pattern (simplified from source)
template<typename F>
auto execute(GEOSContextHandle_t handle, ErrorValue errval, F&& func) {
    if (handle == nullptr) {
        throw std::runtime_error("GEOS context handle is uninitialized");
    }

    GEOSContextHandleInternal_t* h = 
        reinterpret_cast<GEOSContextHandleInternal_t*>(handle);

    try {
        return func();
    } 
    catch (const std::exception& e) {
        h->ERROR_MESSAGE("%s", e.what());
    } 
    catch (...) {
        h->ERROR_MESSAGE("Unknown exception thrown");
    }

    return errval;
}
```

This pattern:
- Catches all C++ exceptions
- Converts them to C error codes/NULLs
- Calls the error handler with the message
- Returns the specified error value

### Error Handling Best Practices

```c
// Example: Proper error handling
GEOSContextHandle_t handle = GEOS_init_r();
if (!handle) {
    fprintf(stderr, "Failed to initialize GEOS\n");
    return -1;
}

// Set error handler
GEOSContext_setErrorMessageHandler_r(handle, error_handler, NULL);

// Perform operation
GEOSGeometry* result = GEOSBuffer_r(handle, geom, 10.0, 8);
if (!result) {
    // Error message already sent to error_handler
    GEOS_finish_r(handle);
    return -1;
}

// Success
GEOSGeom_destroy_r(handle, result);
GEOS_finish_r(handle);
```

### Error Details

Some functions provide detailed error information:

```c
char* reason = NULL;
GEOSGeometry* location = NULL;

char valid = GEOSisValidDetail_r(handle, geom, flags, &reason, &location);

if (!valid) {
    printf("Invalid geometry: %s\n", reason);
    // location contains the point of invalidity
    GEOSFree_r(handle, reason);
    GEOSGeom_destroy_r(handle, location);
}
```

---

## API Patterns and Conventions

### Reentrant vs Non-Reentrant

LibGEOS provides two parallel APIs:

#### Non-Reentrant (Legacy)
```c
// Uses global handle
initGEOS(notice, error);
GEOSGeometry* g = GEOSBuffer(geom, 10.0, 8);
finishGEOS();
```

#### Reentrant (Recommended)
```c
// Explicit context handle
GEOSContextHandle_t h = GEOS_init_r();
GEOSGeometry* g = GEOSBuffer_r(h, geom, 10.0, 8);
GEOS_finish_r(h);
```

**All reentrant functions end with `_r` suffix.**

### Function Naming

Functions follow consistent naming patterns:

```c
// Pattern: GEOS[Module][Operation][_r]

// Geometry operations
GEOSGeom_createPoint_r()
GEOSGeom_destroy_r()
GEOSGeom_clone_r()

// Coordinate sequence operations
GEOSCoordSeq_create_r()
GEOSCoordSeq_setX_r()
GEOSCoordSeq_getSize_r()

// Topology operations
GEOSIntersection_r()
GEOSBuffer_r()
GEOSUnion_r()

// Predicates
GEOSIntersects_r()
GEOSContains_r()
GEOSisValid_r()

// Input/Output
GEOSWKTReader_read_r()
GEOSWKTWriter_write_r()
```

### Parameter Order

Consistent parameter ordering:

```c
// Standard pattern:
RetType Function_r(
    GEOSContextHandle_t handle,  // Always first for _r functions
    const GEOSGeometry* input,   // Input geometry/geometries
    ... other params ...,         // Operation-specific parameters
    Type* output                  // Output parameters last
)

// Examples:
int GEOSArea_r(
    GEOSContextHandle_t handle,
    const GEOSGeometry* g,
    double* area                  // Output
);

char GEOSIntersects_r(
    GEOSContextHandle_t handle,
    const GEOSGeometry* g1,       // Input 1
    const GEOSGeometry* g2        // Input 2
);
```

### Const Correctness

Input geometries are marked `const`:

```c
// Input geometries are const (won't be modified)
GEOSGeometry* GEOSBuffer_r(
    GEOSContextHandle_t handle,
    const GEOSGeometry* g,        // const input
    double width,
    int quadsegs
);

// Mutating operations take non-const
int GEOSNormalize_r(
    GEOSContextHandle_t handle,
    GEOSGeometry* g               // Non-const (modified in-place)
);
```

### Input/Output (Serialization)

Consistent patterns for I/O:

```c
// WKT (Well-Known Text)
GEOSGeometry* GEOSGeomFromWKT_r(handle, "POINT(1 2)");
char* wkt = GEOSGeomToWKT_r(handle, geom);
GEOSFree_r(handle, wkt);

// WKB (Well-Known Binary)
size_t size;
unsigned char* wkb = GEOSGeomToWKB_buf_r(handle, geom, &size);
GEOSGeometry* geom = GEOSGeomFromWKB_buf_r(handle, wkb, size);
GEOSFree_r(handle, wkb);

// GeoJSON
char* json = GEOSGeoJSONWriter_writeGeometry_r(writer, geom, 8);
GEOSGeometry* geom = GEOSGeoJSONReader_readGeometry_r(reader, json);
```

### Parameter Objects

Complex operations use parameter objects:

```c
// Create parameter object
GEOSBufferParams* params = GEOSBufferParams_create_r(handle);

// Configure parameters
GEOSBufferParams_setEndCapStyle_r(handle, params, GEOSBUF_CAP_ROUND);
GEOSBufferParams_setJoinStyle_r(handle, params, GEOSBUF_JOIN_ROUND);
GEOSBufferParams_setQuadrantSegments_r(handle, params, 8);

// Use parameters
GEOSGeometry* result = GEOSBufferWithParams_r(handle, geom, params, 10.0);

// Clean up
GEOSBufferParams_destroy_r(handle, params);
GEOSGeom_destroy_r(handle, result);
```

---

## Thread Safety

### Context-Based Thread Safety

GEOS achieves thread safety through context isolation:

```c
// Each thread gets its own context
void* thread_func(void* arg) {
    GEOSContextHandle_t handle = GEOS_init_r();

    // All operations on this handle are isolated
    GEOSGeometry* geom = ...;
    GEOSGeometry* result = GEOSBuffer_r(handle, geom, 10.0, 8);

    GEOSGeom_destroy_r(handle, result);
    GEOS_finish_r(handle);
    return NULL;
}
```

**Key Rules:**
1. **One context per thread**: Never share a context across threads
2. **No global state**: The `_r` functions use no global state
3. **Geometries tied to context**: Geometries created in one context should only be used with that context's factory

### Legacy Non-Thread-Safe API

The non-`_r` functions use a global handle:

```c
// NOT thread-safe - uses global handle
static GEOSContextHandle_t handle = NULL;

void initGEOS(GEOSMessageHandler nf, GEOSMessageHandler ef) {
    if (!handle) {
        handle = initGEOS_r(nf, ef);
    }
}

GEOSGeometry* GEOSBuffer(const Geometry* g, double w, int q) {
    return GEOSBuffer_r(handle, g, w, q);  // Uses global handle
}
```

This is provided for backwards compatibility only.

### Interrupt Handling

GEOS supports operation interruption:

```c
// Register interrupt callback (per-context, thread-safe)
int my_interrupt_callback(void* userdata) {
    // Return non-zero to interrupt
    return should_cancel();
}

GEOSContext_setInterruptCallback_r(handle, my_interrupt_callback, user_data);

// The callback is checked at interruption points during long operations
```

---

## Naming Conventions

### Type Names

```c
// Opaque types use typedef to pointer to incomplete struct
typedef struct GEOSGeom_t GEOSGeometry;
typedef struct GEOSCoordSeq_t GEOSCoordSequence;
typedef struct GEOSPrepGeom_t GEOSPreparedGeometry;
typedef struct GEOSSTRtree_t GEOSSTRtree;
```

### Enum Names

```c
// Enums use GEOS prefix and ALL_CAPS values
enum GEOSGeomTypes {
    GEOS_POINT,
    GEOS_LINESTRING,
    // ...
};

enum GEOSBufCapStyles {
    GEOSBUF_CAP_ROUND,
    GEOSBUF_CAP_FLAT,
    GEOSBUF_CAP_SQUARE
};
```

### Function Names

```c
// Module prefixes
GEOS*           // General functions (initGEOS, finishGEOS)
GEOSGeom_*      // Geometry functions
GEOSCoordSeq_*  // Coordinate sequence functions
GEOSPrepared*   // Prepared geometry functions
GEOSSTRtree_*   // Spatial index functions
GEOSWKT*        // WKT I/O functions
GEOSWKB*        // WKB I/O functions

// Operation style
GEOSBuffer_r()           // Operation as verb
GEOSisValid_r()          // Predicate with "is" prefix
GEOSGetNumRings_r()      // Accessor with "Get" prefix
GEOSSetSRID_r()          // Mutator with "Set" prefix
```

### Parameter Names

Common parameter name patterns:

```c
handle        // Context handle
g, g1, g2     // Geometry parameters
cs, s         // Coordinate sequence
size          // Size parameter
idx           // Index parameter
val, value    // Value parameters
x, y, z, m    // Coordinate values
```

### Return Values

Consistent return value semantics:

```c
// Pointers
GEOSGeometry*  // NULL on error
char*          // NULL on error, must GEOSFree()

// Integers
int            // -1 on error (size/count functions)
               // 0 on error, 1 on success (boolean functions)

// Characters
char           // 2 on error, 1 for true, 0 for false (predicates)
```

---

## Summary

The LibGEOS C API demonstrates several key patterns for C library design:

1. **Opaque Types**: Hide implementation details behind typedef'd struct pointers
2. **Context Handles**: Maintain thread-local state for thread safety
3. **Clear Ownership**: Explicit documentation of who owns what
4. **Consistent Naming**: Predictable function and parameter names
5. **Return Value Conventions**: Standard error signaling mechanisms
6. **Callback-Based Errors**: Flexible error reporting without exceptions
7. **Reentrant Design**: Thread-safe `_r` variants alongside legacy API
8. **Resource Management**: Paired create/destroy functions
9. **Const Correctness**: Clear indication of mutating vs non-mutating operations
10. **Comprehensive Documentation**: Every function documents ownership and error behavior

These patterns create a stable, predictable, and maintainable C API suitable for use across many programming languages and environments.
