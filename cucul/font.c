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
 * The libcaca font format
 * -----------------------
 *
 * All types are big endian.
 *
 * struct
 * {
 *    uint8_t caca_header[4];    // "CACA"
 *    uint8_t caca_file_type[4]; // "FONT"
 *
 * header:
 *    uint32_t header_size;      // size of the header (data - header)
 *    uint16_t version;          // font format version (= 1)
 *
 *    uint16_t glyphs;           // total number of glyphs in the font
 *    uint16_t width;            // glyph width
 *    uint16_t height;           // glyph height
 *    uint16_t bytes;            // bytes used to code each character,
 *                               // rounded up to a multiple of 2.
 *
 *    uint16_t blocks;           // number of blocks in the font
 *
 *    struct
 *    {
 *        uint32_t start;        // Unicode index of the first character
 *        uint32_t end;          // Unicode index of the last character + 1
 *        uint32_t offset;       // Offset (starting from data) to the data
 *                               // for the first character
 *    }
 *    block_list[blocks];
 *
 * padding:
 *    ...                        // reserved for future use
 *
 * data:
 *    uint4_t[glyphs * bytes];   // glyph data
 * };
 *
 */

