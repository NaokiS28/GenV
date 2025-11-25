/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gifn.cpp - Created on 12-09-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gifn.h"

#include <cstddef>
#include <string.h> // memcpy, memset
#include <stdlib.h>
#include <stddef.h>

#include "../vendor_conf.h"

// -----------------------------
// Allocators
// -----------------------------
#ifdef GIFN_COMPILE_ALLOCATORS
static void *gifn_malloc(size_t size)
{
#ifdef GIFN_MAX_ALLOC
    if (size > GIFN_MAX_ALLOC) return 0;
#endif
    return malloc(size);
}

static void gifn_free(void *ptr)
{
    free(ptr);
}
#else  /*GIFN_COMPILE_ALLOCATORS*/
/* TODO: support giving additional void* payload to the custom allocators */
void *gifn_malloc(size_t size);
void gifn_free(void *ptr);
#endif /*GIFN_COMPILE_ALLOCATORS*/

// -----------------------------
// Small helpers / arena
// -----------------------------
typedef struct
{
    const uint8_t *data;
    const uint8_t *end;
    size_t pos;
} GifData;

void gifn_gifdata_init(GifData *gif, const uint8_t *p, size_t n)
{
    gif->data = p;
    gif->end  = p + n;
    gif->pos  = 0;
}

bool gifn_gifdata_has(GifData *gif, size_t n)
{
    return (size_t)(gif->end - (gif->data + gif->pos)) >= n;
}

bool gifn_gifdata_read_u8(GifData *gif, uint8_t *v)
{
    if (!gifn_gifdata_has(gif, 1)) return false;
    *v = gif->data[gif->pos++];
    return true;
}

bool gifn_gifdata_read_u16le(GifData *gif, uint16_t *v)
{
    if (!gifn_gifdata_has(gif, 2)) return false;
    *v = (uint16_t)gif->data[gif->pos] | ((uint16_t)gif->data[gif->pos + 1] << 8);
    gif->pos += 2;
    return true;
}

bool gifn_gifdata_read(GifData *gif, void *dst, size_t n)
{
    if (!gifn_gifdata_has(gif, n)) return false;
    memcpy(dst, gif->data + gif->pos, n);
    gif->pos += n;
    return true;
}

bool gifn_gifdata_seek(GifData *gif, size_t n)
{
    if (!gifn_gifdata_has(gif, n)) return false;
    gif->pos += n;
    return true;
}

bool gifn_gifdata_test_str(GifData *gif, const char *s)
{
    size_t len = 0;
    while (s[len])
        ++len;
    if (!gifn_gifdata_has(gif, len)) return false;
    bool ok = (memcmp(gif->data + gif->pos, s, len) == 0);
    gif->pos += len;
    return ok;
}

uint8_t gifn_gifdata_at_rel(GifData *gif, size_t rel)
{
    if (!gifn_gifdata_has(gif, rel + 1)) return 0;
    return gif->data[gif->pos + rel];
}

bool gifn_gifdata_eof(GifData *gif)
{
    return gif->pos >= (size_t)(gif->end - gif->data);
}

// Internal only in gifn.cpp
typedef struct
{
    void *next;
    size_t len;
    // flexible payload follows
} ArenaBlk;

static void *arena_alloc(GIF_Object *g, size_t n, size_t align)
{
    if (align < 1) align = 1;
    const size_t header = sizeof(ArenaBlk);
    const size_t total  = header + (align - 1) + n;

    uint8_t *raw = (uint8_t *)gifn_malloc(total);
    if (!raw) return NULL;

    ArenaBlk *blk = (ArenaBlk *)raw;
    blk->next     = (ArenaBlk *)g->_arena; // push-front
    blk->len      = n;
    g->_arena     = blk; // head of list
    g->_arenaSize = 0;   // not used anymore

    uint8_t *payload  = raw + header;
    uintptr_t p       = (uintptr_t)payload;
    uintptr_t aligned = (p + (align - 1)) & ~(uintptr_t)(align - 1);
    return (void *)aligned;
}

