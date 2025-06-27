#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <exqudens/Log.hpp>

#include "TestUtils.hpp"
#include "exqudens/usb/ClientFactory.hpp"

namespace exqudens::usb {

    class IClientUnitTests: public testing::Test {

        protected:

            inline static const char* LOGGER_ID = "IClientUnitTests";

            inline static std::vector<std::map<std::string, std::string>> events = {};

            static void log(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            ) {
                std::map<std::string, std::string> event = {
                    {"file", file},
                    {"line", std::to_string(line)},
                    {"function", function},
                    {"id", id},
                    {"level", std::to_string(level)},
                    {"message", message}
                };
                events.emplace_back(event);
            }

    };

    TEST_F(IClientUnitTests, test1) {
        try {
            std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
            std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "'" << testGroup << "." << testCase << "' bgn";

            std::shared_ptr<IClient> client = {};
            std::string expected = {};
            std::string actual = {};

            client = ClientFactory::createShared();

            client->setLogFunction(log);
            EXQUDENS_LOG_INFO(LOGGER_ID) << "client.isSetLogFunction: " << client->isSetLogFunction();

            ASSERT_TRUE(client->isSetLogFunction());

            expected = TestUtils::readFileString((std::filesystem::path(TestUtils::getProjectSourceDir()) / "name-version.txt").generic_string());
            expected = TestUtils::split(expected, ":").at(1);
            expected = TestUtils::trim(expected);
            EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: '" << expected << "'";

            actual = client->getVersion();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: '" << actual << "'";

            ASSERT_EQ(expected, actual);

            EXQUDENS_LOG_INFO(LOGGER_ID) << "events.size: " << events.size();

            ASSERT_FALSE(events.empty());

            actual = events.front().at("message");
            EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: '" << actual << "'";

            ASSERT_EQ(expected, actual);

            EXQUDENS_LOG_INFO(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
        } catch (const std::exception& e) {
            std::string errorMessage = TestUtils::toString(e);
            EXQUDENS_LOG_ERROR(LOGGER_ID) << errorMessage;
            FAIL() << errorMessage;
        }
    }

}
