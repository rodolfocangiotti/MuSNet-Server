#include <cassert>
#include <iostream>
#include "Chrono.h"
#include "Console.h"
#include "Profiler.h"
#include "TCPResponse.h"
#include "TCPSegment.h"
#include "commons.h"
#include "prettyprint.h"
#include "utils.h"


TCPResponse::TCPResponse(Manager& manager):
    _manager(manager) {
#if defined(DEBUG) && VERBOSENESS > 2
    Console::log(getUTCTime() + " [DEBUG] Constructing TCPResponse class...");
#endif
}

TCPResponse::~TCPResponse() {
#if defined(DEBUG) && VERBOSENESS > 2
    Console::log(getUTCTime() + " [DEBUG] Destructing TCPResponse class...");
#endif
}

void TCPResponse::operator()(TCPRequestInfo& request) {
    const TCPSegment& request_segment = request.refer_segment();

    if (request_segment.header() == AUDIO_STREAM_DATA) {
        ClientToken t = request_segment.token();
        ClientTID request_tid = request_segment.tid();
        ClientTID response_tid = 0;
        // Profiler::add_record(t, request_tid, Profiler::OperationID::RESPONSE_START);
        AudioVector from_client = request_segment.streamCopy();
        AudioVector to_client;
        {
            std::lock_guard<std::mutex> locker(_manager.referMutex());
            std::list<ClientTID> tids = _manager.getTIDHistory(t);
            for (std::list<ClientTID>::iterator it = tids.begin(); it != tids.end(); it++) {
                if (request_tid == *it) {
                    std::cerr << RED << "[WARNING] Skipping TCP duplicate request " << request_tid << '\n';
                    return;
                }
            }
            if (_manager.updateClientStream(t, request_tid, from_client) < 0) {  // Save stream from client...
                std::cerr << getUTCTime() + RED << " [ERROR] Error updating client stream!" << RESET << '\n';
            }
            to_client = _manager.getOtherClientStreams(t);  // Compute the response stream to client...
            // response_tid = _manager.getClientResponseTID(t);
            response_tid = request_tid;
        }
        // Profiler::add_record(t, request_tid, Profiler::OperationID::RESPONSE_END);
        if (response_tid > 0) {
            TCPSegment& tcp_segment = request.refer_writable_segment();
            tcp_segment.buildAudioStream(t, response_tid, to_client);
            // Profiler::add_record(t, response_tid, Profiler::OperationID::DISPATCH_PENDING);

            struct sockaddr_in addrss = request.address();
            socklen_t addrssLen = request.addressLength();
            SocketFD sockFD = request.fileDescriptor();
            std::cout << "DEBUG: sockFD = " << sockFD << '\n';
            int bytes = sendto(sockFD, (const char*) request_segment.rawBuffer(), UDP_BUFFER_SIZE, 0, nullptr, 0);
            if (bytes < 0) { // TODO Test it!
                perror("sendto()");
            }
        }
    } else {
        std::cerr << getUTCTime() + RED << " [ERROR] Not consistent header (" << static_cast<int16_t>(request_segment.header()) << ") of TCP datagram!" << RESET << '\n';
    }
}
