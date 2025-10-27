/**
 * @file arena.c
 * @brief Arena allocator for bulk temporary allocations
 *
 * Provides an arena (region) allocator that allocates memory in large blocks
 * and can free all allocations at once. This is ideal for temporary allocations
 * during rendering, simplification, and other operations that create many
 * short-lived objects.
 *
 * Benefits:
 * - Fast allocation (just bump a pointer)
 * - No individual frees needed
 * - Excellent cache locality
 * - Bulk deallocation in O(1)
 */

#include "grammatica_utils.h"
#include "grammatica_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * Default block size for arena allocations (64 KB).
 * Large enough to amortize malloc overhead, small enough to not waste memory.
 */
#define DEFAULT_BLOCK_SIZE (64 * 1024)

/**
 * Minimum allocation alignment (8 bytes for 64-bit pointers).
 * Ensures all allocations are properly aligned for any data type.
 */
#define ARENA_ALIGNMENT 8

/**
 * Represents a single memory block in the arena.
 */
typedef struct ArenaBlock {
    struct ArenaBlock* next;  /**< Next block in the chain */
    size_t size;              /**< Total size of this block */
    size_t used;              /**< Bytes used in this block */
    char data[];              /**< Flexible array member for block data */
} ArenaBlock;

/**
 * Arena allocator structure.
 */
struct Arena {
    GrammaticaContext* ctx;   /**< Associated context for error reporting */
    ArenaBlock* current;      /**< Current allocation block */
    ArenaBlock* blocks;       /**< Head of block chain */
    size_t block_size;        /**< Default size for new blocks */
    size_t total_allocated;   /**< Total bytes allocated across all blocks */
};

/**
 * Round up size to next multiple of alignment.
 */
static inline size_t align_size(size_t size) {
    return (size + ARENA_ALIGNMENT - 1) & ~(ARENA_ALIGNMENT - 1);
}

/**
 * Creates a new arena allocator.
 *
 * @param ctx Context for error reporting (can be NULL for standalone use)
 * @param block_size Size of each memory block (0 for default)
 * @return New arena, or NULL on allocation failure
 */
Arena* arena_create(GrammaticaContext* ctx, size_t block_size) {
    VALIDATE_CONTEXT_RET_NULL(ctx);
    
    if (block_size == 0) {
        block_size = DEFAULT_BLOCK_SIZE;
    }
    
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (!arena) {
        grammatica_report_error(ctx, "Failed to allocate arena structure");
        return NULL;
    }
    
    arena->ctx = ctx;
    arena->current = NULL;
    arena->blocks = NULL;
    arena->block_size = block_size;
    arena->total_allocated = 0;
    
    return arena;
}

/**
 * Allocates a new block for the arena.
 *
 * @param arena The arena
 * @param min_size Minimum size needed (may be larger than default block size)
 * @return New block, or NULL on allocation failure
 */
static ArenaBlock* arena_new_block(Arena* arena, size_t min_size) {
    // Ensure block is large enough for the requested size
    size_t block_size = arena->block_size;
    if (min_size > block_size) {
        block_size = min_size;
    }
    
    // Allocate block with flexible array member
    ArenaBlock* block = (ArenaBlock*)malloc(sizeof(ArenaBlock) + block_size);
    if (!block) {
        if (arena->ctx) {
            grammatica_report_error(arena->ctx, "Failed to allocate arena block");
        }
        return NULL;
    }
    
    block->next = NULL;
    block->size = block_size;
    block->used = 0;
    
    return block;
}

/**
 * Allocates memory from the arena.
 *
 * @param arena The arena
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 *
 * @note The returned memory is aligned to ARENA_ALIGNMENT bytes
 * @note Individual allocations cannot be freed; use arena_reset() or arena_destroy()
 */
void* arena_alloc(Arena* arena, size_t size) {
    if (!arena || size == 0) {
        return NULL;
    }
    
    // Align size for proper data alignment
    size = align_size(size);
    
    // Check if current block has enough space
    if (arena->current && (arena->current->used + size <= arena->current->size)) {
        void* ptr = arena->current->data + arena->current->used;
        arena->current->used += size;
        arena->total_allocated += size;
        return ptr;
    }
    
    // Need a new block
    ArenaBlock* new_block = arena_new_block(arena, size);
    if (!new_block) {
        return NULL;
    }
    
    // Link new block into chain
    new_block->next = arena->blocks;
    arena->blocks = new_block;
    arena->current = new_block;
    
    // Allocate from new block
    void* ptr = new_block->data;
    new_block->used = size;
    arena->total_allocated += size;
    
    return ptr;
}

