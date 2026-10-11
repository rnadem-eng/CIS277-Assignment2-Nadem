#include "PacketQueue.h"

#include <iomanip>
#include <iostream>
#include <vector>

int main()
{
    // Three blocks, each able to hold eight bytes.
    PacketQueue packets(8, 3);

    unsigned char first[] = {0x00, 0xFF, 0x11};
    unsigned char second[] = {0x22, 0x33};
    unsigned char third[] = {0x44, 0x55, 0x66};
    unsigned char fourth[] = {0x77, 0x88};

    std::cout << std::boolalpha;
    std::cout << "Network Packet Arrival Queue\n\n";

    std::cout << "Receive 101: "
              << packets.receive(101, first, sizeof(first)) << '\n';

    std::cout << "Receive 102: "
              << packets.receive(102, second, sizeof(second)) << '\n';

    std::cout << "Receive 103: "
              << packets.receive(103, third, sizeof(third)) << '\n';

    std::cout << "Pending: " << packets.pending() << '\n';
    std::cout << "Available blocks: "
              << packets.availableBlocks() << '\n';

    // The queue is full. This attempt must change nothing.
    std::cout << "\nReceive 104 while full: "
              << packets.receive(104, fourth, sizeof(fourth)) << '\n';

    std::cout << "Pending after rejection: "
              << packets.pending() << '\n';
    std::cout << "Available after rejection: "
              << packets.availableBlocks() << '\n';

    std::size_t id = 0;
    std::vector<unsigned char> output;

    // Process the first packet.
    if (!packets.processNext(id, output))
    {
        std::cerr << "Could not process the first packet.\n";
        return 1;
    }

    std::cout << "\nFirst processed ID: " << id << '\n';
    std::cout << "Its bytes: ";

    for (unsigned char byte : output)
    {
        std::cout << std::hex << std::uppercase
                  << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(byte) << ' ';
    }

    std::cout << std::dec << std::setfill(' ') << '\n';

    std::vector<unsigned char> expected = {0x00, 0xFF, 0x11};

    std::cout << "Binary data preserved: "
              << (output == expected) << '\n';

    std::cout << "Pending: " << packets.pending() << '\n';
    std::cout << "Available blocks: "
              << packets.availableBlocks() << '\n';

    // One block is now free, so another packet can arrive.
    std::cout << "\nReceive 104 after processing 101: "
              << packets.receive(104, fourth, sizeof(fourth)) << '\n';

    std::cout << "The pool can reuse the released block.\n";
    std::cout << "Packet 104 still waits behind 102 and 103.\n";
    std::cout << "Pending: " << packets.pending() << '\n';
    std::cout << "Available blocks: "
              << packets.availableBlocks() << '\n';

    std::cout << "\nExpected remaining order: 102 103 104\n";
    std::cout << "Actual remaining order:   ";

    while (packets.processNext(id, output))
    {
        std::cout << id << ' ';
    }

    std::cout << "\nPending after draining: "
              << packets.pending() << '\n';
    std::cout << "Available after draining: "
              << packets.availableBlocks() << '\n';

    // Set known outputs before trying an empty queue.
    id = 999;
    output = {0xAA};

    bool result = packets.processNext(id, output);
    std::vector<unsigned char> unchangedBytes = {0xAA};

    std::cout << "\nProcess empty queue: " << result << '\n';
    std::cout << "Output ID unchanged: " << (id == 999) << '\n';
    std::cout << "Output bytes unchanged: "
              << (output == unchangedBytes) << '\n';

    return 0;
}