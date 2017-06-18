#include <algorithms/FEAL.hpp>
#include <cassert>
#include "util/Logging.hpp"

namespace crypto {namespace algorithms
{
FEAL::FEAL(uint64_t key) noexcept :
    m_left{0},
    m_right{0},
    m_key{key}
{}


std::vector<char>::iterator FEAL::encrypt(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept
{
    for (auto i = 0; i < m_blockSize; ++i)
    {
        m_keys[i] = static_cast<uint16_t>(m_key >> (24 - (i * 8)));
    }
    return applyEnc(data, block);
}

std::vector<char>::iterator FEAL::decrypt(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept
{
    for (auto i = 0; i < m_blockSize; ++i)
    {
        m_keys[i] = static_cast<uint16_t>(m_key >> (i * 8));
    }
    return applyEnc(data, block);
}

size_t FEAL::getBlockSize() noexcept
{
    return m_blockSize;
}

std::vector<char>::iterator FEAL::applyEnc(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept
{
    assert(block + m_blockSize < data.end());

    copyInput(block, block + 4);

    feistel();

    copyOutput(block, block + 4);

    return block + m_blockSize;
}

uint8_t rol2(uint8_t x)
{
    return (x << 2) | ((x >> 6) & 0b00000011);
}

uint8_t FEAL::sBox(uint8_t A, uint8_t B, uint8_t x) noexcept 
{
    return rol2(A + B + x);
}

uint32_t FEAL::roundFunction(uint32_t B, uint16_t k) noexcept
{
    uint32_t result = 0;
    uint8_t B0_7 = static_cast<uint8_t>(B);
    uint8_t B8_15 = static_cast<uint8_t>(B >> 8);
    uint8_t B16_23 = static_cast<uint8_t>(B >> 16);
    uint8_t B24_31 = static_cast<uint8_t>(B >> 24);
    uint8_t k0_7 = static_cast<uint8_t>(k);
    uint8_t k8_15 = static_cast<uint8_t>(k >> 8);

    uint8_t s01_input = B16_23 ^ B24_31 ^ k8_15;

    uint8_t F8_15 = sBox(B0_7 ^ B8_15 ^ k0_7, s01_input, 1);
    uint8_t F16_23 = sBox(F8_15, s01_input, 0);

    result |= sBox(F16_23, B24_31, 1);
    result <<= 8;

    result |= F16_23;
    result <<= 8;

    result |= F8_15;
    result <<= 8;

    result |= sBox(F8_15, B0_7, 0);
    return result;
}

void FEAL::copyInput(std::vector<char>::iterator leftByte, std::vector<char>::iterator rightByte) noexcept
{
    m_left = 0;
    m_right = 0;
    for (auto i = 0; i < 4; ++i)
    {
        m_left <<= 8;
        m_left |= static_cast<uint8_t>(*leftByte);
        ++leftByte;

        m_right <<= 8;
        m_right |= static_cast<uint8_t>(*rightByte);
        ++rightByte;
    }
}

void FEAL::copyOutput(std::vector<char>::iterator leftByte, std::vector<char>::iterator rightByte) noexcept
{
    for (auto i = 0; i < 4; ++i)
    {
        *leftByte = static_cast<char>(m_left >> (24 - (8 * i)));
        ++leftByte;

        *rightByte = static_cast<char>(m_right >> (24 - (8 * i)));
        ++rightByte;
    }
    m_left = 0;
    m_right = 0;
}

void FEAL::feistel() noexcept
{
    m_right ^= m_left;

    for (auto round = 0; round < 4; ++round)
    {
        m_left ^= roundFunction(m_right, static_cast<uint16_t>(m_keys[round]));
        std::swap(m_left, m_right);
    }

    std::swap(m_left, m_right);

    m_right ^= m_left;
}
}}
