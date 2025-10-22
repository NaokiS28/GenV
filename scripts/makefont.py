#!/usr/bin/env python3
# 
# GenV - Copyright (C) 2025 NaokiS, spicyjpeg
# makefont.py - Created on 07-06-2025
# 
# GenV is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
# 
# GenV is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# GenV. If not, see <https://www.gnu.org/licenses/>.
#
# Some parts from 573in1 - Copyright (C) 2022-2024 spicyjpeg
#
# 573in1 is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# 573in1 is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# 573in1. If not, see <https://www.gnu.org/licenses/>.


from __future__ import annotations
import json
import sys
from argparse import ArgumentParser, FileType, Namespace
from dataclasses import dataclass
from io import BufferedReader
from pathlib import Path
from struct import Struct
from typing import Any
import numpy as np

from common.util import normalizeFileName, hashData, HashTableBuilder

# ---------- Structs ----------
LE = {
	"metrics_entry": "< 2I",
	"fontset_header": "< 4s B B {family_len}s B {designer_len}s H I",
	"font_header"   : "< 4s B B {name_len}s I B B B B b b b H H I 4s B I I B B"
	#							^  ^ ^^^ ^ 	^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^^ ^ ^ ^ ^ ^ 
	#							|  | |   |    | | | | | | | | | | |  | | | | | 
	# 	magic, ver, name, id -------------    | | | | | | | | | | |  | | | | |
	# 	flags, fontSize, space, tab --------------    | | | | | | |  | | | | |
	# 	line, base, kerning ------------------------------/ | | | |  | | | | |
	# 	buckets, entries, table_offset -------------------------/ |  | | | | |
	# 	bitmapType, bpp, bitmapLen, -------------------------------------/ | |
	# 	fg,sh 					----------------------------------------------
}

_FONTSET_MAGIC: bytes = b"GVBF"
_FONTSET_VERSION: int = 1

_FONTOBJ_MAGIC: bytes = b"GVFO"
_FONTOBJ_VERSION: int	= 1

# ---------- Helpers ----------
# TODO: Make this actually detect the BPP of the image
def _detect_bitmap_type(path: Path) -> tuple[int,int,bytes]:
	"""
	Return (bitmapType, bpp, raw_bytes)
	bitmapType encodes ascii of magic:
	  'PNGI' for PNG and 'GIFI' for GIF
	If unknown, type=0, bpp=32.
	"""
	raw = path.read_bytes()
	if raw.startswith(b"\\x89PNG\\r\\n\\x1a\\x0a"):
		return (b"PNGI", 8, raw) 
	if raw.startswith(b"GIF87a") or raw.startswith(b"GIF89a"):
		return (b"GIFI", 8, raw)
	return (0, 32, raw)

def _clamp(v:int, lo:int, hi:int) -> int:
	return lo if v < lo else hi if v > hi else v

# ---------- Builders ----------
def _read_metrics(metrics_path: Path) -> dict[str, Any]:
	with metrics_path.open("r", encoding="utf-8") as f:
		return json.load(f)

