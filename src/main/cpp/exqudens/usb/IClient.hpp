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

            virtual size_t bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint, const unsigned int& timeout) = 0;

            virtual std::vector<unsigned char> bulkRead(const int& size, const unsigned char& endpoint, const unsigned int& timeout) = 0;
            virtual std::vector<unsigned char> bulkRead(const unsigned char& endpoint, const unsigned int& timeout) = 0;

            virtual void close() = 0;

            virtual void destroy() = 0;

            virtual ~IClient() noexcept = default;

    };

}
