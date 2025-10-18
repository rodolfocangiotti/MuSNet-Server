#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>
#include "Profiler.h"

bool is_increasing_transaction_id(Profiler::Record& first, Profiler::Record& second) {
    return first.transaction < second.transaction;
}

bool is_increasing_operation_id(Profiler::Record& first, Profiler::Record& second) {
    return first.operation < second.operation;
}

int Profiler::add_record(ClientToken token, ClientTID transaction, OperationID operation) {
    SystemTime t = SystemClock::now();
    Timestamp timestamp = std::chrono::duration_cast<Nanoseconds>(t.time_since_epoch()).count();
    _records.emplace_back(token, transaction, operation, timestamp);
}

Profiler::Timestamp Profiler::get_timestamp(ClientToken token, ClientTID transaction, OperationID operation) {
    for (RecordLog::iterator it = _records.begin(); it != _records.end(); it++) {
        if (
            (it->token == token) &&
            (it->transaction == transaction) &&
            (it->operation == operation)    
        ) {
            return it->timestamp;
        }
    }
    return 0;
}

void Profiler::print_statistics() {
    // This function doesn't need to be optimized in terms of performance.
    typedef std::list<Record> RecordList;
    typedef std::unordered_map<ClientToken, RecordList> RecordMap;
    RecordMap record_map;
    while (_records.begin() != _records.end()) {
        Record& record = _records.back();
        record_map[record.token].emplace_back(record);
        _records.pop_back();
    }
    for (RecordMap::iterator map_it = record_map.begin(); map_it != record_map.end(); map_it++) {
        RecordList& client_records = map_it->second;
        client_records.sort(is_increasing_operation_id);
        client_records.sort(is_increasing_transaction_id);
        for (RecordList::iterator list_it = client_records.begin(); list_it != client_records.end(); list_it++) {
            assert(map_it->first == list_it->token);
            std::string readable_operation;
            if (list_it->operation == UDP_RECEIPT) {
                readable_operation = "udp_receipt";
            } else if (list_it->operation == RESPONSE_PENDING) {
                readable_operation = "response_pending";
            } else if (list_it->operation == RESPONSE_START) {
                readable_operation = "response_start";
            } else if (list_it->operation == RESPONSE_END) {
                readable_operation = "response_end";
            } else if (list_it->operation == DISPATCH_PENDING) {
                readable_operation = "dispatch_pending";
            } else if (list_it->operation == DISPATCH_START) {
                readable_operation = "dispatch_start";
            } else if (list_it->operation == UDP_DISPATCH) {
                readable_operation = "udp_dispatch";
            }
            std::cout << list_it->token << ',' << list_it->transaction << ',' << readable_operation << ',' << list_it->timestamp * 1e-9 << std::endl;
        }
    }