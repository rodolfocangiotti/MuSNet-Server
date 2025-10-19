#ifndef PROFILER_H
#define PROFILER_H

#include <deque>
#include <mutex>
#include "types.h"

class Profiler {
private:
    enum OperationID {
        UDP_RECEIPT = 1,
        RESPONSE_PENDING,
        RESPONSE_START,
        RESPONSE_END,
        DISPATCH_PENDING,
        DISPATCH_START,
        UDP_DISPATCH,
    };
public:
    typedef OperationID OperationID;
    typedef uint64_t Timestamp;
    typedef uint64_t Duration;
    struct Record {
        Record();
        Record(ClientToken token, ClientTID transaction, OperationID operation, Timestamp timestamp);
        ClientToken token;
        ClientTID transaction;
        OperationID operation;
        Timestamp timestamp;
    };
    typedef std::deque<Record> RecordLog;
    static int add_record(ClientToken token, ClientTID transaction, OperationID operation);
    static Timestamp get_timestamp(ClientToken token, ClientTID transaction, OperationID operation);
    static void print_statistics();
    static void export_records(std::string file_path);
private:
    typedef std::lock_guard<std::mutex> Locker;
    static std::mutex _mutex;
    static RecordLog _records;
};

#endif