static GIF_Color *arena_alloc_colors(GIF_Object *g, size_t count)
{
    return (GIF_Color *)arena_alloc(g, count * sizeof(GIF_Color), 1);
}
static uint8_t *arena_alloc_u8(GIF_Object *g, size_t count)
{
    return (uint8_t *)arena_alloc(g, count * sizeof(uint8_t), 1);
}

static GIF_ImageDescriptor *arena_alloc_frames(GIF_Object *g, size_t count)
{
    return (GIF_ImageDescriptor *)arena_alloc(g, count * sizeof(GIF_ImageDescriptor), 4);
}
static GIF_GCE *arena_alloc_gce(GIF_Object *g, size_t count)
{
    return (GIF_GCE *)arena_alloc(g, count * sizeof(GIF_GCE), 2);
}
static size_t *arena_alloc_sizes(GIF_Object *g, size_t count)
{
    return (size_t *)arena_alloc(g, count * sizeof(size_t), 4);
}

static unsigned int pow2_table_size(uint8_t flags_bits)
{
    // GIF: size = 2^(N+1), where N=flags&7
    return (1u << (((unsigned int)flags_bits & 7u) + 1u));
}

uint8_t gifn_get_bpp(const uint8_t flags)
{
    // From table size: 2^bpp entries
    uint8_t entries = pow2_table_size(flags);
    uint8_t bpp     = 0;
    while ((1u << bpp) < entries)
        ++bpp;
    if (bpp == 0) bpp = 1;
    return bpp;
}

// -----------------------------
// Palette parsing
// -----------------------------
static bool parse_color_table(GifData *R, GIF_Color *dst, int count)
{
    for (int i = 0; i < count; i++)
    {
        uint8_t r, g, b;
        if (!gifn_gifdata_read_u8(R, &r) ||
            !gifn_gifdata_read_u8(R, &g) ||
            !gifn_gifdata_read_u8(R, &b)) return false;
        dst[i].r = r;
        dst[i].g = g;
        dst[i].b = b;
    }
    return true;
}

// -----------------------------
// LZW state
// -----------------------------
typedef struct
{
    int16_t prefix;
    uint8_t first;
    uint8_t suffix;
} GifLZW;

typedef struct
{
    uint16_t screen_w, screen_h;
    // Per-image rect & flags
    uint16_t x, y, w, h;
    uint8_t interlace;
    uint8_t lct_present;
    uint8_t lct_flags;

    // Decode write cursors (byte = index)
    int start_x, start_y;
    int max_x, max_y;
    int cur_x, cur_y;
    int line_size; // bytes per full screen scanline (== screen_w)

    // Active palette for this image (points into GCT or LCT owned by GIF_Object)
    const GIF_Color *active_ct;
    int active_ct_count;

    // Output buffer (indices) for THIS image
    uint8_t *out; // size = w*h (sub-rectangle only)

    // Internal code table
    GifLZW codes[4096]; // 12-bit max
    uint8_t lflags;
} GifDecodeCtx;

// For interlaced images, advance lines in pass order
static void gif_write_index(GifDecodeCtx *C, uint8_t idx, int rel_pixel)
{
    // Compute absolute target within the image's out (w*h), respecting interlace write order.
    // We use a linear cursor over the sub-rectangle, but must mimic STB’s scan progression.
    // Simpler approach: map cur_x/cur_y to sub-buffer coordinates.
    int abs_x = (C->cur_x / 1) - C->start_x; // since 1 byte per pixel, cur_x is pixel offset
    int abs_y = (C->cur_y / C->line_size) - (C->start_y / C->line_size);
    // clamp (GIF files with zero-width rects are allowed; guard)
    if ((unsigned)abs_x < C->w && (unsigned)abs_y < C->h)
    {
        C->out[abs_y * C->w + abs_x] = idx;
    }
    C->cur_x += 1;
    if (C->cur_x >= C->max_x)
    {
        C->cur_x = C->start_x;
        // interlace stepping
        if (C->interlace)
        {

            // Find current pass from y mod pattern; cheap approach: move by step and wrap when crossing h
            // We'll implement stb-like "parse" counter:
            static const int steps[4]  = {8, 8, 4, 2};
            static const int starts[4] = {0, 4, 2, 1};
            // We emulate stb's variable by computing next interlaced y:
            int rel_y = (C->cur_y / C->line_size) - (C->start_y / C->line_size);
            int pass  = 0;
            if (rel_y % 8 == 0)
                pass = 0;
            else if ((rel_y - 4) % 8 == 0)
                pass = 1;
            else if ((rel_y - 2) % 4 == 0)
                pass = 2;
            else
                pass = 3;
            int next_rel_y = rel_y + steps[pass];
            if (next_rel_y >= C->h)
            {
                // move to next pass start
                pass++;
                if (pass > 3)
                {
                    C->cur_y = C->max_y;
                    return;
                }
                next_rel_y = starts[pass];
            }
            C->cur_y = C->start_y + next_rel_y * C->line_size;
        }
        else
        {
            C->cur_y += C->line_size;
        }
    }
}

