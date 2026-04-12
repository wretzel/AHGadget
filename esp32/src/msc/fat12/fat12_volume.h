#pragma once
#include <stdint.h>
#include "fat12_utils.h"

namespace fat12 {

    class Volume {
    public:
        bool load(const uint8_t* sector0);
        const BPB& bpb() const { return _bpb; }

        uint32_t fatOffset() const;
        uint32_t rootDirOffset() const;
        uint32_t dataOffset() const;

    private:
        BPB _bpb;
    };

}
