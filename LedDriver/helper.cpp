#include <stdint.h>

typedef uint32_t gpio_bits_t;

#define gpio_set_bits (uint32_t *)0
#define gpio_clr_bits (uint32_t *)0

int slowdown_ = 4;

inline void WriteSetBits(gpio_bits_t value) { //=
    *gpio_set_bits = static_cast<uint32_t>(value & 0xFFFFFFFF);
}

inline void WriteClrBits(gpio_bits_t value) { //=
    *gpio_clr_bits = static_cast<uint32_t>(value & 0xFFFFFFFF);
}


    // Write all the bits of "value" mentioned in "mask". Leave the rest untouched.
inline void WriteMaskedBits(gpio_bits_t value, gpio_bits_t mask) {
    // Writing a word is two operations. The IO is actually pretty slow, so
    // this should probably  be unnoticable.
    ClearBits(~value & mask);
    SetBits(value & mask);
}

inline void SetBits(gpio_bits_t value) {
    if (!value) return;
        WriteSetBits(value);
    for (int i = 0; i < slowdown_; ++i) {
        WriteSetBits(value);
    }
}

// Clear the bits that are '1' in the output. Leave the rest untouched.
inline void ClearBits(gpio_bits_t value) {
    if (!value) return;
        WriteClrBits(value);
    for (int i = 0; i < slowdown_; ++i) {
        WriteClrBits(value);
    }
}


virtual void SendPulse(int time_spec_number) {
    io_->ClearBits(bits_);
    sleep_nanos(nano_specs_[time_spec_number]);
    io_->SetBits(bits_);
}