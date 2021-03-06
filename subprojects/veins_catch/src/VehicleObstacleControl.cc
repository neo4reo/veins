#include "catch2/catch.hpp"

#include "veins/modules/obstacle/VehicleObstacleControl.h"
#include "veins/base/toolbox/Spectrum.h"
#include "veins/base/toolbox/Signal.h"
#include "testutils/Simulation.h"

using veins::Coord;
using veins::Signal;
using veins::Spectrum;
using veins::VehicleObstacleControl;

SCENARIO("Using VehicleObstacleControl", "[vehicleObstacles]")
{
    DummySimulation ds(new cNullEnvir(0, nullptr, nullptr)); // necessary so simtime_t works

    GIVEN("A constellation of sender and receiver vehicle, as well as an obstacle")
    {

        double h1 = 10;
        double h2 = 8;
        double h = 9;
        double d = 7;
        double d1 = 3;

        THEN("A signal at 5.89 GHz is attenuated by approx. 0.65 dB")
        {

            Spectrum::Frequencies freqs = {5.89e9};
            Signal attenuationPrototype = Signal(Spectrum(freqs));

            auto r = VehicleObstacleControl::getVehicleAttenuationSingle(h1, h2, h, d, d1, attenuationPrototype);

            REQUIRE(r.at(0) == Approx(0.6454291084));
        }

        std::vector<std::pair<double, double>> dz_vec = {{0, h1}, {d1, h}, {d, h2}};

        THEN("A signal at 5.89 GHz is attenuated by approx. 0.65 dB (if called DZ style)")
        {

            Spectrum::Frequencies freqs = {5.89e9};
            Signal attenuationPrototype = Signal(Spectrum(freqs));

            auto r = VehicleObstacleControl::getVehicleAttenuationDZ(dz_vec, attenuationPrototype);

            REQUIRE(r.at(0) == Approx(0.6454291084));
        }
    }

    GIVEN("An obstacle at same height as sender and receiver")
    {

        std::vector<std::pair<double, double>> dz_vec = {{0, 5}, {5, 5}, {10, 5}};

        THEN("A signal is attenuated by approx. 6 dB, independent of geometry and frequency")
        {

            Spectrum::Frequencies freqs = {5.89e9};
            Signal attenuationPrototype = Signal(Spectrum(freqs));

            auto r = VehicleObstacleControl::getVehicleAttenuationDZ(dz_vec, attenuationPrototype);

            double r_des = 6.9 + 20 * log10(sqrt(pow((-0.1), 2) + 1) - 0.1);

            REQUIRE(r.at(0) == Approx(r_des));
        }
    }

    GIVEN("Two obstacles at same height as sender and receiver at pairwise distances of 3,4,3")
    {

        std::vector<std::pair<double, double>> dz_vec = {{0, 5}, {3, 5}, {7, 5}, {10, 5}};

        THEN("A signal is attenuated by approx. 2x6 dB minus a correction term of approx. 0.9 dB")
        {

            Spectrum::Frequencies freqs = {5.89e9};
            Signal attenuationPrototype = Signal(Spectrum(freqs));

            auto r = VehicleObstacleControl::getVehicleAttenuationDZ(dz_vec, attenuationPrototype);

            double r_des = 6.9 + 20 * log10(sqrt(pow((-0.1), 2) + 1) - 0.1);
            double r_corr = -10 * log10(360.0 / 441.0);

            REQUIRE(r.at(0) == Approx(2 * r_des + r_corr));
        }
    }
}
