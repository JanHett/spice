#include <gtest/gtest.h>

#include <spice/effect.hpp>
#include <spice/print.hpp>

using namespace spice;

TEST(effect, magic_mist) {
    auto boat = load_image<float>("../data/testing/boat.jpg");
    auto res = effect::magic_mist(boat, 20, 20, 2);
    write_image("../data/testing/boat_magic.jpg", res, OIIO::TypeDesc::UINT8);
}

// TEST(effect, gaussian_fn) {
//     std::cout << "9: ---------------------\n";
//     for (long i = -20; i < 21; ++i) {
//         std::cout << i << ":\t" << effect::gaussian_fn(9, i) << '\n';
//     }
//     std::cout << "8: ---------------------\n";
//     for (long i = -20; i < 21; ++i) {
//         std::cout << i << ":\t" << effect::gaussian_fn(8, i) << '\n';
//     }
//     std::cout << "7: ---------------------\n";
//     for (long i = -20; i < 21; ++i) {
//         std::cout << i << ":\t" << effect::gaussian_fn(7, i) << '\n';
//     }
//     std::cout << "6: ---------------------\n";
//     for (long i = -20; i < 21; ++i) {
//         std::cout << i << ":\t" << effect::gaussian_fn(6, i) << '\n';
//     }
//     std::cout << "5: ---------------------\n";
//     for (long i = -20; i < 21; ++i) {
//         std::cout << i << ":\t" << effect::gaussian_fn(5, i) << '\n';
//     }
//     std::cout << "4: ---------------------\n";
//     for (long i = -20; i < 21; ++i) {
//         std::cout << i << ":\t" << effect::gaussian_fn(4, i) << '\n';
//     }
//     std::cout << "3: ---------------------\n";
//     for (long i = -20; i < 21; ++i) {
//         std::cout << i << ":\t" << effect::gaussian_fn(3, i) << '\n';
//     }
// }