/**
 * Allocates zero-initialized memory from the arena.
 *
 * @param arena The arena
 * @param count Number of elements
 * @param size Size of each element
 * @return Pointer to zero-initialized memory, or NULL on failure
 */
void* arena_calloc(Arena* arena, size_t count, size_t size) {
    if (!arena) {
        return NULL;
    }
    
    size_t total = count * size;
    if (total == 0) {
        return NULL;
    }
    
    // Check for overflow
    if (count > 0 && total / count != size) {
        if (arena->ctx) {
            grammatica_report_error(arena->ctx, "Arena calloc size overflow");
        }
        return NULL;
    }
    
    void* ptr = arena_alloc(arena, total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    
    return ptr;
}

/**
 * Duplicates a string in the arena.
 *
 * @param arena The arena
 * @param str String to duplicate (must be null-terminated)
 * @return Pointer to duplicated string, or NULL on failure
 */
char* arena_strdup(Arena* arena, const char* str) {
    if (!arena || !str) {
        return NULL;
    }
    
    size_t len = strlen(str) + 1;  // Include null terminator
    char* copy = (char*)arena_alloc(arena, len);
    if (copy) {
        memcpy(copy, str, len);
    }
    
    return copy;
}

/**
 * Duplicates n characters of a string in the arena.
 *
 * @param arena The arena
 * @param str String to duplicate
 * @param n Maximum number of characters to copy
 * @return Pointer to duplicated string (null-terminated), or NULL on failure
 */
char* arena_strndup(Arena* arena, const char* str, size_t n) {
    if (!arena || !str) {
        return NULL;
    }
    
    // Find actual length (may be less than n if null terminator found)
    size_t len = 0;
    while (len < n && str[len] != '\0') {
        len++;
    }
    
    char* copy = (char*)arena_alloc(arena, len + 1);
    if (copy) {
        memcpy(copy, str, len);
        copy[len] = '\0';
    }
    
    return copy;
}

/**
 * Resets the arena, freeing all allocations.
 *
 * @param arena The arena to reset
 *
 * @note This keeps the first block allocated for reuse but frees all others.
 *       Subsequent allocations will reuse the first block.
 */
void arena_reset(Arena* arena) {
    if (!arena) {
        return;
    }
    
    // Keep first block, free the rest
    if (arena->blocks) {
        ArenaBlock* first = arena->blocks;
        ArenaBlock* current = first->next;
        
        while (current) {
            ArenaBlock* next = current->next;
            free(current);
            current = next;
        }
        
        // Reset first block
        first->next = NULL;
        first->used = 0;
        arena->current = first;
        arena->blocks = first;
    }
    
    arena->total_allocated = 0;
}

/**
 * Gets the total bytes allocated by the arena across all blocks.
 *
 * @param arena The arena
 * @return Total bytes allocated, or 0 if arena is NULL
 */
size_t arena_total_allocated(const Arena* arena) {
    return arena ? arena->total_allocated : 0;
}

/**
 * Gets the number of blocks in the arena.
 *
 * @param arena The arena
 * @return Number of blocks, or 0 if arena is NULL
 */
size_t arena_block_count(const Arena* arena) {
    if (!arena) {
        return 0;
    }
    
    size_t count = 0;
    ArenaBlock* block = arena->blocks;
    while (block) {
        count++;
        block = block->next;
    }
    
    return count;
}

/**
 * Destroys the arena and frees all memory.
 *
 * @param arena The arena to destroy
 */
void arena_destroy(Arena* arena) {
    if (!arena) {
        return;
    }
    
    // Free all blocks
    ArenaBlock* current = arena->blocks;
    while (current) {
        ArenaBlock* next = current->next;
        free(current);
        current = next;
    }
    
    // Free arena structure
    free(arena);
}
