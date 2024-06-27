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
            std::vector<std::string> stackTrace = {};
            std::string data = "";
            std::vector<unsigned char> bytes = {};
            size_t size = 0;

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
            ASSERT_TRUE(device == client->getDevice());

            try {
                bytes = client->bulkRead(1);
            } catch (const std::exception& e) {
                stackTrace = TestUtils::toStackTrace(e);
            }
            TEST_LOG_I(LOGGER_ID) << "stackTrace.size: " << stackTrace.size();
            TEST_LOG_I(LOGGER_ID) << "stackTrace[0]: '" << stackTrace.at(0) << "'";

            ASSERT_TRUE(bytes.empty());
            ASSERT_FALSE(stackTrace.empty());
            ASSERT_TRUE(stackTrace.at(0).ends_with("libusbErrorName: 'LIBUSB_ERROR_TIMEOUT'"));

            stackTrace = {};
            data = "";
            bytes = {};
            size = 0;

            data = "abc";
            bytes = std::vector<unsigned char>(data.begin(), data.end());
            size = client->bulkWrite(bytes, 1);
            TEST_LOG_I(LOGGER_ID) << "size: " << size;

            ASSERT_EQ(3, size);

            stackTrace = {};
            data = "";
            bytes = {};
            size = 0;

            bytes = client->bulkRead(1);
            data = std::string(bytes.begin(), bytes.end());
            TEST_LOG_I(LOGGER_ID) << "data: '" << data << "'";

            ASSERT_EQ(std::string("ABC"), data);

            stackTrace = {};
            data = "";
            bytes = {};
            size = 0;

            try {
                bytes = client->bulkRead(1);
            } catch (const std::exception& e) {
                stackTrace = TestUtils::toStackTrace(e);
            }
            TEST_LOG_I(LOGGER_ID) << "stackTrace.size: " << stackTrace.size();
            TEST_LOG_I(LOGGER_ID) << "stackTrace[0]: '" << stackTrace.at(0) << "'";

            ASSERT_TRUE(bytes.empty());
            ASSERT_FALSE(stackTrace.empty());
            ASSERT_TRUE(stackTrace.at(0).ends_with("libusbErrorName: 'LIBUSB_ERROR_TIMEOUT'"));

            client->close();
            ASSERT_FALSE(client->isOpen());
            ASSERT_TRUE(client->getDevice().empty());

            TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
        } catch (const std::exception& e) {
            std::string errorMessage = TestUtils::toString(e);
            TEST_LOG_E(LOGGER_ID) << errorMessage;
            FAIL() << errorMessage;
        }
    }

}
