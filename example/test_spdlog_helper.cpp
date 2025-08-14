#include "spdlog_helper.h"

int main(int argc, char *argv[]) {
    init_spdlog_async();

    LOG(INFO) << "SPDLOG_INFO() called!";
    LOG(WARNING) << "SPDLOG_WARN() called!";
    LOG(ERROR) << "SPDLOG_ERROR() called!";
    // LOG(FATAL) << "SPDLOG_CRITICAL() called!";

    return 0;
}