#pragma endian little
import * from gif as GIF;
import * from png as PNG;
import * from bmp as BMP;

// -----------------------------
// Records inside a FontBlob
// -----------------------------
bitfield Metrics {
    x : 8;
    y : 8;
    w : 7;
    na : 1;
    h : 7;
    icon : 1;
};

bitfield Hashcode {
    hash : 21;
    chain : 11;
};

struct Character {
    Hashcode hash;
    Metrics metrics;
};

// -----------------------------
// Embedded Font Blob 
// -----------------------------
struct FontBlob {
    char magic[4];              // "GVFO"
    u32  entry_hash;
    u8   version;
    u8   flags;
    u8   font_size;
    u8   space_width;
    u8   tab_width;
    u8   line_height;
    s8   baseline_offset;
    s8   kerning;
    u16  buckets;
    u16  char_count;
    char bitmapType[4];
    u8   bpp;
    u8   fgIndex;
    u8   shIndex;
    u8   reserved;
    u32  bitmapLength;
    u32  bitmapOffset;
    Character characters[char_count];
    u8 bitmapData[bitmapLength];
    if (bitmapType == "GIFI"){
        GIF gif @ addressof(bitmapData);
    } else if (bitmapType == "PNGI") {
        PNG png @ addressof(bitmapData);
    } else if (bitmapType == "BMPI"){
        BMP bmp @ addressof(bitmapData);
    }
};

// -----------------------------
// GVF container ("GVF\0")
// -----------------------------
struct GVF {
    char magic[4];              // "GVBF"
    u32  font_hash;
    u8   version;
    u8   family_len;
    u8   designer_len;
    u8   res1;
    u16  font_count;
    u16  res2;
    u32  first_blob_ofs;        // absolute offset to start of blob region
    char family_name[family_len];
    char family_designer[designer_len];
    u32  blob_ofs[font_count];
    FontBlob fontList[font_count];
};

// Materialize the header at file start
GVF gvf @ 0x0;