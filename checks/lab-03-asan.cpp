#include <cstdint>
#include <cstddef>
int main(int argc, char**) {
    volatile std::uint8_t data[4096]{};
    const std::size_t index = 4095 + static_cast<std::size_t>(argc);
    data[index] = 1; // Deliberate one-past-end write: standalone experiment only.
    return data[0];
}
