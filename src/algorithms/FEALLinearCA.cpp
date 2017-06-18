#include "algorithms/FEALLinearCA.hpp"
#include "util/Logging.hpp"

namespace crypto { namespace algorithms
{
void FEALLinearCA::addPlaintextCiphertextPair(const PlaintextCiphertextPair& pair) noexcept
{ 
    m_plaintextCiphertextPairs.push_back(pair);
    add_pcPairU64(pair);
}

uint64_t FEALLinearCA::breakCipher() noexcept 
{
    calculate_k4_candidates();

    return 0;
}

void FEALLinearCA::add_pcPairU64(const PlaintextCiphertextPair& pair) noexcept 
{
    auto plainText = pair.first.begin();
    auto cipherText = pair.second.begin();

    uint64_t plainTextu64 = 0;
    uint64_t cipherTextu64 = 0;

    for(auto i = 0; i < 8; ++i)
    {
        plainTextu64 <<= 8;
        plainTextu64 |= static_cast<uint8_t>(*plainText);
        ++plainText;

        cipherTextu64 <<= 8;
        cipherTextu64 |= static_cast<uint8_t>(*cipherText);
        ++cipherText;
    }

    m_pcPairsU64.push_back({plainTextu64, cipherTextu64});
}

uint8_t bitAtu32(uint32_t value, size_t i)
{
    return (value >> i) & 1;
}

void FEALLinearCA::calculate_k4_candidates() noexcept 
{
    // For each key k4 we need to calculate backwards one round
    // for each pc pair.
    
    // Iterate over all k4
    for (uint16_t k4 = 0; k4 < 0xFFFF; ++k4)
    {
        auto candidate = true;
        auto characteristicSet = false;
        uint8_t characteristic2 = 0;
        // calculate backwards one round.
        for (auto pcPair : m_pcPairsU64)
        {
            auto leftPT = static_cast<uint32_t>(pcPair.first >> 32);
            auto rightPT = static_cast<uint32_t>(pcPair.first);

            auto left = static_cast<uint32_t>(pcPair.second >> 32);
            auto right = static_cast<uint32_t>(pcPair.second);

            right ^= left;

            left ^= FEAL::roundFunction(right, k4);

            // calculate and check characteristic
            uint8_t characteristic =
                bitAtu32(leftPT, 0) ^
                bitAtu32(leftPT, 2) ^
                bitAtu32(leftPT, 8) ^
                bitAtu32(rightPT, 0) ^
                bitAtu32(right, 2) ^
                bitAtu32(right, 8) ^
                bitAtu32(left, 0);

            if (characteristic != 0)
            {
                candidate = false;
                break;
            }

            uint8_t newCharacteristic =
                bitAtu32(leftPT, 10) ^
                bitAtu32(right, 10) ^
                bitAtu32(leftPT, 0) ^
                bitAtu32(rightPT, 0) ^
                bitAtu32(leftPT, 8) ^
                bitAtu32(rightPT, 8) ^
                bitAtu32(leftPT, 16) ^
                bitAtu32(rightPT, 16) ^
                bitAtu32(leftPT, 24) ^
                bitAtu32(rightPT, 24) ^
                bitAtu32(left, 0) ^
                bitAtu32(left, 8) ^
                bitAtu32(left, 16) ^
                bitAtu32(left, 24);

            if (characteristicSet)
            {
                if (characteristic2 != newCharacteristic)
                {
                    candidate = false;
                    break;
                }
            }
            else
            {
                characteristicSet = true;
                characteristic2 = newCharacteristic;
            }
        }

        if (candidate)
        {
            CRYPTO_LOG_DEBUG(g_logger::get()) << "Found key candidate: " << std::hex << k4;
            m_k4Candidates.insert(k4);
        }
    }
}
}}
