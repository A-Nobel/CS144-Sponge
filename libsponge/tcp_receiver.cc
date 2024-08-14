#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // 记录seqno,因为后面要修改,按照规定,SYN的时候,这个segment占用一个seqno,如果不+1,那么unwrap计算abs_seq的时候,
    // 1.注意unwrap()-1是stream index,index+1是abs 绝对seqno
    // 2.注意syn和fin seq都要加1
    WrappingInt32 seq = seg.header().seqno;
    // 如果没SYN
    if (!_isn_set) {
        // 如果没有SYN,丢弃
        if (!seg.header().syn)
            return;
        // 第一次SYN
        _isn_set = true;
        // 起始isn
        _isn = seg.header().seqno;
        // 不知道
        seq = seq + 1;
    }
    // 读取body
    string body = seg.payload().copy();
    // 这里_reassembler.stream_out().bytes_written()+多少都行,不知道为什么
    // 写入buffer,具体index是abs-1
    _reassembler.push_substring(
        body, unwrap(seq, _isn, _reassembler.stream_out().bytes_written() + 114514) - 1, seg.header().fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_isn_set)
        return nullopt;
    return optional<WrappingInt32>(
        wrap(_reassembler.stream_out().bytes_written() + 1 + (_reassembler.stream_out().input_ended() ? 1 : 0), _isn));
}

size_t TCPReceiver::window_size() const { return _reassembler.stream_out().remaining_capacity(); }
