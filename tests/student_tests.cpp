#include "../Queue.h"
#include "../PacketQueue.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

void testIntegerQueue()
{
    Queue<int> numbers(3);
    int value = 99;

    assert(numbers.capacity() == 3);
    assert(numbers.empty());
    assert(!numbers.full());
    assert(numbers.size() == 0);

    // Empty operations must leave value unchanged.
    assert(!numbers.front(value));
    assert(value == 99);
    assert(!numbers.dequeue(value));
    assert(value == 99);

    assert(numbers.enqueue(10));
    assert(numbers.enqueue(20));
    assert(numbers.enqueue(30));
    assert(numbers.full());

    // A full queue must reject another item.
    assert(!numbers.enqueue(40));
    assert(numbers.size() == 3);

    // Looking at the front must not remove it.
    assert(numbers.front(value));
    assert(value == 10);
    assert(numbers.size() == 3);

    assert(numbers.dequeue(value));
    assert(value == 10);
    assert(!numbers.full());

    // This insertion wraps to the beginning of the array.
    assert(numbers.enqueue(40));

    assert(numbers.dequeue(value));
    assert(value == 20);
    assert(numbers.dequeue(value));
    assert(value == 30);
    assert(numbers.dequeue(value));
    assert(value == 40);

    assert(numbers.empty());
    assert(numbers.size() == 0);
    assert(!numbers.dequeue(value));
    assert(value == 40);

    std::cout << "PASS: integer queue and wraparound\n";
}

void testStringQueue()
{
    Queue<std::string> names(2);
    std::string value = "unchanged";

    assert(!names.front(value));
    assert(value == "unchanged");

    assert(names.enqueue("first"));
    assert(names.enqueue("second"));
    assert(!names.enqueue("third"));

    assert(names.front(value));
    assert(value == "first");
    assert(names.size() == 2);

    assert(names.dequeue(value));
    assert(value == "first");

    assert(names.enqueue("third"));

    assert(names.dequeue(value));
    assert(value == "second");
    assert(names.dequeue(value));
    assert(value == "third");
    assert(names.empty());

    assert(!names.dequeue(value));
    assert(value == "third");

    std::cout << "PASS: string queue\n";
}

void testPackets()
{
    PacketQueue packets(4, 2);

    unsigned char small[] = {0xFF};
    unsigned char exact[] = {0x00, 0xFF, 0x12, 0x34};
    unsigned char oversized[] = {1, 2, 3, 4, 5};

    // Invalid arrivals must not consume a block or queue slot.
    assert(!packets.receive(1, nullptr, 1));
    assert(!packets.receive(1, small, 0));
    assert(!packets.receive(1, oversized, sizeof(oversized)));

    assert(packets.pending() == 0);
    assert(packets.availableBlocks() == 2);

    // Test lengths of one byte and exactly one block.
    assert(packets.receive(11, small, sizeof(small)));
    assert(packets.receive(22, exact, sizeof(exact)));

    assert(packets.pending() == 2);
    assert(packets.availableBlocks() == 0);

    assert(!packets.receive(33, small, sizeof(small)));
    assert(packets.pending() == 2);
    assert(packets.availableBlocks() == 0);

    std::size_t id = 0;
    std::vector<unsigned char> output;
    std::vector<unsigned char> expectedSmall = {0xFF};
    std::vector<unsigned char> expectedExact = {
        0x00, 0xFF, 0x12, 0x34};

    assert(packets.processNext(id, output));
    assert(id == 11);
    assert(output == expectedSmall);
    assert(packets.pending() == 1);
    assert(packets.availableBlocks() == 1);

    // Reuse the released space without breaking FIFO order.
    assert(packets.receive(33, small, sizeof(small)));

    assert(packets.processNext(id, output));
    assert(id == 22);
    assert(output == expectedExact);

    assert(packets.processNext(id, output));
    assert(id == 33);
    assert(output == expectedSmall);

    assert(packets.pending() == 0);
    assert(packets.availableBlocks() == 2);

    id = 999;
    output = {0xAA};
    std::vector<unsigned char> unchanged = {0xAA};

    assert(!packets.processNext(id, output));
    assert(id == 999);
    assert(output == unchanged);
    assert(packets.pending() == 0);
    assert(packets.availableBlocks() == 2);

    std::cout << "PASS: packet boundaries, binary data, and FIFO\n";
}

int main()
{
    testIntegerQueue();
    testStringQueue();
    testPackets();

    std::cout << "All student tests passed.\n";
    return 0;
}