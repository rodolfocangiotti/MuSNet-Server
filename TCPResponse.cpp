#include <unistd.h>
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
    const SocketFD request_fd = request.fileDescriptor();
    const struct sockaddr_in request_addr = request.address();
    const socklen_t request_addr_len = request.addressLength();

    if (request_segment.header() == AUDIO_STREAM_DATA) {
        ClientToken t = request_segment.token();
#ifdef DEBUG
        Console::log("Audio exchange request from token " + str(t));
#endif
        if ((_manager.is_valid_token(t))) {
            ClientTID request_tid = request_segment.tid();
            TCPSegment::Flag flag = request_segment.flag();
            // std::cout << "flag (checkpoint n.2): " << (int) flag << '\n';
            // Profiler::add_record(t, request_tid, Profiler::OperationID::RESPONSE_START);
            AudioVector from_client = request_segment.streamCopy();
            // Response data...
            ClientToken response_token;
            ClientTID response_tid;
            Flag response_flag = 0;
            SocketFD response_fd;
            struct sockaddr_in response_addr;
            socklen_t response_addr_len;
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
                // Always update connection info...
                _manager.update_client_connection_info(t, request_fd, &request_addr, request_addr_len);
                // ...and flag. TODO: Check if it is necessary.
                _manager.update_client_flag(t, flag);

                if (flag == RECEIVE_ONLY_STREAM) {
                    from_client = AudioVector(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
                }
                if (_manager.updateClientStream(t, request_tid, from_client) < 0) {
                    std::cerr << getUTCTime() + RED << " [ERROR] Error updating client stream!" << RESET << '\n';
                }

                while (!(_manager.find_response_candidate(&response_token))) {
                    // std::cout << "Response token chosen: " << response_token << std::endl;
                    if (_manager.get_client_connection_info(response_token, &response_fd, &response_addr, &response_addr_len)) {
                        std::cerr << "Error getting connection info" << '\n';
                        break; // TODO
                    }
                    to_client = _manager.getOtherClientStreams(response_token);
                    response_tid = _manager.getClientResponseTID(response_token);
                    _manager.get_client_flag(response_token, &response_flag);

                    // Profiler::add_record(t, request_tid, Profiler::OperationID::RESPONSE_END);
                    if (response_tid > 0) {
                        TCPSegment& response_segment = request.refer_writable_segment();
                        response_segment.buildAudioStream(response_token, response_tid, to_client, response_flag);
                        // Profiler::add_record(t, response_tid, Profiler::OperationID::DISPATCH_PENDING);

                        // std::cout << "Response size: " << response_segment.size() << '\n';
                        int bytes = send(response_fd, static_cast<const uint8_t*>(response_segment.rawBuffer()), response_segment.size(), 0);
                        if (bytes < 0) {
                            perror("send()");
                            if (errno == EINTR) {
                                // Do nothing...
                            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                // Do nothing...
                            } else {
                                shutdown(response_fd, SHUT_RDWR);
                                close(response_fd);
                            }
                        }
                        // if (bytes != request_segment.size()) {
                        //     std::cout << "bytes: " << bytes << std::endl;
                        // }
                    }
                }
            }
        } else {
            std::cerr << "Request from invalid token (" << t << "). Skipping it." << '\n';
        }
    } else {
        std::cerr << getUTCTime() + RED << " [ERROR] Not consistent header (" << static_cast<uint16_t>(request_segment.header()) << ") of TCP segment!" << RESET << '\n';
    }
}
