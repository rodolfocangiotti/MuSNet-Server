#include <iostream>
#include "Console.h"
#include "TCPRequestInfo.h"
#include "utils.h"

TCPRequestInfo::TCPRequestInfo(const BufferSize bs):
    RequestInfo(),
    _segment(bs) {
#if defined(DEBUG) && VERBOSENESS > 2
    Console::log(getUTCTime() + " [DEBUG] Constructing TCPRequestInfo class...");
#endif
}

TCPRequestInfo::~TCPRequestInfo() {
#if defined(DEBUG) && VERBOSENESS > 2
    Console::log(getUTCTime() + " [DEBUG] Destructing TCPRequestInfo class...");
#endif
}


const TCPSegment& TCPRequestInfo::refer_segment() const {
    return _segment;
}

TCPSegment& TCPRequestInfo::refer_writable_segment() {
    return _segment;
}
