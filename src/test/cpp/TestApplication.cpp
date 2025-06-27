#include <cstddef>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestApplication.hpp"
#include "TestUtils.hpp"

// include test files
#include "unit/IClientUnitTests.hpp"
#include "system/IClientSystemTests.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

int TestApplication::run(int argc, char** argv) {
    try {
        std::vector<std::string> args = {};
        for (int i = 0; i < argc; i++) {
            std::cout << "argv[" << i << "]: '" << argv[i] << "'" << std::endl;
            args.emplace_back(argv[i]);
        }

        TestUtils::init(args);

        // logging
        std::filesystem::path executableDir(TestUtils::getExecutableDir());
        std::string loggingFile = (executableDir / "log" / "log.txt").generic_string();
        size_t loggingFileSize = 1073741824; // 1 gb
        std::map<std::string, unsigned short> loggerIdLevelMap = {
            {LOGGER_ID, 0},
            {"exqudens.usb.Client", 0},
            {"IClientUnitTests", 0},
            {"IClientSystemTests", 0}
        };
        std::string loggingConfigResult = exqudens::Log::configure(loggingFile, loggingFileSize, loggerIdLevelMap);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";
        EXQUDENS_LOG_INFO(LOGGER_ID) << "loggingConfigResult: '" << loggingConfigResult << "'";

        testing::InitGoogleMock(&argc, argv);
        testing::InitGoogleTest(&argc, argv);

        int result = RUN_ALL_TESTS();

        EXQUDENS_LOG_INFO(LOGGER_ID) << "result: " << result;

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";

        return result;
    } catch (const std::exception& e) {
        std::cerr << CALL_INFO + ": " + TestUtils::toString(e) << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << CALL_INFO + ": Unknown error!" << std::endl;
        return EXIT_FAILURE;
    }
}

#undef CALL_INFO
