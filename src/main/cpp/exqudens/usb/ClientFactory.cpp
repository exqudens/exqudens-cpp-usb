#include <filesystem>
#include <stdexcept>

#include "exqudens/usb/ClientFactory.hpp"
#include "exqudens/usb/Client.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::usb {

    std::shared_ptr<IClient> ClientFactory::createShared(
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
    ) {
        try {
            std::shared_ptr<IClient> result(new Client(autoInit, autoClose, logFunction));
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::shared_ptr<IClient> ClientFactory::createShared(
        const bool& autoInit,
        const bool& autoClose
    ) {
        try {
            return createShared(autoInit, autoClose, {});
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::shared_ptr<IClient> ClientFactory::createShared() {
        try {
            return createShared(true, true, {});
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
