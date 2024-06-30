#pragma once

#include <cstddef>
#include <string>
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
                        const size_t& line,
                        const std::string& function,
                        const std::string& id,
                        const unsigned short& level,
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
            virtual std::vector<std::map<std::string, unsigned short>> listDevices() = 0;

            virtual std::string toString(
                const std::map<std::string, unsigned short>& value
            ) = 0;

            virtual void open(
                const std::map<std::string, unsigned short>& value,
                const bool& detachKernelDriverIfActive,
                const bool& claimInterface
            ) = 0;

            virtual void open(
                const std::map<std::string, unsigned short>& value
            ) = 0;

            virtual bool isOpen() = 0;

            virtual std::map<std::string, unsigned short> getDevice() = 0;

            virtual unsigned char toWriteEndpoint(const unsigned char& endpoint) = 0;
            virtual unsigned char toReadEndpoint(const unsigned char& endpoint) = 0;

            virtual size_t bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint, const unsigned int& timeout, const bool& autoEndpointDirection) = 0;
            virtual size_t bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint, const unsigned int& timeout) = 0;
            virtual size_t bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint) = 0;

            virtual std::vector<unsigned char> bulkRead(const unsigned char& endpoint, const unsigned int& timeout, const int& size, const bool& autoEndpointDirection) = 0;
            virtual std::vector<unsigned char> bulkRead(const unsigned char& endpoint, const unsigned int& timeout, const int& size) = 0;
            virtual std::vector<unsigned char> bulkRead(const unsigned char& endpoint, const unsigned int& timeout) = 0;
            virtual std::vector<unsigned char> bulkRead(const unsigned char& endpoint) = 0;

            virtual void close() = 0;

            virtual void destroy() = 0;

            virtual ~IClient() noexcept = default;

    };

}
