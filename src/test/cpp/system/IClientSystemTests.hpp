#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "TestLog.hpp"
#include "exqudens/usb/ClientFactory.hpp"

namespace exqudens::usb {

    class IClientSystemTests: public testing::Test {

        protected:

            inline static const char* LOGGER_ID = "exqudens.usb.IClientSystemTests";

        public:

            static void log(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            ) {
                TestLog::Writer(file, line, function, id, level) << message;
            }

    };

    TEST_F(IClientSystemTests, test1) {
        try {
            std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
            std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
            TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' start";

            std::shared_ptr<IClient> client = nullptr;
            std::vector<std::map<std::string, unsigned short>> devices = {};
            std::map<std::string, unsigned short> device = {};

            client = ClientFactory::createShared(&IClientSystemTests::log, true, true);
            devices = client->listDevices();
            for (size_t i = 0; i < devices.size(); i++) {
                TEST_LOG_I(LOGGER_ID) << client->toString(devices.at(i));
                if (devices.at(i).at("vendor") == 0x0483 && devices.at(i).at("product") == 0x5741) {
                    device = devices.at(i);
                    break;
                }
            }
            ASSERT_FALSE(device.empty());

            client->open(device, true, true);
            ASSERT_TRUE(client->isOpen());

            std::string data = "abc";
            std::vector<unsigned char> bytes = std::vector<unsigned char>(data.begin(), data.end());
            size_t size = client->bulkWrite(bytes, 1, 1000);
            TEST_LOG_I(LOGGER_ID) << "size: " << size;

            bytes = client->bulkRead(1 | 0x80, 1000);
            data = std::string(bytes.begin(), bytes.end());
            TEST_LOG_I(LOGGER_ID) << "data: '" << data << "'";

            client->close();
            ASSERT_FALSE(client->isOpen());

            TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
        } catch (const std::exception& e) {
            std::string errorMessage = TestUtils::toString(e);
            TEST_LOG_E(LOGGER_ID) << errorMessage;
            FAIL() << errorMessage;
        }
    }

}
