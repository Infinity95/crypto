#pragma once

#ifndef CRYPTO_ALGORITHMS_SYMMETRIC_BLOCK_CIPHER_HPP
#define CRYPTO_ALGORITHMS_SYMMETRIC_BLOCK_CIPHER_HPP
#include <vector>

namespace crypto {namespace algorithms
{
class SymmetricBlockCipher
{
public:
    virtual ~SymmetricBlockCipher() = 0;

    /**
     * Encrypts one block of the supplied data vector.
     */
    virtual std::vector<char>::iterator encrypt(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept = 0;

    virtual std::vector<char>::iterator decrypt(std::vector<char>& data, const std::vector<char>::iterator& block) noexcept = 0;

    virtual size_t getBlockSize() noexcept = 0;
};

inline SymmetricBlockCipher::~SymmetricBlockCipher() {}
}}

#endif // CRYPTO_ALGORITHMS_SYMMETRIC_BLOCK_CIPHER_HPP