static void gifn_out_code(GifDecodeCtx *C, uint16_t code)
{
    // Expand prefix chain recursively (iterative to avoid deep recursion on malformed data)
    uint16_t stack[4096];
    int sp = 0;
    while (C->codes[code].prefix >= 0 && sp < 4096)
    {
        stack[sp++] = (uint16_t)C->codes[code].suffix;
        code        = (uint16_t)C->codes[code].prefix;
    }
    // first & the last suffix
    uint8_t first = C->codes[code].first;
    gif_write_index(C, first, 0);
    while (sp)
    {
        gif_write_index(C, (uint8_t)stack[--sp], 0);
        if (C->cur_y >= C->max_y) return;
    }
};

static bool process_gif_raster(GifData *R, GifDecodeCtx *C)
{
    uint8_t lzw_cs;
    if (!gifn_gifdata_read_u8(R, &lzw_cs)) return false;
    if (lzw_cs > 12) return false;

    const int clear = 1 << lzw_cs;
    int codesize    = lzw_cs + 1;
    int codemask    = (1 << codesize) - 1;

    // init table
    for (int i = 0; i < clear; ++i)
    {
        C->codes[i].prefix = -1;
        C->codes[i].first  = (uint8_t)i;
        C->codes[i].suffix = (uint8_t)i;
    }
    int avail   = clear + 2;
    int oldcode = -1;
    int bits = 0, valid_bits = 0;
    int len = 0;

    // Prepare write cursors (byte addressing)
    C->line_size = C->screen_w; // bytes per full line
    C->start_x   = C->x;
    C->start_y   = C->y * C->line_size;
    C->max_x     = C->start_x + C->w;
    C->max_y     = C->start_y + C->h * C->line_size;
    C->cur_x     = C->start_x;
    C->cur_y     = C->start_y;

    for (;;)
    {
        if (valid_bits < codesize)
        {
            if (len == 0)
            {
                uint8_t s;
                if (!gifn_gifdata_read_u8(R, &s)) return false;
                len = s;
                if (len == 0) return true; // finished
            }
            uint8_t b;
            if (!gifn_gifdata_read_u8(R, &b)) return false;
            bits |= (int)b << valid_bits;
            valid_bits += 8;
            --len;
        }
        else
        {
            int code = bits & codemask;
            bits >>= codesize;
            valid_bits -= codesize;

            if (code == clear)
            {
                // reset
                codesize = lzw_cs + 1;
                codemask = (1 << codesize) - 1;
                avail    = clear + 2;
                oldcode  = -1;
            }
            else if (code == clear + 1)
            {
                // end of image data sub-blocks
                // consume rest of sub-blocks
                if (len) gifn_gifdata_seek(R, len), len = 0;
                uint8_t s;
                if (!gifn_gifdata_read_u8(R, &s)) return true;
                while (s)
                {
                    if (!gifn_gifdata_seek(R, s)) return false;
                    if (!gifn_gifdata_read_u8(R, &s)) return false;
                }
                return true;
            }
            else if (code < avail)
            {
                if (oldcode >= 0)
                {
                    GifLZW *p = &C->codes[avail++];
                    if (avail > 4096) return false;
                    p->prefix = (int16_t)oldcode;
                    p->first  = C->codes[oldcode].first;
                    p->suffix = C->codes[code].first;
                }
                gifn_out_code(C, (uint16_t)code);
                if ((avail & codemask) == 0 && avail <= 0x0FFF)
                {
                    ++codesize;
                    codemask = (1 << codesize) - 1;
                }
                oldcode = code;
            }
            else if (code == avail)
            {
                // KwKwK case
                if (oldcode < 0) return false;
                GifLZW *p = &C->codes[avail++];
                if (avail > 4096) return false;
                p->prefix = (int16_t)oldcode;
                p->first  = C->codes[oldcode].first;
                p->suffix = p->first;
                gifn_out_code(C, (uint16_t)avail - 1);
                if ((avail & codemask) == 0 && avail <= 0x0FFF)
                {
                    ++codesize;
                    codemask = (1 << codesize) - 1;
                }
                oldcode = code;
            }
            else
            {
                return false;
            }
            if (C->cur_y >= C->max_y)
            {
                // consume remaining sub-block bytes if any
                if (len) gifn_gifdata_seek(R, len), len = 0;
                uint8_t s;
                if (!gifn_gifdata_read_u8(R, &s)) return true;
                while (s)
                {
                    if (!gifn_gifdata_seek(R, s)) return false;
                    if (!gifn_gifdata_read_u8(R, &s)) return false;
                }
                return true;
            }
        }
    }
}

