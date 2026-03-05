/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * hashmap.hpp - Created on 26-02-2026
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

#include <stddef.h>
#include <string.h>
#include "common/util/hash.hpp"

namespace util
{
    // Fixed-size open-addressing hash map with linear probing.
    //
    // N must be a power of 2. The map is fully stack/static allocatable —
    // no heap, no constructors required beyond zero-initialisation.
    //
    // Sentinel key values (never valid as user keys):
    //   hashEmpty     (0x00000000) — slot has never been used
    //   hashTombstone (0xFFFFFFFF) — slot was deleted; skip on find(), reuse on insert()
    //
    // Probe sequence: linear, wrapping at N via key & (N-1).
    // find() terminates on hashEmpty (key absent). Tombstones are skipped.
    // insert() reuses the first tombstone seen during the probe, or the first
    // empty slot if none.
    //
    // Usage:
    //   util::HashMap<OutputPortRef, 32> map;
    //   map.insert("START_LAMP"_h, ref);
    //   if (auto *r = map.find("START_LAMP"_h)) r->on();
    //   map.remove("START_LAMP"_h);

    template <typename T, size_t N>
    class HashMap
    {
        static_assert(N && !(N & (N - 1)), "HashMap: N must be a power of 2");

        struct Slot
        {
            Hash key;
            T    value;
        };

        Slot   m_slots[N];
        size_t m_count;

    public:
        // Zero-init: all slots become hashEmpty, count = 0.
        inline void clear()
        {
            memset(m_slots, 0, sizeof(m_slots));
            m_count = 0;
        }

        inline size_t count()    const { return m_count; }
        inline size_t capacity() const { return N; }
        inline bool   full()     const { return m_count >= N; }

        // Insert or overwrite. Returns false if the map is full and no slot
        // (empty or tombstone) was found.
        bool insert(Hash key, const T &value)
        {
            // key must not be a sentinel
            if (key == hashEmpty || key == hashTombstone) return false;

            size_t idx       = key & (N - 1);
            size_t tombstone = N; // index of first tombstone seen, N = none

            for (size_t i = 0; i < N; i++)
            {
                Slot &s = m_slots[idx];

                if (s.key == hashEmpty)
                {
                    // Use tombstone slot if we passed one, otherwise this slot
                    size_t target = (tombstone < N) ? tombstone : idx;
                    if (tombstone < N && m_slots[tombstone].key == hashTombstone)
                    {
                        // Replacing a tombstone does not increase count
                        m_slots[tombstone].key   = key;
                        m_slots[tombstone].value = value;
                    }
                    else
                    {
                        s.key   = key;
                        s.value = value;
                        m_count++;
                    }
                    (void)target;
                    return true;
                }

                if (s.key == key)
                {
                    // Overwrite existing entry
                    s.value = value;
                    return true;
                }

                if (s.key == hashTombstone && tombstone == N)
                    tombstone = idx;

                idx = (idx + 1) & (N - 1);
            }

            // Probe exhausted — use tombstone if available
            if (tombstone < N)
            {
                m_slots[tombstone].key   = key;
                m_slots[tombstone].value = value;
                return true;
            }

            return false; // map is full
        }

        // Returns pointer to value, or nullptr if not found.
        T *find(Hash key)
        {
            if (key == hashEmpty || key == hashTombstone) return nullptr;

            size_t idx = key & (N - 1);

            for (size_t i = 0; i < N; i++)
            {
                Slot &s = m_slots[idx];

                if (s.key == hashEmpty)   return nullptr;
                if (s.key == key)         return &s.value;

                idx = (idx + 1) & (N - 1);
            }

            return nullptr;
        }

        const T *find(Hash key) const
        {
            if (key == hashEmpty || key == hashTombstone) return nullptr;

            size_t idx = key & (N - 1);

            for (size_t i = 0; i < N; i++)
            {
                const Slot &s = m_slots[idx];

                if (s.key == hashEmpty)   return nullptr;
                if (s.key == key)         return &s.value;

                idx = (idx + 1) & (N - 1);
            }

            return nullptr;
        }

        // Iterate all live entries in bucket order (not insertion order).
        // Callback signature: void(Hash key, T &value).
        // Safe to call remove(key) inside the callback — iteration continues
        // by raw bucket index so tombstoning a visited slot does not skip others.
        template <typename Fn>
        void forEach(Fn callback)
        {
            for (size_t i = 0; i < N; i++)
            {
                Slot &s = m_slots[i];
                if (s.key != hashEmpty && s.key != hashTombstone)
                    callback(s.key, s.value);
            }
        }

        // Marks the slot as a tombstone. Returns false if key was not found.
        bool remove(Hash key)
        {
            if (key == hashEmpty || key == hashTombstone) return false;

            size_t idx = key & (N - 1);

            for (size_t i = 0; i < N; i++)
            {
                Slot &s = m_slots[idx];

                if (s.key == hashEmpty)    return false;
                if (s.key == key)
                {
                    s.key = hashTombstone;
                    m_count--;
                    return true;
                }

                idx = (idx + 1) & (N - 1);
            }

            return false;
        }
    };

} // namespace util
