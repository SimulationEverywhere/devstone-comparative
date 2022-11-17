#define BOOST_TEST_MODULE DEVStoneTests
#include <boost/test/unit_test.hpp>
#include <string>
#include "../src/devstone.hpp"

#define STEP 5
#define MAX_WIDTH 50
#define MAX_DEPTH 50

int expectedAtomics(const std::string& type, int width, int depth) {
    auto wFactor = width - 1;
    if (type == "HOmod") {
        wFactor += (width - 1) * width / 2;
    }
    return wFactor * (depth - 1) + 1;
}

int expectedEICs(const std::string& type, int width, int depth) {
    auto wFactor = width;
    if (type == "HO") {
        wFactor = width + 1;
    } else if (type == "HOmod") {
        wFactor = 2 * (width - 1) + 1;
    }
    return wFactor * (depth - 1) + 1;
}

int expectedICs(const std::string& type, int width, int depth) {
    auto wFactor = 0;
    if (width > 2 && (type == "HI" || type == "HO")) {
        wFactor = (width - 2);
    } else if (type == "HOmod") {
        wFactor = (width - 1) * (width - 1) + (width - 1) * width / 2;
    }
    return wFactor * (depth - 1);
}

int expectedEOCs(const std::string& type, int width, int depth) {
    return (type == "HO")? width * (depth - 1) + 1 : depth;
}

int expectedInternals(const std::string& type, int width, int depth) {
    if (type == "LI") {
        return (width - 1) * (depth - 1) + 1;
    } else if (type == "HI" || type == "HO") {
        return (width - 1) * width / 2 * (depth - 1) + 1;
    }
    auto n = 1;
    for (int d = 1; d < depth; ++d) {
        n += (1 + (d - 1) * (width - 1)) * (width - 1) * width / 2 + (width - 1) * (width + (d - 1) * (width - 1));
    }
    return n;
}

int expectedExternals(const std::string& type, int width, int depth) {
    return expectedInternals(type, width, depth);
}

int expectedEvents(const std::string& type, int width, int depth) {
    if (type != "HOmod") {
        return expectedInternals(type, width, depth);
    }
    auto n = 1;
    if (width > 1 && depth > 1) {
        n += 2 * (width - 1);
        auto aux = 0;
        for (int i = 2; i < depth; ++i) {
            aux += 1 + (i - 1) * (width - 1);
        }
        n += aux * 2 * (width - 1) * (width - 1);
        n += (aux + 1) * ((width - 1) * (width - 1) + (width - 2) * (width - 1) / 2);
    }
    return n;
}

adevs::Simulator<IO_Type> createEngine(DEVStone* devstone) {
    return {devstone};
}

[[maybe_unused]] void runSimulation(adevs::Simulator<IO_Type>& sim) {
    while (sim.nextEventTime() < DBL_MAX) {
        sim.execNextEvent();
    }
}

BOOST_AUTO_TEST_CASE(DEVStoneLI)
{
    for (int w = 1; w <= MAX_WIDTH; w += STEP) {
        for (int d = 1; d <= MAX_DEPTH; d += STEP) {
            auto coupled = new DEVStone("LI", w, d, 0, 0);
            BOOST_CHECK_EQUAL(coupled->model->nAtomics(), expectedAtomics("LI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEICs(), expectedEICs("LI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nICs(), expectedICs("LI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEOCs(), expectedEOCs("LI", w, d));
            auto coordinator = createEngine(coupled);
            runSimulation(coordinator);
            BOOST_CHECK_EQUAL(coupled->model->nInternals(), expectedInternals("LI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nExternals(), expectedExternals("LI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEvents(), expectedEvents("LI", w, d));
        }
    }
}

BOOST_AUTO_TEST_CASE(DEVStoneHI)
{
    for (int w = 1; w <= MAX_WIDTH; w += STEP) {
        for (int d = 1; d <= MAX_DEPTH; d += STEP) {
            auto coupled = new DEVStone("HI", w, d, 0, 0);
            BOOST_CHECK_EQUAL(coupled->model->nAtomics(), expectedAtomics("HI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEICs(), expectedEICs("HI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nICs(), expectedICs("HI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEOCs(), expectedEOCs("HI", w, d));
            auto coordinator = createEngine(coupled);
            runSimulation(coordinator);
            BOOST_CHECK_EQUAL(coupled->model->nInternals(), expectedInternals("HI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nExternals(), expectedExternals("HI", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEvents(), expectedEvents("HI", w, d));
        }
    }
}

BOOST_AUTO_TEST_CASE(DEVStoneHO)
{
    for (int w = 1; w <= MAX_WIDTH; w += STEP) {
        for (int d = 1; d <= MAX_DEPTH; d += STEP) {
            auto coupled = new DEVStone("HO", w, d, 0, 0);
            BOOST_CHECK_EQUAL(coupled->model->nAtomics(), expectedAtomics("HO", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEICs(), expectedEICs("HO", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nICs(), expectedICs("HO", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEOCs(), expectedEOCs("HO", w, d));
            auto coordinator = createEngine(coupled);
            runSimulation(coordinator);
            BOOST_CHECK_EQUAL(coupled->model->nInternals(), expectedInternals("HO", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nExternals(), expectedExternals("HO", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEvents(), expectedEvents("HO", w, d));
        }
    }
}

BOOST_AUTO_TEST_CASE(DEVStoneHOmod)
{
    for (int w = 1; w <= MAX_WIDTH / 5; w += STEP / 5) {
        for (int d = 1; d <= MAX_DEPTH / 5; d += STEP / 5) {
            auto coupled = new DEVStone("HOmod", w, d, 0, 0);
            BOOST_CHECK_EQUAL(coupled->model->nAtomics(), expectedAtomics("HOmod", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEICs(), expectedEICs("HOmod", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nICs(), expectedICs("HOmod", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEOCs(), expectedEOCs("HOmod", w, d));
            auto coordinator = createEngine(coupled);
            runSimulation(coordinator);
            BOOST_CHECK_EQUAL(coupled->model->nInternals(), expectedInternals("HOmod", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nExternals(), expectedExternals("HOmod", w, d));
            BOOST_CHECK_EQUAL(coupled->model->nEvents(), expectedEvents("HOmod", w, d));
        }
    }
}