// -----------------------------
// Main loader
// -----------------------------
static int gifn_load_core(GIF_Object *gif, const uint8_t *in, size_t inSize)
{
    memset(gif, 0, sizeof(*gif));

    GifData R;
    gifn_gifdata_init(&R, in, inSize);

    // Header
    if (!gifn_gifdata_test_str(&R, "GIF")) return GIFN_ERR_FORMAT;
    uint8_t v3 = gifn_gifdata_at_rel(&R, 0);
    uint8_t v4 = gifn_gifdata_at_rel(&R, 1);
    uint8_t v5 = gifn_gifdata_at_rel(&R, 2);
    if (v3 != '8' || !((v4 == '7' || v4 == '9') && v5 == 'a')) return GIFN_ERR_FORMAT;
    gifn_gifdata_seek(&R, 3);

    if (!gifn_gifdata_read_u16le(&R, &gif->header.width) ||
        !gifn_gifdata_read_u16le(&R, &gif->header.height)) return GIFN_ERR_TRUNCATED;
    if (!gifn_gifdata_read_u8(&R, &gif->header.flags) ||
        !gifn_gifdata_read_u8(&R, &gif->header.bgColorIdx) ||
        !gifn_gifdata_read_u8(&R, &gif->header.aspect)) return GIFN_ERR_TRUNCATED;

    // Global Color Table
    gif->header.gct     = NULL;
    gif->header.gctSize = 0;
    if (gif->header.flags & 0x80)
    {
        int gcount      = pow2_table_size(gif->header.flags);
        gif->header.gct = arena_alloc_colors(gif, gcount);
        if (!gif->header.gct) return GIFN_ERR_NOMEM;
        if (!parse_color_table(&R, gif->header.gct, gcount)) return GIFN_ERR_TRUNCATED;
        gif->header.gctSize = (uint16_t)gcount; // entry count, not bytes
    }

    // Prepare dynamic arrays (conservative first pass: grow by re-alloc in arena style)
    gif->frames      = NULL;
    gif->framesGCE   = NULL;
    gif->frameLength = NULL;
    gif->frameCount  = 0;

    GIF_GCE currGCE        = {};
    currGCE.delayTime      = 0;
    currGCE.transparent    = 0;
    currGCE.transparentIdx = 0;
    currGCE.disposal       = 0;

    // Blocks loop
    for (;;)
    {
        if (gifn_gifdata_eof(&R)) break;
        uint8_t tag;
        if (!gifn_gifdata_read_u8(&R, &tag)) break;

        if (tag == 0x2C)
        { // Image Descriptor
            uint16_t x, y, w, h;
            uint8_t lflags;
            if (!gifn_gifdata_read_u16le(&R, &x) ||
                !gifn_gifdata_read_u16le(&R, &y) ||
                !gifn_gifdata_read_u16le(&R, &w) ||
                !gifn_gifdata_read_u16le(&R, &h)) return GIFN_ERR_TRUNCATED;
            if (!gifn_gifdata_read_u8(&R, &lflags)) return GIFN_ERR_TRUNCATED;

            // Allocate/extend arrays
            uint16_t idx                   = gif->frameCount;
            GIF_ImageDescriptor *oldFrames = gif->frames;
            GIF_GCE *oldGCE                = gif->framesGCE;
            size_t *oldLen                 = gif->frameLength;

            GIF_ImageDescriptor *newFrames = arena_alloc_frames(gif, idx + 1);
            GIF_GCE *newGCE                = arena_alloc_gce(gif, idx + 1);
            size_t *newLen                 = arena_alloc_sizes(gif, idx + 1);
            if (!newFrames || !newGCE || !newLen) return GIFN_ERR_NOMEM;

            if (idx > 0)
            {
                // preserve previously parsed frames
                memcpy(newFrames, oldFrames, idx * sizeof(GIF_ImageDescriptor));
                memcpy(newGCE, oldGCE, idx * sizeof(GIF_GCE));
                memcpy(newLen, oldLen, idx * sizeof(size_t));
            }

            gif->frames      = newFrames;
            gif->framesGCE   = newGCE;
            gif->frameLength = newLen;

            GIF_ImageDescriptor *F = &gif->frames[idx];

            F->x       = x;
            F->y       = y;
            F->w       = w;
            F->h       = h;
            F->flags   = lflags;
            F->indices = NULL;
            F->lct     = NULL;
            F->lctSize = 0;

            // Local Color Table
            const GIF_Color *active_ct = gif->header.gct;
            int active_ct_count        = gif->header.gct ? (int)gif->header.gctSize : 0;

            if (lflags & 0x80)
            {
                int lcount = pow2_table_size(lflags);
                F->lct     = arena_alloc_colors(gif, lcount);
                if (!F->lct) return GIFN_ERR_NOMEM;
                if (!parse_color_table(&R, F->lct, lcount)) return GIFN_ERR_TRUNCATED;
                F->lctSize      = (uint16_t)lcount; // entry count
                active_ct       = F->lct;
                active_ct_count = lcount;
            }

            // Allocate output (sub-rectangle only; you composite on your side)
            F->indices = arena_alloc_u8(gif, (size_t)w * (size_t)h);
            if (!F->indices) return GIFN_ERR_NOMEM;
            memset(F->indices, 0, (size_t)w * (size_t)h);

            // Decode image data into indices
            GifDecodeCtx C    = {};
            C.screen_w        = gif->header.width;
            C.screen_h        = gif->header.height;
            C.x               = x;
            C.y               = y;
            C.w               = w;
            C.h               = h;
            C.interlace       = (lflags & 0x40) ? 1 : 0;
            C.lct_present     = (lflags & 0x80) ? 1 : 0;
            C.active_ct       = active_ct;
            C.active_ct_count = active_ct_count;
            C.out             = F->indices;

            if (!process_gif_raster(&R, &C)) return GIFN_ERR_FORMAT;

            gif->framesGCE[idx]   = currGCE;
            gif->frameLength[idx] = (size_t)w * (size_t)h; // bytes in indices buffer

            gif->frameCount = idx + 1;
        }
        else if (tag == 0x21)
        { // Extension
            uint8_t label;
            if (!gifn_gifdata_read_u8(&R, &label)) return GIFN_ERR_TRUNCATED;
            if (label == 0xF9)
            {
                // Graphic Control Extension
                uint8_t block_size;
                if (!gifn_gifdata_read_u8(&R, &block_size)) return GIFN_ERR_TRUNCATED;
                if (block_size != 4)
                {
                    if (!gifn_gifdata_read_u8(&R, &block_size)) return GIFN_ERR_TRUNCATED;
                }
                else
                {
                    uint8_t packed;
                    if (!gifn_gifdata_read_u8(&R, &packed)) return GIFN_ERR_TRUNCATED;
                    uint16_t delay;
                    if (!gifn_gifdata_read_u16le(&R, &delay)) return GIFN_ERR_TRUNCATED;
                    uint8_t tindex;
                    if (!gifn_gifdata_read_u8(&R, &tindex)) return GIFN_ERR_TRUNCATED;

                    currGCE.delayTime      = delay;
                    currGCE.transparent    = (packed & 0x01) ? 1u : 0u;
                    currGCE.transparentIdx = tindex;
                    currGCE.disposal       = (uint8_t)((packed >> 2) & 0x07);
                }
                // Skip block terminator
                uint8_t term;
                if (!gifn_gifdata_read_u8(&R, &term)) return GIFN_ERR_TRUNCATED;
                while (term)
                {
                    if (!gifn_gifdata_seek(&R, term)) return GIFN_ERR_TRUNCATED;
                    if (!gifn_gifdata_read_u8(&R, &term)) return GIFN_ERR_TRUNCATED;
                }
            }
            else
            {
                // skip arbitrary extension blocks
                uint8_t s;
                if (!gifn_gifdata_read_u8(&R, &s)) return GIFN_ERR_TRUNCATED;
                while (s)
                {
                    if (!gifn_gifdata_seek(&R, s)) return GIFN_ERR_TRUNCATED;
                    if (!gifn_gifdata_read_u8(&R, &s)) return GIFN_ERR_TRUNCATED;
                }
            }
        }
        else if (tag == 0x3B)
        { // Trailer
            break;
        }
        else
        {
            // Unknown tag; error out to avoid infinite loops
            return GIFN_ERR_FORMAT;
        }
    }

    return GIFN_OK;
}

