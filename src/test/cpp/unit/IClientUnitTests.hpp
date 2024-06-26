#pragma once

#include <string>
#include <memory>
#include <filesystem>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "TestLog.hpp"
#include "exqudens/usb/ClientFactory.hpp"

namespace exqudens::usb {

    class IClientUnitTests: public testing::Test {

        protected:

            inline static const char* LOGGER_ID = "exqudens.usb.IClientUnitTests";

    };

    TEST_F(IClientUnitTests, test1) {
        try {
            std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
            std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
            TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' start";

            std::filesystem::path file = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path().parent_path() / "name-version.txt";
            TEST_LOG_I(LOGGER_ID) << "file: '" << file.generic_string() << "'";
            std::string expected = TestUtils::readFileString(file.generic_string());
            expected = TestUtils::split(expected, ":").at(1);
            expected = TestUtils::trim(expected);
            TEST_LOG_I(LOGGER_ID) << "expected: '" << expected << "'";

            std::shared_ptr<IClient> client = ClientFactory::createShared();
            std::string actual = client->getVersion();
            TEST_LOG_I(LOGGER_ID) << "actual: '" << actual << "'";

            ASSERT_EQ(expected, actual);

            TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
        } catch (const std::exception& e) {
            std::string errorMessage = TestUtils::toString(e);
            TEST_LOG_E(LOGGER_ID) << errorMessage;
            FAIL() << errorMessage;
        }
    }

}
