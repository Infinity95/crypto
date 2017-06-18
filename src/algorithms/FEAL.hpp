#pragma once

#ifndef CRYPTO_ALGORITHMS_FEAL_HPP
#define CRYPTO_ALGORITHMS_FEAL_HPP
#include <vector>
#include "algorithms/SymmetricBlockCipher.hpp"

namespace crypto {namespace algorithms
{
class FEAL final : public SymmetricBlockCipher
{
private:
    const size_t m_blockSize = 8;
    uint32_t m_left;
    uint32_t m_right;
    uint64_t m_key;
    uint16_t m_keys[4];
public:
    explicit FEAL(uint64_t key) noexcept;

    std::vector<char>::iterator encrypt(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept override;

    std::vector<char>::iterator decrypt(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept override;

    size_t getBlockSize() noexcept override;
private:

    std::vector<char>::iterator applyEnc(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept;

    uint8_t sBox(uint8_t A, uint8_t B, uint8_t i) noexcept;
    uint32_t roundFunction(uint32_t B, uint16_t k) noexcept;

    void copyInput(std::vector<char>::iterator leftByte, std::vector<char>::iterator rightByte) noexcept;
    void copyOutput(std::vector<char>::iterator leftByte, std::vector<char>::iterator rightByte) noexcept;

    void feistel() noexcept;
};
}}

#endif // CRYPTO_ALGORITHMS_FEAL_HPP
