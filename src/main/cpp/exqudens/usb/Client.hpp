#pragma once

#include <cstddef>

#include <libusb.h>

#include "exqudens/usb/IClient.hpp"

namespace exqudens::usb {

    class EXQUDENS_USB_EXPORT Client: public virtual IClient {

        private:

            std::function<void(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            )> logFunction;
            bool autoInit = false;
            bool autoClose = false;
            std::map<std::string, unsigned short> device = {};
            libusb_context* context = nullptr;
            libusb_device_handle* handle = nullptr;

        public:

            Client(
                const std::function<void(
                    const std::string& file,
                    const size_t& line,
                    const std::string& function,
                    const std::string& id,
                    const unsigned short& level,
                    const std::string& message
                )>& logFunction,
                const bool& autoInit,
                const bool& autoClose
            );
            Client();

            void init() override;

            bool isInitialized() override;

            std::string getVersion() override;

            std::vector<std::map<std::string, unsigned short>> listDevices() override;

            std::string toString(const std::map<std::string, unsigned short>& value) override;

            void open(const std::map<std::string, unsigned short>& value, const bool& detachKernelDriverIfActive, const bool& claimInterface) override;
            void open(const std::map<std::string, unsigned short>& value) override;

            bool isOpen() override;

            size_t bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint, const unsigned int& timeout) override;

            std::vector<unsigned char> bulkRead(const int& size, const unsigned char& endpoint, const unsigned int& timeout) override;
            std::vector<unsigned char> bulkRead(const unsigned char& endpoint, const unsigned int& timeout) override;

            void close() override;

            void destroy() override;

            ~Client() noexcept override;

        private:

            std::map<std::string, unsigned short> toMap(libusb_device* libusbDevice);

            void log(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            );

    };

}