#include "algorithms/FEALLinearCA.hpp"
#include "util/Logging.hpp"

namespace crypto { namespace algorithms
{
void FEALLinearCA::addPlaintextCiphertextPair(const PlaintextCiphertextPair& pair) noexcept
{ 
    m_plaintextCiphertextPairs.push_back(pair);
    add_pcPairU64(pair);
}

uint64_t FEALLinearCA::breakCipher() 
{
    calculate_k1_k4_candidates();

    for (auto k1 : m_k1Candidates)
    {
        for (auto k4 : m_k4Candidates)
        {
            for (auto pcPair : m_pcPairsU64)
            {
                auto leftPT = static_cast<uint32_t>(pcPair.first >> 32);
                auto rightPT = static_cast<uint32_t>(pcPair.first);

                auto left = static_cast<uint32_t>(pcPair.second >> 32);
                auto right = static_cast<uint32_t>(pcPair.second);

                // calculate forward one round.
                rightPT ^= leftPT;

                leftPT ^= FEAL::roundFunction(rightPT, k1);

                std::swap(leftPT, rightPT);

                // calculate backwards one round.
                right ^= left;

                left ^= FEAL::roundFunction(right, k4);

                std::swap(left, right);
                
                try
                {
                    auto k2 = bruteForceRound(rightPT, leftPT ^ right);
                    auto k3 = bruteForceRound(right, left ^ rightPT);

                    uint64_t key = k1;
                    key <<= 16;

                    key |= k2;
                    key <<= 16;

                    key |= k3;
                    key <<= 16;

                    key |= k4;

                    if (isCorrectKey(key))
                    {
                        CRYPTO_LOG_DEBUG(g_logger::get()) << "Key found: " << std::hex << key;
                        return key;
                    }
                }
                catch(std::runtime_error e)
                {
                    break;
                }
            }
        }
    }

    throw std::runtime_error("Could not break cipher!");
}

void FEALLinearCA::reset() noexcept
{
    m_pcPairsU64.clear();
    m_k1Candidates.clear();
    m_k4Candidates.clear();
    m_plaintextCiphertextPairs.clear();
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

void FEALLinearCA::calculateCandidates(bool flipIO, std::unordered_set<uint16_t>& candidateSet) noexcept 
{
    for (uint16_t key = 0; key < 0xFFFF; ++key)
    {
        auto candidate = true;
        auto characteristic2Set = false;
        auto characteristic3Set = false;
        auto characteristic4Set = false;
        uint8_t characteristic2 = 0;
        uint8_t characteristic3 = 0;
        uint8_t characteristic4 = 0;
        // calculate backwards one round.
        for (auto pcPair : m_pcPairsU64)
        {
            auto leftPT = static_cast<uint32_t>(pcPair.first >> 32);
            auto rightPT = static_cast<uint32_t>(pcPair.first);

            auto left = static_cast<uint32_t>(pcPair.second >> 32);
            auto right = static_cast<uint32_t>(pcPair.second);

            if (flipIO)
            {
                std::swap(leftPT, left);
                std::swap(rightPT, right);
            }

            right ^= left;

            left ^= FEAL::roundFunction(right, key);

            // calculate and check characteristics
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

            uint8_t newCharacteristic2 =
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

            if (characteristic2Set)
            {
                if (characteristic2 != newCharacteristic2)
                {
                    candidate = false;
                    break;
                }
            }
            else
            {
                characteristic2Set = true;
                characteristic2 = newCharacteristic2;
            }

            // is equal to k_0[8] ^ k_2[8] = const
            uint8_t newCharacteristic3 =
                bitAtu32(leftPT, 18) ^
                bitAtu32(right, 18) ^
                bitAtu32(left, 16) ^
                bitAtu32(left, 24) ^
                bitAtu32(leftPT, 8) ^
                bitAtu32(right, 8) ^
                bitAtu32(leftPT, 16) ^
                bitAtu32(rightPT, 16) ^
                bitAtu32(leftPT, 24) ^
                bitAtu32(rightPT, 24);

            if (characteristic3Set)
            {
                if (characteristic3 != newCharacteristic3)
                {
                    candidate = false;
                    break;
                }
            }
            else
            {
                characteristic3Set = true;
                characteristic3 = newCharacteristic3;
            }

            uint8_t newCharacteristic4 =
                bitAtu32(leftPT, 26) ^
                bitAtu32(right, 26) ^
                bitAtu32(left, 24) ^
                bitAtu32(leftPT, 24) ^
                bitAtu32(rightPT, 24) ^
                bitAtu32(leftPT, 16) ^
                bitAtu32(right, 16);

            if (characteristic4Set)
            {
                if (characteristic4 != newCharacteristic4)
                {
                    candidate = false;
                    break;
                }
            }
            else
            {
                characteristic4Set = true;
                characteristic4 = newCharacteristic4;
            }
        }

        if (candidate)
        {
            CRYPTO_LOG_DEBUG(g_logger::get()) << "Found key candidate: " << std::hex << key;
            candidateSet.insert(key);
        }
    }
}

void FEALLinearCA::calculate_k1_k4_candidates() noexcept 
{
    m_k1Candidates.clear();
    m_k4Candidates.clear();

    CRYPTO_LOG_DEBUG(g_logger::get()) << "Calculating k1 candidates...";
    calculateCandidates(true, m_k1Candidates);
    CRYPTO_LOG_DEBUG(g_logger::get()) << "Calculating k4 candidates...";
    calculateCandidates(false, m_k4Candidates);
}

uint16_t FEALLinearCA::bruteForceRound(uint32_t in, uint32_t out) 
{

    for (uint16_t key = 0; key < static_cast<uint16_t>(-1); ++key)
    {
        if (FEAL::roundFunction(in, key) == out)
        {
            CRYPTO_LOG_DEBUG(g_logger::get()) << "Found matching key: " << std::hex << key;
            return key;
        }
    }
    
    throw std::runtime_error("No key found. Wrong candidate.");
}

bool FEALLinearCA::isCorrectKey(uint64_t key) noexcept
{
    FEAL feal{key};

    auto pcPairs = m_plaintextCiphertextPairs;

    for (auto pcPair : pcPairs)
    {
        feal.encrypt(pcPair.first, pcPair.first.begin());
        
        if (pcPair.first != pcPair.second)
        {
            return false;
        }
    }

    return true;
}
}}
