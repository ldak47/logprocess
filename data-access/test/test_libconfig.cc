#include "glog/logging.h"
#include "gtest/gtest.h"
#include "libconfig.h++"
#include "range_iterator.h"
#include <string>

TEST (test, test_libconfig) {
    libconfig::Config cfg;
    try {
        cfg.readFile("example.cfg");
    } catch (const libconfig::FileIOException &fioerr) {
        LOG(WARNING) << "I/O error while reading file example.cfg";
        return;
    } catch (const libconfig::ParseException &perr) {
        LOG(WARNING) << "Parse error at " << perr.getFile() << ":" << perr.getLine() << "--" << perr.getError();
        return;
    }

    const libconfig::Setting &root = cfg.getRoot();
    const libconfig::Setting &inventory = root["inventory"];
    const libconfig::Setting &books_s = inventory["books"];
    const libconfig::Setting &books = root["inventory"]["books"];
    int count = books.getLength();
    for (auto i: common::Range(0, count)) {
        libconfig::Setting &book = books[i];
        std::string exist = "", notexist = "";
        book.lookupValue("title", exist);
        LOG(INFO) << "title: " << exist;
        book.lookupValue("balabala", notexist);
        LOG(INFO) << "balabala: " << notexist;
    }
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
