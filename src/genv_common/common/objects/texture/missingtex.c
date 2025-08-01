/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * missingtex.cpp - Created on 12-06-2025
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

#include "missingtex.h"

// StartOffset(h): 00000000, EndOffset(h): 000003D8, Length(h): 000003D9 */
// PNG format
const unsigned char missingTextureImg[] = {
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
	0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7A, 0x7A, 0xF4, 0x00, 0x00, 0x00,
	0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00,
	0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
	0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00,
	0x12, 0x74, 0x00, 0x00, 0x12, 0x74, 0x01, 0xDE, 0x66, 0x1F, 0x78, 0x00,
	0x00, 0x00, 0x26, 0x65, 0x58, 0x49, 0x66, 0x4D, 0x4D, 0x00, 0x2A, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x02, 0x01, 0x12, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x13, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x2E, 0x6A,
	0xAC, 0x00, 0x00, 0x01, 0x87, 0x69, 0x54, 0x58, 0x74, 0x58, 0x4D, 0x4C,
	0x3A, 0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x78,
	0x6D, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x3F, 0x78, 0x70, 0x61,
	0x63, 0x6B, 0x65, 0x74, 0x20, 0x62, 0x65, 0x67, 0x69, 0x6E, 0x3D, 0x27,
	0xEF, 0xBB, 0xBF, 0x27, 0x20, 0x69, 0x64, 0x3D, 0x27, 0x57, 0x35, 0x4D,
	0x30, 0x4D, 0x70, 0x43, 0x65, 0x68, 0x69, 0x48, 0x7A, 0x72, 0x65, 0x53,
	0x7A, 0x4E, 0x54, 0x63, 0x7A, 0x6B, 0x63, 0x39, 0x64, 0x27, 0x3F, 0x3E,
	0x0D, 0x0A, 0x3C, 0x78, 0x3A, 0x78, 0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61,
	0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x3D, 0x22, 0x61, 0x64,
	0x6F, 0x62, 0x65, 0x3A, 0x6E, 0x73, 0x3A, 0x6D, 0x65, 0x74, 0x61, 0x2F,
	0x22, 0x3E, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x52, 0x44, 0x46, 0x20, 0x78,
	0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x72, 0x64, 0x66, 0x3D, 0x22, 0x68, 0x74,
	0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x77, 0x33, 0x2E,
	0x6F, 0x72, 0x67, 0x2F, 0x31, 0x39, 0x39, 0x39, 0x2F, 0x30, 0x32, 0x2F,
	0x32, 0x32, 0x2D, 0x72, 0x64, 0x66, 0x2D, 0x73, 0x79, 0x6E, 0x74, 0x61,
	0x78, 0x2D, 0x6E, 0x73, 0x23, 0x22, 0x3E, 0x3C, 0x72, 0x64, 0x66, 0x3A,
	0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x20,
	0x72, 0x64, 0x66, 0x3A, 0x61, 0x62, 0x6F, 0x75, 0x74, 0x3D, 0x22, 0x75,
	0x75, 0x69, 0x64, 0x3A, 0x66, 0x61, 0x66, 0x35, 0x62, 0x64, 0x64, 0x35,
	0x2D, 0x62, 0x61, 0x33, 0x64, 0x2D, 0x31, 0x31, 0x64, 0x61, 0x2D, 0x61,
	0x64, 0x33, 0x31, 0x2D, 0x64, 0x33, 0x33, 0x64, 0x37, 0x35, 0x31, 0x38,
	0x32, 0x66, 0x31, 0x62, 0x22, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A,
	0x74, 0x69, 0x66, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F,
	0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F,
	0x6D, 0x2F, 0x74, 0x69, 0x66, 0x66, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x22,
	0x3E, 0x3C, 0x74, 0x69, 0x66, 0x66, 0x3A, 0x4F, 0x72, 0x69, 0x65, 0x6E,
	0x74, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x3E, 0x31, 0x3C, 0x2F, 0x74, 0x69,
	0x66, 0x66, 0x3A, 0x4F, 0x72, 0x69, 0x65, 0x6E, 0x74, 0x61, 0x74, 0x69,
	0x6F, 0x6E, 0x3E, 0x3C, 0x2F, 0x72, 0x64, 0x66, 0x3A, 0x44, 0x65, 0x73,
	0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x3E, 0x3C, 0x2F, 0x72,
	0x64, 0x66, 0x3A, 0x52, 0x44, 0x46, 0x3E, 0x3C, 0x2F, 0x78, 0x3A, 0x78,
	0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61, 0x3E, 0x0D, 0x0A, 0x3C, 0x3F, 0x78,
	0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x3D, 0x27,
	0x77, 0x27, 0x3F, 0x3E, 0x2C, 0x94, 0x98, 0x0B, 0x00, 0x00, 0x01, 0xA9,
	0x49, 0x44, 0x41, 0x54, 0x58, 0x47, 0xED, 0x96, 0xB1, 0x4A, 0xC3, 0x50,
	0x14, 0x40, 0x4F, 0x5C, 0xBA, 0xE8, 0xE6, 0xD4, 0xC9, 0x48, 0x9E, 0x5D,
	0xF2, 0x05, 0x8E, 0x0A, 0x15, 0xD4, 0xA5, 0xF9, 0x86, 0x62, 0x5D, 0x44,
	0x10, 0x24, 0x8B, 0x3F, 0x10, 0x04, 0xD1, 0xCD, 0x80, 0xDF, 0x60, 0x17,
	0x15, 0x2C, 0xD4, 0xCD, 0x3F, 0x28, 0xA2, 0x3E, 0x49, 0x37, 0xA7, 0x6E,
	0xBA, 0x38, 0xC5, 0xA1, 0xB1, 0xD6, 0x6B, 0x5E, 0x4C, 0xA8, 0xD5, 0x25,
	0x07, 0x1E, 0x84, 0x7B, 0x5F, 0x78, 0x27, 0xEF, 0xDD, 0xE4, 0xC6, 0xBA,
	0x27, 0x8E, 0x99, 0x80, 0x4D, 0x47, 0xC9, 0x50, 0x21, 0x66, 0x64, 0xE0,
	0xAF, 0x29, 0x05, 0x32, 0x05, 0xE6, 0x35, 0xD4, 0x34, 0x54, 0x64, 0x22,
	0x07, 0x5E, 0xA8, 0xD1, 0x7A, 0x38, 0x3A, 0xBE, 0x2B, 0xD3, 0x23, 0x8C,
	0x02, 0xD5, 0x18, 0x38, 0x84, 0x37, 0x99, 0xC8, 0x83, 0xEB, 0xB3, 0x16,
	0x35, 0x50, 0x4A, 0xA1, 0x1A, 0x67, 0xD0, 0xDC, 0xC1, 0x93, 0x73, 0x12,
	0x8C, 0x02, 0xCF, 0x16, 0x0C, 0x7A, 0x32, 0x9A, 0x93, 0x5E, 0x40, 0x2B,
	0x48, 0x6E, 0xEE, 0x69, 0xFA, 0xD8, 0x28, 0xC3, 0x26, 0x18, 0x05, 0x7E,
	0x0D, 0x57, 0x61, 0xF7, 0xBB, 0x5C, 0x1A, 0x1E, 0x66, 0xCA, 0x02, 0x2E,
	0xFE, 0xD1, 0x2A, 0xDD, 0xBD, 0x00, 0xC3, 0xFA, 0xD3, 0x15, 0xF0, 0xC2,
	0x36, 0xAB, 0xDD, 0x3D, 0x3E, 0x4E, 0x23, 0x8D, 0x29, 0x09, 0xB8, 0xF8,
	0x1D, 0xCD, 0x76, 0xD4, 0xA0, 0x9E, 0xB5, 0x7A, 0x96, 0x40, 0x35, 0x86,
	0xDA, 0x2D, 0x54, 0x1C, 0x58, 0x88, 0xA1, 0xBA, 0x25, 0x67, 0x64, 0xE0,
	0xED, 0xD0, 0xB4, 0xC1, 0x6E, 0xB6, 0x7F, 0x7C, 0x15, 0xAD, 0xB2, 0x17,
	0xC8, 0xC0, 0x5F, 0xF3, 0xEF, 0x02, 0x96, 0xE3, 0x38, 0x13, 0xD5, 0xC0,
	0xC5, 0x93, 0x96, 0xA1, 0x42, 0xFC, 0xFB, 0x0E, 0x94, 0x02, 0x46, 0x81,
	0xBC, 0xFD, 0x3C, 0x93, 0xE5, 0xE1, 0x47, 0x6C, 0xE1, 0x58, 0x26, 0x3E,
	0x49, 0x17, 0x28, 0xD0, 0xCF, 0xB3, 0x98, 0x3B, 0x80, 0xD7, 0x13, 0x19,
	0xFD, 0x4A, 0xBA, 0xC0, 0x78, 0x3F, 0x07, 0xE8, 0x47, 0x3C, 0x8E, 0xE7,
	0xF3, 0xB0, 0x05, 0x55, 0x60, 0x70, 0x27, 0x13, 0x5F, 0x49, 0x17, 0x18,
	0x3F, 0x82, 0xF6, 0x22, 0xA7, 0x75, 0x73, 0x3B, 0x35, 0x31, 0xBF, 0x0F,
	0xCF, 0xEB, 0x32, 0xFA, 0x1D, 0xA3, 0xC0, 0x79, 0x4B, 0x25, 0x47, 0x10,
	0xB1, 0xAD, 0x3B, 0x14, 0x29, 0x83, 0xCA, 0x31, 0xCC, 0x5E, 0xC2, 0x8B,
	0x4C, 0xA4, 0x60, 0x14, 0x18, 0xD1, 0x0B, 0x38, 0xBD, 0xB1, 0x59, 0x5C,
	0x92, 0x09, 0x33, 0x73, 0x1B, 0x50, 0xD9, 0x85, 0x5A, 0x0C, 0xB5, 0x30,
	0xB9, 0xBE, 0x92, 0xB3, 0x86, 0xA4, 0x0B, 0x78, 0x21, 0xE1, 0xA8, 0xEA,
	0x3C, 0xD6, 0x56, 0xFA, 0x44, 0x05, 0x8A, 0x60, 0xA0, 0xE0, 0xC1, 0x4A,
	0x46, 0x0B, 0xDE, 0x4E, 0xE0, 0xC1, 0x70, 0x1C, 0xE9, 0x02, 0xE7, 0xD7,
	0x10, 0x7C, 0xBC, 0x86, 0x01, 0xF8, 0xF5, 0xCC, 0xBF, 0x9A, 0x49, 0x28,
	0x7B, 0x41, 0x29, 0xF0, 0x0E, 0xC6, 0x59, 0x8F, 0x1A, 0x7D, 0x31, 0xC3,
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60,
	0x82
};

const unsigned int missingTextureSize = sizeof(missingTextureImg);
