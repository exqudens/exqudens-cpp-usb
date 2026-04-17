#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <functional>

#include "exqudens/usb/export.hpp"

namespace exqudens::usb {

    class EXQUDENS_USB_EXPORT IClient {

        public:

            virtual std::string getLoggerId() = 0;

            virtual void setLogFunction(
                    const std::function<void(
                        const std::string& file,
                        size_t line,
                        const std::string& function,
                        const std::string& id,
                        uint16_t level,
                        const std::string& message
                    )>& value //!< A log function.
            ) = 0;

            virtual bool isSetLogFunction() = 0;

            virtual void init() = 0;

            virtual bool isInitialized() = 0;

            virtual std::string getVersion() = 0;

            /*!
            * Lists USB devices.
            *
            * @return An available USB devices, each represented by a map with keys: ["vendor", "product", "port", "bus", "address"].
            *
            * @throws std::runtime_error
            */
            virtual std::vector<std::map<std::string, uint16_t>> listDevices() = 0;

            virtual std::string toString(
                const std::map<std::string, uint16_t>& value
            ) = 0;

            virtual void open(
                const std::map<std::string, uint16_t>& value,
                const std::optional<int32_t>& interfaceNumber,
                const std::optional<bool>& detachKernelDriver
            ) = 0;

            virtual void open(
                const std::map<std::string, uint16_t>& value
            ) = 0;

            virtual bool isOpen() = 0;

            virtual std::map<std::string, uint16_t> getDevice() = 0;

            virtual uint8_t toWriteEndpoint(uint8_t endpoint) = 0;
            virtual uint8_t toReadEndpoint(uint8_t endpoint) = 0;

            virtual size_t bulkWrite(const std::vector<uint8_t>& value, uint8_t endpoint, uint32_t timeout, bool autoEndpointDirection) = 0;
            virtual size_t bulkWrite(const std::vector<uint8_t>& value, uint8_t endpoint, uint32_t timeout) = 0;
            virtual size_t bulkWrite(const std::vector<uint8_t>& value, uint8_t endpoint) = 0;

            virtual std::vector<uint8_t> bulkRead(uint8_t endpoint, uint32_t timeout, const int& size, bool autoEndpointDirection) = 0;
            virtual std::vector<uint8_t> bulkRead(uint8_t endpoint, uint32_t timeout, const int& size) = 0;
            virtual std::vector<uint8_t> bulkRead(uint8_t endpoint, uint32_t timeout) = 0;
            virtual std::vector<uint8_t> bulkRead(uint8_t endpoint) = 0;

            virtual void close() = 0;

            virtual void destroy() = 0;

            virtual ~IClient() noexcept = default;

    };

}
