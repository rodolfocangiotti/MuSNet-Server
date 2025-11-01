#ifndef TCP_REQUEST_INFO
#define TCP_REQUEST_INFO

#include <arpa/inet.h>
#include <cstdint>
#include "RequestInfo.h"
#include "TCPSegment.h"
#include "types.h"

class TCPRequestInfo: public RequestInfo {
public:
    typedef uint64_t BufferSize;
    // ********************
    TCPRequestInfo(const BufferSize bs);
    ~TCPRequestInfo();
    // ********************
    const TCPSegment& refer_segment() const;
    TCPSegment& refer_writable_segment();
private:
    TCPSegment _segment;
};

#endif
