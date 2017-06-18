
#include "algorithms/FEAL.hpp"
#include "util/Logging.hpp"

int main()
{
    crypto::initLogging();

    CRYPTO_LOG_DEBUG(crypto::g_logger::get()) << "Program started";

    crypto::algorithms::FEAL feal(0xABCDEFABCDEFABCD);

    std::string myString;

    while (true)
    {
        std::cout << "Enter string to encrypt: ";
        std::getline(std::cin, myString);
        std::cout << std::endl;
        
        if (myString.length() % feal.getBlockSize() != 0 || myString.length() == 0)
        {
            auto spaceToFill = feal.getBlockSize() - myString.length() % feal.getBlockSize();
            for (auto i = 0; i < spaceToFill; ++i)
            {
                myString += static_cast<char>(myString.length());
            }
        }

        std::vector<char> myData(myString.c_str(), myString.c_str() + myString.length() + 1);

        auto encryptionPos = myData.begin();
        while (encryptionPos < myData.end() - feal.getBlockSize())
        {
            encryptionPos = feal.encrypt(myData, encryptionPos);
        }

        std::string myEncrypted(myData.begin(), myData.end());
        std::cout << "Encrypted: " << myEncrypted << std::endl;

        auto decryptionPos = myData.begin();
        while (decryptionPos < myData.end() - feal.getBlockSize())
        {
            decryptionPos = feal.decrypt(myData, decryptionPos);
        }

        std::string myDecrypted(myData.begin(), myData.end());
        std::cout << "Decrypted: " << myDecrypted << std::endl;
    }

    return 0;
}
