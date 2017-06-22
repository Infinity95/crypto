#include "algorithms/FEAL.hpp"
#include "util/Logging.hpp"
#include "algorithms/FEALLinearCA.hpp"

inline std::ostream& operator<<(std::ostream& o, const std::vector<char>& chars)
{
    for (auto c : chars)
    {
        o << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(c));
    }
    return o;
}

void string_to_vector(std::string str, std::vector<char>& array)
{
    auto length = str.length();
    // make sure the input string has an even digit numbers
    if (length % 2 == 1)
    {
        str = "0" + str;
        length++;
    }

    // allocate memory for the output array
    array.reserve(length / 2);

    std::stringstream sstr(str);
    for (int i = 0; i < length / 2; i++)
    {
        char ch1, ch2;
        sstr >> ch1 >> ch2;
        int dig1 = 0, dig2 = 0;
        if (isdigit(ch1)) dig1 = ch1 - '0';
        else if (ch1 >= 'A' && ch1 <= 'F') dig1 = ch1 - 'A' + 10;
        else if (ch1 >= 'a' && ch1 <= 'f') dig1 = ch1 - 'a' + 10;
        if (isdigit(ch2)) dig2 = ch2 - '0';
        else if (ch2 >= 'A' && ch2 <= 'F') dig2 = ch2 - 'A' + 10;
        else if (ch2 >= 'a' && ch2 <= 'f') dig2 = ch2 - 'a' + 10;
        array.push_back(dig1 * 16 + dig2);
    }
}

int main()
{
    crypto::initLogging();

    CRYPTO_LOG_DEBUG(crypto::g_logger::get()) << "Program started";

    std::string myString;
    std::cout << "enter 'breakCipher' to attempt to break a cipher or type anything else to encrypt something" << std::endl;
    std::getline(std::cin, myString);

    if (myString == "breakCipher")
    {
        std::cout << "Enter plaintext: ";
        std::getline(std::cin, myString);

        std::cout << "Enter ciphertext: ";
        std::string encrypted;
        std::getline(std::cin, encrypted);
        while (myString != "quit")
        {
            crypto::algorithms::FEAL feal{0x0};
            crypto::algorithms::FEALLinearCA linearCa;

            if (myString.length() % feal.getBlockSize() != 0 || myString.length() == 0)
            {
                auto spaceToFill = feal.getBlockSize() - myString.length() % feal.getBlockSize();
                for (auto i = 0; i < spaceToFill; ++i)
                {
                    myString += ' ';
                }
            }


            std::vector<char> myData(myString.c_str(), myString.c_str() + myString.length());
            std::vector<char> myEncrypted;

            string_to_vector(encrypted, myEncrypted);

            auto ptPos = myData.begin();
            auto encPos = myEncrypted.begin();
            while (ptPos <= myData.end() - feal.getBlockSize())
            {
                std::vector<char> plaintext{ptPos, ptPos + feal.getBlockSize()};

                std::vector<char> ciphertext{encPos, encPos + feal.getBlockSize()};
                linearCa.addPlaintextCiphertextPair({plaintext, ciphertext});

                ptPos += feal.getBlockSize();
                encPos += feal.getBlockSize();
            }

            try
            {
                std::cout << linearCa.breakCipher();
            }
            catch (std::runtime_error e)
            {
                std::cout << e.what() << std::endl;
            }

            std::cout << "Enter plaintext: ";
            std::getline(std::cin, myString);

            std::cout << "Enter ciphertext: ";
            std::getline(std::cin, encrypted);
        }
    }
    else
    {
        ////////
        std::cout << "Enter string to encrypt: ";
        std::getline(std::cin, myString);

        while (myString != std::string("quit"))
        {
            uint64_t myKey;
            {
                std::string input;

                std::cout << "Enter 8 byte hex key (i.e. 16 hex digits): ";
                std::getline(std::cin, input);
                while (input.length() != 16 || input.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos)
                {
                    std::cout << "Not a hexadecimal key with 16 digits" << std::endl;
                    std::cout << "Enter 8 byte hex key (i.e. 16 hex digits): ";
                    std::getline(std::cin, input);
                }
                std::istringstream is(input);
                is >> std::hex >> myKey;
            }

            crypto::algorithms::FEAL feal(myKey);
            crypto::algorithms::FEALLinearCA linearCa;

            std::cout << std::endl;

            if (myString.length() % feal.getBlockSize() != 0 || myString.length() == 0)
            {
                auto spaceToFill = feal.getBlockSize() - myString.length() % feal.getBlockSize();
                for (auto i = 0; i < spaceToFill; ++i)
                {
                    myString += ' ';
                }
            }

            std::vector<char> myData(myString.c_str(), myString.c_str() + myString.length());

            auto encryptionPos = myData.begin();
            while (encryptionPos <= myData.end() - feal.getBlockSize())
            {
                auto encryptedPos = encryptionPos;
                std::vector<char> plaintext{encryptionPos, encryptionPos + feal.getBlockSize()};

                encryptionPos = feal.encrypt(myData, encryptionPos);

                std::vector<char> ciphertext{encryptedPos, encryptedPos + feal.getBlockSize()};
                linearCa.addPlaintextCiphertextPair({plaintext, ciphertext});
            }

            std::cout << "Encrypted: " << myData << std::endl;

            auto decryptionPos = myData.begin();
            while (decryptionPos <= myData.end() - feal.getBlockSize())
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
    }

    return 0;
}