#ifndef GIFN_NO_STDIO
int gifn_load_file(GIF_Object *gif, const char *filepath)
{
    if (!gif || !filepath) return GIFN_ERR_FORMAT;
    FILE *f = fopen(filepath, "rb");
    if (!f) return GIFN_ERR_FORMAT;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0)
    {
        fclose(f);
        return GIFN_ERR_TRUNCATED;
    }
    uint8_t *buf = (uint8_t *)gifn_malloc((size_t)sz);
    if (!buf)
    {
        fclose(f);
        return GIFN_ERR_NOMEM;
    }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz)
    {
        gifn_free(buf);
        return GIFN_ERR_TRUNCATED;
    }
    int rc = gifn_load_core(gif, buf, (size_t)sz);
    gifn_free(buf);
    return rc;
}
#endif

int gifn_load_memory(GIF_Object *gif, const uint8_t *in, size_t inSize)
{
    if (!gif || !in || inSize < 10) return GIFN_ERR_FORMAT;
    return gifn_load_core(gif, in, inSize);
}

void gifn_cleanup(GIF_Object *gif)
{
    if (!gif) return;
    ArenaBlk *blk = (ArenaBlk *)gif->_arena;
    while (blk)
    {
        ArenaBlk *next = blk->next;
        gifn_free(blk);
        blk = next;
    }
    memset(gif, 0, sizeof(*gif));
}

