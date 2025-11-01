#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <fstream>
#include <unordered_map>
#include "Profiler.h"

bool is_increasing_transaction_id(Profiler::Record& first, Profiler::Record& second) {
    return first.transaction < second.transaction;
}

bool is_increasing_operation_id(Profiler::Record& first, Profiler::Record& second) {
    return first.operation < second.operation;
}

Profiler::Record::Record():
    token(),
    transaction(),
    operation(),
    timestamp() {

}

Profiler::Record::Record(ClientToken token, ClientTID transaction, OperationID operation, Timestamp timestamp):
    token(token),
    transaction(transaction),
    operation(operation),
    timestamp(timestamp) {

}

int Profiler::add_record(ClientToken token, ClientTID transaction, OperationID operation) {
    HighResolutionTime t = HighResolutionClock::now();
    Timestamp timestamp = std::chrono::duration_cast<Nanoseconds>(t.time_since_epoch()).count();
    {
        Locker lock_guard(_mutex);
        _records.emplace_back(token, transaction, operation, timestamp);
    }
}

Profiler::Timestamp Profiler::get_timestamp(ClientToken token, ClientTID transaction, OperationID operation) {
    Locker lock_guard(_mutex);
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
    {
        Locker lock_guard(_mutex);
        for (RecordLog::iterator it = _records.begin(); it != _records.end(); it++) {
            Record record = *it;
            record_map[record.token].emplace_back(record);
        }
    }
    uint64_t to_response_pending_count = 0;
    double total_to_response_pending = 0.0;
    uint64_t to_response_start_count = 0;
    double total_to_response_start = 0.0;
    uint64_t to_response_end_count = 0;
    double total_to_response_end = 0.0;
    uint64_t to_dispatch_pending_count = 0;
    double total_to_dispatch_pending = 0.0;
    uint64_t to_dispatch_start_count = 0;
    double total_to_dispatch_start = 0.0;
    uint64_t to_udp_dispatch_count = 0;
    double total_to_udp_dispatch= 0.0;
    std::cout << std::fixed;
    for (RecordMap::iterator map_it = record_map.begin(); map_it != record_map.end(); map_it++) {
        RecordList& client_records = map_it->second;
        client_records.sort(is_increasing_operation_id);
        client_records.sort(is_increasing_transaction_id);
        Record previous_entry;
        for (RecordList::iterator list_it = client_records.begin(); list_it != client_records.end(); list_it++) {
            assert(map_it->first == list_it->token);
            double t_delta = static_cast<double>(list_it->timestamp) -
                             static_cast<double>(previous_entry.timestamp);

            if (list_it->operation == RESPONSE_PENDING) {
                if (previous_entry.operation == UDP_RECEIPT) {
                    to_response_pending_count++;
                    total_to_response_pending += t_delta;
                }
            } else if (list_it->operation == RESPONSE_START) {
                if (previous_entry.operation == RESPONSE_PENDING) {
                    to_response_start_count++;
                    total_to_response_start += t_delta;
                }
            } else if (list_it->operation == RESPONSE_END) {
                if (previous_entry.operation == RESPONSE_START) {
                    to_response_end_count++;
                    total_to_response_end += t_delta;
                }
            } else if (list_it->operation == DISPATCH_PENDING) {
                if (previous_entry.operation == RESPONSE_END) {
                    to_dispatch_pending_count++;
                    total_to_dispatch_pending += t_delta;
                }
            } else if (list_it->operation == DISPATCH_START) {
                if (previous_entry.operation == DISPATCH_PENDING) {
                    to_dispatch_start_count++;
                    total_to_dispatch_start += t_delta;
                }
            } else if (list_it->operation == UDP_DISPATCH) {
                if (previous_entry.operation == DISPATCH_START) {
                    to_udp_dispatch_count++;
                    total_to_udp_dispatch += t_delta;
                }
            }
            previous_entry = *list_it;
        }
    }
    std::cout << "UDP Receipt - Response Pending -- average: " << (total_to_response_pending / to_response_pending_count) * 1e-6 << std::endl;
    std::cout << "Response Pending - Response Start -- average: " << (total_to_response_start / to_response_start_count) * 1e-6 << std::endl;
    std::cout << "Response Start - Response End -- average: " << (total_to_response_end / to_response_end_count) * 1e-6 << std::endl;
    std::cout << "Response End - Dispatch Pending -- average: " << (total_to_dispatch_pending / to_dispatch_pending_count) * 1e-6 << std::endl;
    std::cout << "Dispatch Pending - Dispatch Start -- average: " << (total_to_dispatch_start / to_dispatch_start_count) * 1e-6 << std::endl;
    std::cout << "Dispatch Start - UDP Dispatch -- average: " << (total_to_udp_dispatch / to_udp_dispatch_count) * 1e-6 << std::endl;
}

void Profiler::export_records(std::string file_path) {
    std::ofstream log_file;
    log_file.open(file_path + "statistics.csv");
    {
        Locker lock_guard(_mutex);
        for (RecordLog::iterator it = _records.begin(); it != _records.end(); it++) {
            log_file << it->token << ',' << it->transaction << ',' << it->operation << ',' << it->timestamp << std::endl;
        }
    }
    log_file.close();
}

std::mutex Profiler::_mutex;
Profiler::RecordLog Profiler::_records;
