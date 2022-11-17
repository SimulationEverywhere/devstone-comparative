#ifndef DEVSTONE_ADEVS_DEVSTONECOUPLED_HPP
#define DEVSTONE_ADEVS_DEVSTONECOUPLED_HPP

#include "utils.hpp"
#include "devstone_atomic.hpp"
#include <vector>

class DEVStoneCoupled: public adevs::Digraph<int*> {
protected:
    DEVStoneCoupled* childCoupled;
    std::vector<DEVStoneAtomic*> atomics;
    int intDelay, extDelay;
    std::size_t nEIC, nIC, nEOC;

public:
    /// Model input port
    static const int in = 0;
    /// Model output port
    static const int out = 1;
    /// Constructor
    DEVStoneCoupled(int width, int depth, int intDelay, int extDelay): adevs::Digraph<int*>(), childCoupled(), atomics(), intDelay(intDelay), extDelay(extDelay), nEIC(), nIC(), nEOC() {
        if (depth < 1 || width < 1) {
            throw std::bad_exception();
        }
        if (depth == 1) {
            auto *a = addAtomic();
            couple(this, DEVStoneCoupled::in, a, DEVStoneAtomic::in);
            nEIC += 1;
            couple(a, DEVStoneAtomic::out, this, DEVStoneCoupled::out);
            nEOC += 1;
        }
    }

    DEVStoneAtomic* addAtomic() {
        auto *a = new DEVStoneAtomic(intDelay, extDelay);
        add(a);
        atomics.push_back(a);
        return a;
    }

    [[nodiscard]] unsigned long nEICs() const {
        auto res = nEIC;
        if (childCoupled != nullptr) {
            res += childCoupled->nEICs();
        }
        return res;
    }

    [[nodiscard]] unsigned long nICs() const {
        auto res = nIC;
        if (childCoupled != nullptr) {
            res += childCoupled->nICs();
        }
        return res;
    }

    [[nodiscard]] unsigned long nEOCs() const {
        auto res = nEOC;
        if (childCoupled != nullptr) {
            res += childCoupled->nEOCs();
        }
        return res;
    }

    [[nodiscard]] unsigned long nAtomics() const {
        auto res = atomics.size();
        if (childCoupled != nullptr) {
            res += childCoupled->nAtomics();
        }
        return res;
    }

    [[nodiscard]] unsigned long nInternals() const {
        auto res = (childCoupled != nullptr) ? childCoupled->nInternals() : 0;
        for (const auto& atomic: atomics) {
            res += atomic->nInternals;
        }
        return res;
    }

    [[nodiscard]] unsigned long nExternals() const {
        auto res = (childCoupled != nullptr) ? childCoupled->nExternals() : 0;
        for (const auto& atomic: atomics) {
            res += atomic->nExternals;
        }
        return res;
    }

    [[nodiscard]] unsigned long nEvents() const {
        auto res = (childCoupled != nullptr) ? childCoupled->nEvents() : 0;
        for (const auto& atomic: atomics) {
            res += atomic->nEvents;
        }
        return res;
    }
};

#endif //DEVSTONE_ADEVS_DEVSTONECOUPLED_HPP
