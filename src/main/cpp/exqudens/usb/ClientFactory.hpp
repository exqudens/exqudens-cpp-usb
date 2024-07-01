#pragma once

#include <memory>

#include "exqudens/usb/IClient.hpp"

namespace exqudens::usb {

    class EXQUDENS_USB_EXPORT ClientFactory {

        public:

            static std::shared_ptr<IClient> createShared(
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
            );

            static std::shared_ptr<IClient> createShared(
                const bool& autoInit,
                const bool& autoClose
            );

            static std::shared_ptr<IClient> createShared();

    };

}
