/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file font.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Colour handling
 *
 *  This file contains font handling functions.
 */

/*
 * The libcaca font format, version 1
 * ----------------------------------
 *
 * All types are big endian.
 *
 * struct
 * {
 *    uint8_t caca_header[4];    // "CACA"
 *    uint8_t caca_file_type[4]; // "FONT"
 *
 * font_header:
 *    uint32_t header_size;      // Header size (font_data - font_header)
 *    uint32_t data_size;        // Data size (EOF - font_data)
 *
 *    uint16_t version;          // Font format version
 *                               //  bit 0: set to 1 if font is compatible
 *                               //         with version 1 of the format
 *                               //  bits 1-15: unused yet, must be 0
 *
 *    uint16_t blocks;           // Number of blocks in the font
 *    uint32_t glyphs;           // Total number of glyphs in the font
 *
 *    uint16_t bpp;              // Bits per pixel for glyph data (valid
 *                               // Values are 1, 2, 4 and 8)
 *    uint16_t width;            // Maximum glyph width
 *    uint16_t height;           // Maximum glyph height
 *
 *    uint16_t flags;            // Feature flags
 *                               //  bit 0: set to 1 if font is fixed width
 *                               //  bits 1-15: unused yet, must be 0
 *
 * block_info:
 *    struct
 *    {
 *       uint32_t start;         // Unicode index of the first glyph
 *       uint32_t end;           // Unicode index of the last glyph + 1
 *       uint32_t info_offset;   // Offset (starting from data) to the info
 *                               // for the first glyph
 *    }
 *    block_list[blocks];
 *
 * glyph_info:
 *    struct
 *    {
 *       uint16_t width;         // Glyph width in pixels
 *       uint16_t height;        // Glyph height in pixels
 *       uint32_t data_offset;   // Offset (starting from data) to the data
 *                               // for the first character
 *    }
 *    glyph_list[glyphs];
 *
 * extension_1:
 * extension_2:
 *    ...
 * extension_N:
 *    ...                        // reserved for future use
 *
 * font_data:
 *    uint8_t data[data_size];   // glyph data
 * };
 */

