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
#include "TestConfig.hpp"
#include "TestLog.hpp"
#include "TestUtils.hpp"

// include test files
#include "unit/IClientUnitTests.hpp"
#include "system/IClientSystemTests.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

int TestApplication::run(int argc, char** argv) {
  try {
    if (argc <= 0) {
      throw std::runtime_error(CALL_INFO + ": argc <= 0");
    }

    std::vector<std::string> args = {};
    for (int i = 0; i < argc; i++) {
      args.emplace_back(argv[i]);
    }

    TestConfig::setExecutableFile(args.front());

    if (TestConfig::getExecutableDir().empty()) {
      throw std::runtime_error(CALL_INFO + ": TestConfig::getExecutableDir().empty()");
    }

    if (TestConfig::getExecutableFile().empty()) {
      throw std::runtime_error(CALL_INFO + ": TestConfig::getExecutableFile().empty()");
    }

    std::string configType = TestLog::config(args);

    TEST_LOG_I(LOGGER_ID) << "start config type: '" << configType << "'";

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    TEST_LOG_I(LOGGER_ID) << "end";

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