def _make_font_blob(
	family_name: str, 
	size_from_fontset: int, 
	texture_path: Path, 
	metrics_path: Path
) -> bytearray:
	metrics 	= _read_metrics(metrics_path)
	font_size  	= int(metrics["fontSize"])
	space	  	= int(metrics["spaceWidth"])
	tab			= int(metrics["tabWidth"])
	line_h	 	= _clamp(int(metrics["lineHeight"]), -128, 127)
	baseline   	= _clamp(int(metrics["baselineOffset"]), -128, 127)
	kerning		= _clamp(int(metrics["kerning"]), -128, 127)

	# Texture payload
	bitmapType, bpp, bitmap = _detect_bitmap_type(texture_path)

	# Name
	name = f"{family_name} {font_size}px"

	# Flags
	flag_bits = 0
	for flags in metrics.get("flags"):
		if "italic" in flags: 
			flag_bits |= 1
			name += "Italic"
		if "bold"   in flags: 
			flag_bits |= 2
			name += "Bold"

	# Generate hashtable
	numBuckets: int = 256
	hashTable: HashTableBuilder = HashTableBuilder(numBuckets)
	for ch, entry in metrics["characterSizes"].items():
		x: int  = int(entry["x"])
		y: int  = int(entry["y"])
		w: int  = int(entry["width"])
		h: int  = int(entry["height"])
		i: bool = bool(entry.get("icon", False))

		if (x < 0) or (x > 255) or (y < 0) or (y > 255):
			raise ValueError("all X/Y coordinates must be in 0-255 range")
		if (w < 0) or (w > 127) or (h < 0) or (h > 127):
			raise ValueError("all characters must be <=127x127 pixels")
		if h > font_size:
			raise ValueError("character height exceeds font size")

		# Format here is as follows:
		# 31: IHHHHHHH xHWWWWWW YYYYYYYY XXXXXXXX :0
		hashTable.addEntry(ord(ch), 0
			| (x <<  0)
			| (y <<  8)
			| (w << 16)
			| (h << 23)
			| (i << 30)
		)

	# Make the glyph table
	metrics: bytearray = bytearray()
	mStruct = Struct(LE["metrics_entry"])
	for entry in hashTable.entries:
		if entry is None:
			metrics += bytes(mStruct.size)
		else:
			metrics += mStruct.pack(
				entry.fullHash | (entry.chainIndex << 21),
				entry.data
			)

	# Compose header
	header: bytearray = bytearray()
	fHeader = Struct(LE["font_header"].format(name_len = len(name) + 1))
	header = fHeader.pack(
		_FONTOBJ_MAGIC, 
		_FONTOBJ_VERSION,
		np.uint8(len(name)), 
		name.encode("utf-8"), 
		hashData(name.encode("utf-8")),
		flag_bits, 
		font_size, 
		space, 
		tab,
		line_h, 
		baseline, 
		kerning,
		numBuckets,
		len(hashTable.entries),
		fHeader.size, # Offset of table
		bitmapType, 
		bpp, 
		len(bitmap),
		len(metrics) + 2, # Relative offset of bitmap
		0, 0  # fgIndex, shIndex
	)

	# Add the font bitmap (TODO: encode to standard format?)
	with texture_path.open("rb") as f:
		bitmapData = f.read()

	fontBlob: bytearray = bytearray()
	fontBlob += header
	fontBlob += metrics
	fontBlob += bitmapData
	return fontBlob

def build_gvf(
	spec_path: Path, 
	source_dir: Path | None = None
) -> bytes:
	with spec_path.open("r", encoding="utf-8") as f:
		spec = json.load(f)

	family 		= str(spec["familyName"])
	designer 	= str(spec["familyDesigner"])
	fonts 		= spec["fonts"]

	blobs: list[bytes] = []
	offsets = np.array([], dtype=np.uint32)
	thisOffset: int = 0
	for f in fonts:
		size = int(f["size"])
		tex  = Path(f["texture"])
		met  = Path(f["metrics"])
		if source_dir:
			if not tex.is_absolute(): tex = source_dir / tex
			if not met.is_absolute(): met = source_dir / met
		thisBlob = _make_font_blob(family, size, tex, met)
		blobs.append(thisBlob)
		offsets = np.append(offsets, np.uint32([thisOffset]))
		thisOffset += len(thisBlob)

	# Get first font blob offset
	hStruct = Struct(LE["fontset_header"].format(family_len = len(family) + 1, designer_len = len(designer) + 1))
	blobOffset = hStruct.size + offsets.nbytes
	
	# Build fontset header
	header: bytearray = bytearray()
	header = hStruct.pack(
		_FONTOBJ_MAGIC,
		_FONTSET_VERSION,
		np.uint8(len(family)),
		family.encode("utf-8"),
		np.uint8(len(designer)),
		designer.encode("utf-8"),
		len(blobs),
		blobOffset
	)

	out = bytearray(header)
	out += blobOffset.to_bytes(4, byteorder='little', signed=False)
	for b in blobs:
		out += b
	return bytes(out)

# ---------- CLI ----------
def _parser() -> ArgumentParser:
	p = ArgumentParser(description="Build a GenV .gvf Fontset (GVBF) file from JSON specs.")
	p.add_argument("json", type=str, help="Path to fontset.json-like file")
	p.add_argument("-o", "--output", type=str, required=True, help="Output .gvf path")
	p.add_argument("--source-dir", type=str, default=None, help="Base directory for relative texture/metrics paths")
	return p

def main() -> None:
	args = _parser().parse_args()
	src = Path(args.json)
	source_dir = Path(args.source_dir) if args.source_dir else src.parent
	data = build_gvf(src, source_dir=source_dir)
	Path(args.output).write_bytes(data)

if __name__ == "__main__":
	main()