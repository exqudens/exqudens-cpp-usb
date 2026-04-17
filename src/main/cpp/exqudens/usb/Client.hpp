#pragma once

#include <cstddef>

#include <libusb.h>

#include "exqudens/usb/IClient.hpp"

namespace exqudens::usb {

    class EXQUDENS_USB_EXPORT Client: public virtual IClient {

        public:

            inline static const char* LOGGER_ID = "exqudens.usb.Client";

        private:

            std::function<void(
                const std::string& file,
                size_t line,
                const std::string& function,
                const std::string& id,
                uint16_t level,
                const std::string& message
            )> logFunction;
            bool autoInit = false;
            bool autoClose = false;
            std::map<std::string, uint16_t> device = {};
            libusb_context* context = nullptr;
            bool attachKernelDriver = false;
            std::optional<int32_t> interfaceNumber = {};
            libusb_device_handle* handle = nullptr;

        public:

            Client(
                bool autoInit,
                bool autoClose,
                const std::function<void(
                    const std::string& file,
                    size_t line,
                    const std::string& function,
                    const std::string& id,
                    uint16_t level,
                    const std::string& message
                )>& logFunction
            );
            Client(
                bool autoInit,
                bool autoClose
            );
            Client();

            std::string getLoggerId() override;

            void setLogFunction(
                    const std::function<void(
                        const std::string& file,
                        size_t line,
                        const std::string& function,
                        const std::string& id,
                        uint16_t level,
                        const std::string& message
                    )>& value //!< A log function.
            ) override;

            bool isSetLogFunction() override;

            void init() override;

            bool isInitialized() override;

            std::string getVersion() override;

            std::vector<std::map<std::string, uint16_t>> listDevices() override;

            std::string toString(const std::map<std::string, uint16_t>& value) override;

            void open(const std::map<std::string, uint16_t>& value, const std::optional<int32_t>& interfaceNumber, const std::optional<bool>& detachKernelDriver) override;
            void open(const std::map<std::string, uint16_t>& value) override;

            bool isOpen() override;

            std::map<std::string, uint16_t> getDevice() override;

            uint8_t toWriteEndpoint(uint8_t endpoint) override;
            uint8_t toReadEndpoint(uint8_t endpoint) override;

            size_t bulkWrite(const std::vector<uint8_t>& value, uint8_t endpoint, uint32_t timeout, bool autoEndpointDirection) override;
            size_t bulkWrite(const std::vector<uint8_t>& value, uint8_t endpoint, uint32_t timeout) override;
            size_t bulkWrite(const std::vector<uint8_t>& value, uint8_t endpoint) override;

            std::vector<uint8_t> bulkRead(uint8_t endpoint, uint32_t timeout, int32_t size, bool autoEndpointDirection) override;
            std::vector<uint8_t> bulkRead(uint8_t endpoint, uint32_t timeout, int32_t size) override;
            std::vector<uint8_t> bulkRead(uint8_t endpoint, uint32_t timeout) override;
            std::vector<uint8_t> bulkRead(uint8_t endpoint) override;

            void close() override;

            void destroy() override;

            ~Client() noexcept override;

        private:

            std::map<std::string, uint16_t> toMap(libusb_device* libusbDevice);

            void log(
                const std::string& file,
                size_t line,
                const std::string& function,
                const std::string& id,
                uint16_t level,
                const std::string& message
            );

    };

}
