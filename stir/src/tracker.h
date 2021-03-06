/* -*- mode: C; c-basic-offset: 4; intent-tabs-mode: nil -*-
 *
 * STIR -- Sifteo Tracker Instance Reducer
 * Scott Perry <sifteo@numist.net>
 *
 * Copyright <c> 2012 Sifteo, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _TRACKER_H
#define _TRACKER_H

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <set>
#include "logger.h"
#include "sifteo/abi.h"

namespace Stir {

class Tracker;

class XmTrackerLoader {
public:
    XmTrackerLoader() : log(0), size(0) {}
    bool load(const char *filename, Logger &pLog);
    static void deduplicate(std::set<Tracker*> trackers, Logger &log);

private:
    friend class Tracker;
    bool openTracker(const char *filename);
    bool readSong();

    bool readNextInstrument();
    bool readSample(_SYSXMInstrument &instrument);
    // bool saveInstruments();

    bool readNextPattern();
    bool savePatterns();

    void emulatePingPongLoops(_SYSAudioModule &sample, std::vector<uint8_t> &pcmData);

    bool init();

    // Portable file functions, modules are little-endian binary files!
    void seek(uint32_t offset) { fseek(f, offset, SEEK_CUR); }
    void aseek(uint32_t offset) { fseek(f, offset, SEEK_SET); }
    uint32_t pos() { return ftell(f); }
    uint8_t get8() { return getc(f); }
    uint16_t get16() { return get8() | (uint16_t)get8() << 8; }
    uint32_t get32() { return get16() | (uint32_t)get16() << 16; }
    void getbuf( void *buf, uint32_t bufsiz ) {
        uint8_t *wbuf = (uint8_t*)buf;
        for (uint32_t i = 0; i < bufsiz; i++) {
            wbuf[i] = get8();
        }
    }

    void processName(std::string &name);

    // constants
    enum SampleFormat {
        kSampleFormatPCM8 = 0,
        kSampleFormatADPCM,
        kSampleFormatPCM16,
        kSampleFormatUnknown
    };
    static const char *encodings[3];

    static unsigned bytesToSamples(unsigned fmt, unsigned bytes)
    {
        switch (fmt) {
        case kSampleFormatPCM16:    return bytes / sizeof(int16_t);
        case kSampleFormatPCM8:     return bytes;
        case kSampleFormatADPCM:    return bytes * 2;
        default:                    return 0;
        }
    }

    const char *filename;
    FILE *f;
    Logger *log;
    _SYSXMSong song;
    uint32_t size;
    uint32_t fileSize;
    
    std::vector<std::vector<uint8_t> > patternDatas;
    std::vector<_SYSXMPattern> patterns;
    std::vector<uint8_t> patternTable;

    std::vector<_SYSXMInstrument> instruments;
    std::vector<std::vector<uint8_t> > envelopes;
    static std::vector<std::vector<uint8_t> > globalSampleDatas;
};

}

#endif // _TRACKER_H
