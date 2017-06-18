
#include "util/Logging.hpp"

int main()
{
    crypto::initLogging();

    CRYPTO_LOG_DEBUG(crypto::g_logger::get()) << "Program started";


    //boost::log::core::get()->remove_all_sinks();

    return 0;
}