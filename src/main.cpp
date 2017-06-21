
#include "algorithms/FEAL.hpp"
#include "util/Logging.hpp"
#include "algorithms/FEALLinearCA.hpp"

inline std::ostream& operator<<(std::ostream& o, const std::vector<char>& chars)
{
    for (auto c : chars)
    {
        o << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(c));
    }
    return o;
}

int main()
{
    crypto::initLogging();

    CRYPTO_LOG_DEBUG(crypto::g_logger::get()) << "Program started";

    crypto::algorithms::FEAL feal(0xc531e0ae7f460f11);

    std::string myString;
    std::cout << "Enter string to encrypt: ";
    std::getline(std::cin, myString);

    while (myString != std::string("quit"))
    {
        crypto::algorithms::FEALLinearCA linearCa;

        std::cout << std::endl;
        
        if (myString.length() % feal.getBlockSize() != 0 || myString.length() == 0)
        {
            auto spaceToFill = feal.getBlockSize() - myString.length() % feal.getBlockSize();
            for (auto i = 0; i < spaceToFill; ++i)
            {
                myString += '\0';
            }
        }

        std::vector<char> myData(myString.c_str(), myString.c_str() + myString.length() + 1);

        auto encryptionPos = myData.begin();
        while (encryptionPos < myData.end() - feal.getBlockSize())
        {
            auto encryptedPos = encryptionPos;
            std::vector<char> plaintext{encryptionPos, encryptionPos + feal.getBlockSize()};

            encryptionPos = feal.encrypt(myData, encryptionPos);

            std::vector<char> ciphertext{encryptedPos, encryptedPos + feal.getBlockSize()};
            linearCa.addPlaintextCiphertextPair({plaintext, ciphertext});
        }

        std::cout << "Encrypted: " << myData << std::endl;

        auto decryptionPos = myData.begin();
        while (decryptionPos < myData.end() - feal.getBlockSize())
        {
            decryptionPos = feal.decrypt(myData, decryptionPos);
        }

        std::string myDecrypted(myData.begin(), myData.end());
        std::cout << "Decrypted: " << myDecrypted << std::endl;

        auto key = linearCa.breakCipher();

        std::cout << "Key: " << key << std::endl;

        std::cout << "Enter string to encrypt: ";
        std::getline(std::cin, myString);
    }

    return 0;
}
