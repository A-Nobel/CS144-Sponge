#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity)
    , _capacity(capacity)
    , _stream(capacity)
    , _cur_index(0)
    , _eof_index(std::numeric_limits<size_t>::max())
    , _unassembled_bytes_cnt(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    auto st = max(index, _cur_index);
    auto ed = min(_eof_index, min(index + data.size(), _cur_index + _output.remaining_capacity()));
    if (eof) {
        _eof_index = min(_eof_index, index + data.size());
    }
    // 注意st和st - index的取值非常巧妙
    for (size_t i = st, j = st - index; i < ed; i++, j++) {
        auto &pair = _stream[i % _capacity];
        if (pair.second) {
            if (pair.first != data[j]) {
                throw runtime_error("Different char!");
            }
        } else {
            pair = make_pair(data[j], true);
            _unassembled_bytes_cnt++;
        }
    }
    string str;
    while (_cur_index != _eof_index && _stream[_cur_index % _capacity].second) {
        str.push_back(_stream[_cur_index % _capacity].first);
        _stream[_cur_index % _capacity].second = false;
        _cur_index++;
        _unassembled_bytes_cnt--;
    }
    _output.write(str);
    if (_cur_index == _eof_index) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes_cnt; }

bool StreamReassembler::empty() const { return _unassembled_bytes_cnt == 0; }
