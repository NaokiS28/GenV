/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * return_codes.hpp - Created on 03-10-2025
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

#pragma once

// =======================
// Error Code Bit Layout
// =======================
// 32-bit error code layout:
// [31:28]  Flags / Category
// [27:20]  Service / Subsystem ID (0x00 = generic)
// [19:0]   Error code
//
// Categories (top 4 bits):
// 0x0 = Universal error (standardized, same across services)
// 0x1 = Subsystem-defined error (service-specific meaning)
// 0xF = Reserved (future use)
//
// Example:
//   GV_MAKE_ERROR(GV_CATEGORY_UNIVERSAL, GV_SERVICE_FILESYSTEM, GV_ERR_FILE_NOT_FOUND)
//   GV_MAKE_ERROR(GV_CATEGORY_SUBSYSTEM, GV_SERVICE_NETWORK,    0x001)   // service-specific

// Macros for constructing/parsing codes
#define GV_ERROR(service, category, code) \
    (((service & 0xF) << 28) | ((category & 0xFF) << 20) | (code & 0xFFFFF))

#define GV_GET_SERVICE(err) (((err) >> 28) & 0xF)
#define GV_GET_CATEGORY(err) (((err) >> 20) & 0xFF)
#define GV_GET_CODE(err) ((err) & 0xFFFFF)

enum GVE_Categories
{
    GV_CATEGORY_GENERIC
};

enum GVE_Systems
{
    GV_SERVICE_GENERIC,
    GV_SERVICE_VIDEO,
    GV_SERVICE_AUDIO,
    GV_SERVICE_FILESYSTEM,
    GV_SERVICE_NETWORK,
    GV_SERVICE_MEMORY,
    GV_SERVICE_SECURITY,
};

// =======================
// Universal Error Codes
// =======================
// These are stable and shared across all services.
// Must be < 0x10000 so subsystem-specific codes have room above.
enum GVE_UniversalResults
{
    GV_OK = 0, // Operation succeeded

    // Memory / Resource errors
    GV_ERR_OUT_OF_MEMORY = 1, // Insufficient memory
    GV_ERR_OUT_OF_SPACE,      // No disk/storage space
    GV_ERR_RESOURCE_BUSY,     // Resource is currently in use
    GV_ERR_RESOURCE_LOCKED,   // Could not acquire lock
    GV_ERR_LIST_EMPTY,        // List or collection empty

    // Filesystem errors
    GV_ERR_FILE_NOT_FOUND,    // File was not found
    GV_ERR_PATH_INVALID,      // Invalid/malformed path
    GV_ERR_FILE_IN_USE,       // File locked by another process
    GV_ERR_ACCESS_DENIED,     // Insufficient permissions
    GV_ERR_IO,                // Generic I/O error
    GV_ERR_NOT_LOADED,        // File found but not loadable
    GV_ERR_INCOMPATIBLE_TYPE, // Type mismatch
    GV_ERR_BAD_NAME,          // Invalid name

    // Objects
    GV_ERR_CREATE_FAILED, // Failed to create object/resource

    GV_ERR_UPLOAD_FAILED,
    GV_ERR_DOWNLOAD_FAILED,

    // Network errors
    GV_ERR_NETWORK_DOWN,       // Network unavailable
    GV_ERR_HOST_UNREACHABLE,   // Host/service unreachable
    GV_ERR_TIMEOUT,            // Operation timed out
    GV_ERR_PROTOCOL_ERROR,     // Protocol violation
    GV_ERR_CONNECTION_REFUSED, // Connection refused

    // Parameter / Logic errors
    GV_ERR_INVALID_PARAM,     // Invalid parameter
    GV_ERR_INVALID_STATE,     // Operation not valid in current state
    GV_ERR_INVALID_OPERATION, // Unsupported operation
    GV_ERR_NULL_POINTER,      // Null pointer passed
    GV_ERR_BAD_OBJECT,        // Object invalid or corrupted

    // Security / Auth errors
    GV_ERR_AUTH_FAILED,    // Authentication failure
    GV_ERR_NOT_AUTHORIZED, // Not authorized
    GV_ERR_TOKEN_EXPIRED,  // Security token expired
    GV_ERR_CRYPT_ERROR,    // Cryptographic failure

    // Database / Persistence errors
    GV_ERR_ITEM_NOT_FOUND,    // Missing item
    GV_ERR_CONFLICT,          // Data conflict (duplicate key, etc.)
    GV_ERR_CONSTRAINT_FAIL,   // Constraint violation
    GV_ERR_TRANSACTION_ABORT, // Transaction aborted/rolled back

    // System / Internal errors
    GV_ERR_UNSUPPORTED, // Feature not supported
    GV_ERR_INTERNAL,    // Internal implementation error
    GV_ERR_UNKNOWN      // Unknown/unclassified error
};

// =======================
// Example Usage
// =======================
//
// Universal error (cross-service standard):
// int err = GV_MAKE_ERROR(GV_CATEGORY_UNIVERSAL, GV_SERVICE_FILESYSTEM, GV_ERR_FILE_NOT_FOUND);
//
// Subsystem-specific error (bespoke):
// int err = GV_MAKE_ERROR(GV_CATEGORY_SUBSYSTEM, GV_SERVICE_DATABASE, 0x0100); // e.g. SQL deadlock
//
