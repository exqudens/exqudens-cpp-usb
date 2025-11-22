#include <filesystem>
#include <stdexcept>
#include <limits>

#include "exqudens/usb/Client.hpp"
#include "exqudens/usb/versions.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOGGER_LEVEL_ERROR 2
#define LOGGER_LEVEL_DEBUG 5

namespace exqudens::usb {

    Client::Client(
        const bool& autoInit,
        const bool& autoClose,
        const std::function<void(
            const std::string& file,
            const size_t& line,
            const std::string& function,
            const std::string& id,
            const unsigned short& level,
            const std::string& message
        )>& logFunction
    ):
        autoInit(autoInit),
        autoClose(autoClose),
        logFunction(logFunction)
    {
        if (autoInit) {
            init();
        }
    }

    Client::Client(const bool& autoInit, const bool& autoClose): Client(autoInit, autoClose, {}) {}

    Client::Client(): Client(true, true, {}) {}

    std::string Client::getLoggerId() {
        try {
            return std::string(LOGGER_ID);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void Client::setLogFunction(
            const std::function<void(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            )>& value //!< A log function.
    ) {
        try {
            logFunction = value;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    bool Client::isSetLogFunction() {
        try {
            return (bool) logFunction;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void Client::init() {
        try {
            if (context == nullptr) {
                int libusbError = libusb_init(&context);
                if (libusbError) {
                    const char* libusbErrorName = libusb_error_name(libusbError);
                    throw std::runtime_error(CALL_INFO + ": libusbErrorName: '" + std::string(libusbErrorName) + "'");
                }
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    bool Client::isInitialized() {
        try {
            if (context == nullptr) {
                return false;
            } else {
                return true;
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::string Client::getVersion() {
        try {
            std::string result = std::to_string(PROJECT_VERSION_MAJOR);
            result += ".";
            result += std::to_string(PROJECT_VERSION_MINOR);
            result += ".";
            result += std::to_string(PROJECT_VERSION_PATCH);
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_DEBUG, result);
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::vector<std::map<std::string, unsigned short>> Client::listDevices() {
        try {
            std::vector<std::map<std::string, unsigned short>> result = {};
            libusb_device** libusbDevices;
            ssize_t libusbDevicesSize = libusb_get_device_list(context, &libusbDevices);
            for (ssize_t i = 0; i < libusbDevicesSize; i++) {
                libusb_device* libusbDevice = libusbDevices[i];
                std::map<std::string, unsigned short> entry = toMap(libusbDevice);
                result.emplace_back(entry);
            }
            libusb_free_device_list(libusbDevices, 1);
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::string Client::toString(const std::map<std::string, unsigned short>& value) {
        try {
            std::string result = "";
            std::vector<std::string> parts = {};
            for (const auto& entry : value) {
                std::string k = entry.first;
                unsigned short v = entry.second;
                std::string s = k + ": " + std::to_string(v);
                parts.emplace_back(s);
            }
            for (size_t i = 0; i < parts.size(); i++) {
                if (i == 0) {
                    result += parts.at(i);
                } else {
                    result += std::string(", ") + parts.at(i);
                }
            }
            result = std::string("{") + result + "}";
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void Client::open(const std::map<std::string, unsigned short>& value, const bool& detachKernelDriverIfActive, const bool& claimInterface) {
        try {
            if (isOpen()) {
                throw std::runtime_error(CALL_INFO + ": the device is already open! call 'close' before...");
            }

            std::map<std::string, unsigned short> deviceForOpen = value;

            if (deviceForOpen.empty()) {
                std::vector<std::map<std::string, unsigned short>> devices = listDevices();
                if (devices.empty()) {
                    throw std::runtime_error(CALL_INFO + ": unable to find compatibale devices!");
                }
                if (devices.size() > 1) {
                    std::string errorMessage = CALL_INFO + "Found more than one compatibale device:\n";
                    for (size_t i = 0; i < devices.size(); i++) {
                        errorMessage += toString(devices.at(i)) + "\n";
                    }
                    errorMessage += "use 'open' with not empty map argument!";
                    throw std::runtime_error(errorMessage);
                }
                deviceForOpen = devices.front();
            }

            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_DEBUG, "selected device: " + toString(deviceForOpen));

            libusb_device** libusbDevices;
            ssize_t libusbDevicesSize = libusb_get_device_list(context, &libusbDevices);
            for (ssize_t i = 0; i < libusbDevicesSize; i++) {
                libusb_device* libusbDevice = libusbDevices[i];
                std::map<std::string, unsigned short> entry = toMap(libusbDevice);
                if (entry == deviceForOpen) {
                    int libusbError = libusb_open(libusbDevice, &handle);
                    if (libusbError != 0) {
                        libusb_free_device_list(libusbDevices, 1);
                        const char* libusbErrorName = libusb_error_name(libusbError);
                        throw std::runtime_error(CALL_INFO + ": libusbErrorName: '" + std::string(libusbErrorName) + "'");
                    }
                    break;
                }
            }
            libusb_free_device_list(libusbDevices, 1);

            if (!isOpen()) {
                throw std::runtime_error(CALL_INFO + ": device handle is null!");
            }

            if (detachKernelDriverIfActive) {
                if (libusb_kernel_driver_active(handle, 0) == 1) {
                    if (libusb_detach_kernel_driver(handle, 0) != 0) {
                        close();
                        throw std::runtime_error(CALL_INFO + ": unable to detach kernel driver!");
                    }
                }
            }

            if (claimInterface) {
                int libusbError = libusb_claim_interface(handle, 0);
                if (libusbError) {
                    const char* libusbErrorName = libusb_error_name(libusbError);
                    throw std::runtime_error(CALL_INFO + ": libusbErrorName: '" + std::string(libusbErrorName) + "'");
                }
            }

            device = deviceForOpen;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void Client::open(const std::map<std::string, unsigned short>& value) {
        try {
            open(value, false, false);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    bool Client::isOpen() {
        try {
            if (handle == nullptr) {
                return false;
            } else {
                return true;
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::map<std::string, unsigned short> Client::getDevice() {
        try {
            return device;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    unsigned char Client::toWriteEndpoint(const unsigned char& endpoint) {
        try {
            return endpoint | LIBUSB_ENDPOINT_OUT;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    unsigned char Client::toReadEndpoint(const unsigned char& endpoint) {
        try {
            return endpoint | LIBUSB_ENDPOINT_IN;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    size_t Client::bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint, const unsigned int& timeout, const bool& autoEndpointDirection) {
        try {
            if (value.size() > INT_MAX) {
                throw std::runtime_error(CALL_INFO + ": value.size: " + std::to_string(value.size()) + " greater than INT_MAX: " + std::to_string(INT_MAX));
            }
            int libusbBulkTransfered = 0;
            std::vector<unsigned char>& data = const_cast<std::vector<unsigned char>&>(value);
            int libusbError = libusb_bulk_transfer(
                handle,
                (autoEndpointDirection ? toWriteEndpoint(endpoint) : endpoint),
                data.data(),
                (int) data.size(),
                &libusbBulkTransfered,
                timeout
            );
            if (libusbError) {
                const char* libusbErrorName = libusb_error_name(libusbError);
                throw std::runtime_error(CALL_INFO + ": libusbErrorName: '" + std::string(libusbErrorName) + "'");
            }
            if (libusbBulkTransfered < 0) {
                throw std::runtime_error(CALL_INFO + ": libusbBulkTransfered: " + std::to_string(libusbBulkTransfered) + " less zero");
            }
            size_t result = (size_t) libusbBulkTransfered;
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    size_t Client::bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint, const unsigned int& timeout) {
        try {
            return bulkWrite(value, endpoint, timeout, true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    size_t Client::bulkWrite(const std::vector<unsigned char>& value, const unsigned char& endpoint) {
        try {
            return bulkWrite(value, endpoint, 1000, true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::vector<unsigned char> Client::bulkRead(const unsigned char& endpoint, const unsigned int& timeout, const int& size, const bool& autoEndpointDirection) {
        try {
            if (size < 0) {
                throw std::runtime_error(CALL_INFO + ": size: " + std::to_string(size) + " less zero");
            }
            std::vector<unsigned char> result = {};
            result.resize(size);
            int libusbBulkTransfered = 0;
            int libusbError = libusb_bulk_transfer(
                handle,
                (autoEndpointDirection ? toReadEndpoint(endpoint) : endpoint),
                result.data(),
                (int) result.size(),
                &libusbBulkTransfered,
                timeout
            );
            if (libusbError) {
                const char* libusbErrorName = libusb_error_name(libusbError);
                throw std::runtime_error(CALL_INFO + ": libusbErrorName: '" + std::string(libusbErrorName) + "'");
            }
            if (libusbBulkTransfered < 0) {
                throw std::runtime_error(CALL_INFO + ": libusbBulkTransfered: " + std::to_string(libusbBulkTransfered) + " less zero");
            }
            result.resize(libusbBulkTransfered);
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::vector<unsigned char> Client::bulkRead(const unsigned char& endpoint, const unsigned int& timeout, const int& size) {
        try {
            return bulkRead(endpoint, timeout, size, true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::vector<unsigned char> Client::bulkRead(const unsigned char& endpoint, const unsigned int& timeout) {
        try {
            return bulkRead(endpoint, timeout, INT_MAX, true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::vector<unsigned char> Client::bulkRead(const unsigned char& endpoint) {
        try {
            return bulkRead(endpoint, 1000, INT_MAX, true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void Client::close() {
        try {
            if (handle != nullptr) {
                libusb_close(handle);
                handle = nullptr;
            }
            device = {};
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void Client::destroy() {
        try {
            if (context != nullptr) {
                libusb_exit(context);
                context = nullptr;
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    Client::~Client() noexcept {
        if (autoClose) {
            try {
                close();
            } catch (const std::exception& e) {
                log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_ERROR, "Error in destructor on call function: 'close': '" + std::string(e.what()) + "'");
            } catch (...) {
                log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_ERROR, "Unknown error in destructor on call function: 'close'");
            }
        }
        if (autoInit) {
            try {
                destroy();
            } catch (const std::exception& e) {
                log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_ERROR, "Error in destructor on call function: 'destroy': '" + std::string(e.what()) + "'");
            } catch (...) {
                log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_ERROR, "Unknown error in destructor on call function: 'destroy'");
            }
        }
    }

    std::map<std::string, unsigned short> Client::toMap(libusb_device* libusbDevice) {
        try {
            std::map<std::string, unsigned short> result = {};

            libusb_device_descriptor libusbDeviceDescriptor = {0};
            int libusbError = libusb_get_device_descriptor(libusbDevice, &libusbDeviceDescriptor);
            if (libusbError) {
                const char* libusbErrorName = libusb_error_name(libusbError);
                throw std::runtime_error(CALL_INFO + ": libusbErrorName: '" + std::string(libusbErrorName) + "'");
            }

            unsigned short vendor = libusbDeviceDescriptor.idVendor;
            unsigned short product = libusbDeviceDescriptor.idProduct;
            unsigned short port = libusb_get_port_number(libusbDevice);
            unsigned short bus = libusb_get_bus_number(libusbDevice);
            unsigned short address = libusb_get_device_address(libusbDevice);

            result.insert({"vendor", vendor});
            result.insert({"product", product});
            result.insert({"port", port});
            result.insert({"bus", bus});
            result.insert({"address", address});

            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void Client::log(
        const std::string& file,
        const size_t& line,
        const std::string& function,
        const std::string& id,
        const unsigned short& level,
        const std::string& message
    ) {
        try {
            if (logFunction) {
                std::string internalFile = std::filesystem::path(file).filename().string();
                logFunction(internalFile, line, function, id, level, message);
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
#undef LOGGER_LEVEL_ERROR
