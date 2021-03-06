#pragma once

#ifndef CRYPTO_ALGORITHMS_FEAL_LINEAR_CA_HPP
#define CRYPTO_ALGORITHMS_FEAL_LINEAR_CA_HPP
#include <utility>
#include <cstdint>
#include <vector>
#include "algorithms/FEAL.hpp"
#include <unordered_set>

namespace crypto { namespace algorithms
{
class FEALLinearCA
{
public:
    typedef std::pair<std::vector<char>, std::vector<char>> PlaintextCiphertextPair;
private:
    std::vector<PlaintextCiphertextPair> m_plaintextCiphertextPairs;
    std::vector<std::pair<uint64_t, uint64_t>> m_pcPairsU64;
    std::unordered_set<uint16_t> m_k1Candidates;
    std::unordered_set<uint16_t> m_k4Candidates;
public:

    void addPlaintextCiphertextPair(const PlaintextCiphertextPair& pair) noexcept;

    /**
     * Finds the key that the plaintext ciphertext pairs were encrypted with.
     */
    uint64_t breakCipher();

    void reset() noexcept;
private:

    void add_pcPairU64(const PlaintextCiphertextPair& pair) noexcept;

    void calculateCandidates(bool flipIO, std::unordered_set<uint16_t>& candidateSet) noexcept;

    void calculate_k1_k4_candidates() noexcept;

    uint16_t bruteForceRound(uint32_t in, uint32_t out);

    bool isCorrectKey(uint64_t key) noexcept;
};
}}

#endif // CRYPTO_ALGORITHMS_FEAL_LINEAR_CA_HPP
