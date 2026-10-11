#include "PacketQueue.h"
#include <cstring>

PacketQueue::PacketQueue(std::size_t blockSize,
                         std::size_t blockCount)
    : pool(blockSize, blockCount),
      waitingPackets(blockCount)
{
}

bool PacketQueue::receive(std::size_t id,
                          const unsigned char *bytes,
                          std::size_t length)
{
    // Check the packet before allocating memory.
    if (bytes == nullptr || length == 0)
    {
        return false;
    }

    if (length > pool.blockSize())
    {
        return false;
    }

    if (waitingPackets.full() || pool.availableBlocks() == 0)
    {
        return false;
    }

    void *block = pool.allocate();

    if (block == nullptr)
    {
        return false;
    }

    std::memcpy(block, bytes, length);

    PacketDescriptor packet;
    packet.id = id;
    packet.block = block;
    packet.length = length;

    if (!waitingPackets.enqueue(packet))
    {
        pool.deallocate(block);
        return false;
    }

    return true;
}

bool PacketQueue::processNext(std::size_t &id,
                              std::vector<unsigned char> &bytes)
{
    PacketDescriptor packet;

    if (!waitingPackets.front(packet))
    {
        return false;
    }

    // Copy the data while its memory block is still reserved.
    unsigned char *start =
        static_cast<unsigned char *>(packet.block);

    bytes.assign(start, start + packet.length);

    waitingPackets.dequeue(packet);
    id = packet.id;

    pool.deallocate(packet.block);

    return true;
}

std::size_t PacketQueue::pending() const
{
    return waitingPackets.size();
}

std::size_t PacketQueue::availableBlocks() const
{
    return pool.availableBlocks();
}