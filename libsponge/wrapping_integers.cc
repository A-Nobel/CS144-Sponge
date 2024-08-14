#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return WrappingInt32{static_cast<uint32_t>(n) + isn.raw_value()}; }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
static uint64_t distance(uint64_t candi, uint64_t checkpoint) {
    if (candi < checkpoint) {
        return checkpoint - candi;
    } else {
        return candi - checkpoint;
    }
}

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t up32 = checkpoint & 0xffffffff00000000;
    uint64_t low32 = (n.raw_value() - isn.raw_value()) & 0x00000000ffffffff;
    uint64_t candi1 = (up32 | low32) - 0x0000000100000000;
    uint64_t candi2 = (up32 | low32);
    uint64_t candi3 = (up32 | low32) + 0x0000000100000000;
    if (distance(candi1, checkpoint) < distance(candi2, checkpoint)) {
        if (distance(candi1, checkpoint) < distance(candi3, checkpoint)) {
            return candi1;
        } else {
            return candi3;
        }
    } else {
        if (distance(candi2, checkpoint) < distance(candi3, checkpoint)) {
            return candi2;
        } else {
            return candi3;
        }
    }
}