// -----------------------------
// Color table helpers
// -----------------------------
uint32_t *gifn_color_table_as_u32(const GIF_Color *ct, const size_t size, const bool xbgr)
{
    if (!ct || size == 0) return NULL;
    uint32_t *p = (uint32_t *)gifn_malloc(sizeof(uint32_t) * size);
    if (!p) return NULL;
    for (size_t i = 0; i < size; i++)
    {
        uint8_t r = ct[i].r, g = ct[i].g, b = ct[i].b;
        p[i] = xbgr ? (0u | (b << 16) | (g << 8) | r) : (0u | (r << 16) | (g << 8) | b);
    }
    return p;
}

uint8_t *gifn_color_table_as_u8(const GIF_Color *ct, const size_t size, const bool xbgr)
{
    if (!ct || size == 0) return NULL;
    uint8_t *p = (uint8_t *)gifn_malloc(size * 3);
    if (!p) return NULL;
    for (size_t i = 0; i < size; i++)
    {
        if (xbgr)
        {
            p[i * 3 + 0] = ct[i].b;
            p[i * 3 + 1] = ct[i].g;
            p[i * 3 + 2] = ct[i].r;
        }
        else
        {
            p[i * 3 + 0] = ct[i].r;
            p[i * 3 + 1] = ct[i].g;
            p[i * 3 + 2] = ct[i].b;
        }
    }
    return p;
}
