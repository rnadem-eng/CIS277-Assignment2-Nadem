# CIS-277 Assignment 2: Network Packet Arrival Queue

## Student

Robeul Nadem

## Design

This program uses a custom Queue to process packets in arrival order. It uses the Stack and MemoryPool from Assignment 1 to manage reusable memory blocks.

My Queue uses a vector as fixed-size array storage. It does not resize the vector or use a ready-made queue.

The queue tracks two values:

- `head`: the array position of the oldest item.
- `itemCount`: the number of items currently waiting.

The next insertion position is calculated with:

```cpp
(head + itemCount) % items.size()
```

When an item is removed, head advances by one. If it reaches the array size, it returns to zero. No other items are shifted.

PacketQueue owns a MemoryPool and a Queue of packet descriptors. Each descriptor stores the packet ID, its memory block address, and its byte length. The actual packet bytes are stored in the pool.

Receiving a packet copies its bytes into a block and adds a descriptor to the queue. Processing a packet copies its bytes into the output vector, removes its descriptor, and returns the block to the pool.

## How to Compile

From the repository root, compile the demonstration with C++23:

```bash
clang++ -std=c++23 -Wall -Wextra -pedantic main.cpp PacketQueue.cpp MemoryPool.cpp -o packet_queue
```

Compile the additional student tests:

```bash
clang++ -std=c++23 -Wall -Wextra -pedantic tests/student_tests.cpp PacketQueue.cpp MemoryPool.cpp -o student_tests
```

With GCC, replace `clang++` with `g++`.

## How to Run

Run the demonstration:

```bash
./packet_queue
```

Run the student tests:

```bash
./student_tests
```

Run the instructor's preflight validator:

```bash
python3 validate.py
```

## Tests and Results

The program and student tests compiled using C++23. All student tests passed.

| Test | Expected result | Actual result |
|---|---|---|
| Integer queue wraparound | After removing 10 and adding 40, remaining order is 20, 30, 40 | Passed |
| String queue | Strings leave in arrival order, including after wraparound | Passed |
| Front without removal | Returns the oldest item without changing size | Passed |
| Full queue | Enqueue returns false and preserves existing items | Passed |
| Empty queue | Front and dequeue return false without changing the output | Passed |
| One-byte packet | Accepted and returned with matching bytes | Passed |
| Packet exactly block size | Accepted and returned with matching bytes | Passed |
| Oversized packet | Rejected without consuming a block or queue slot | Passed |
| Null input and zero length | Rejected with counts unchanged | Passed |
| Full packet queue | Receive returns false with counts unchanged | Passed |
| Packet FIFO after reuse | Packet 22 is processed before newly received packet 33 | Passed |
| Binary data | Values including 0x00 and 0xFF are preserved | Passed |
| Empty processNext | Returns false and preserves both output arguments | Passed |
| Pool counts after draining | Zero pending packets and all blocks available | Passed |

The demonstration also showed remaining packet IDs in the order 102, 103, 104 after processing packet 101 and receiving packet 104.

Instructor preflight validator: PASS. Ran python3 validate.py successfully. The independent public tests passed, and the demonstration compiled.

## Analysis Questions

### 1. Why must packet service be FIFO even though free-block reuse is LIFO?

FIFO processes packets in arrival order, so newer packets do not pass older waiting packets. The pool's free blocks contain no waiting packets and are all the same size, so the most recently released block can be reused first. Memory reuse order does not determine packet processing order.

### 2. How does your circular array wrap without shifting all waiting packets?

Insertion uses `(head + itemCount) % capacity` to find the next slot. Removal advances head and resets it to zero when it reaches capacity. Only the indices and count change; the waiting items are not shifted.

### 3. What happens at queue overflow and underflow, and why must those attempts leave state unchanged?

When the queue is full, enqueue returns false. When it is empty, dequeue and front return false without changing their output arguments. These checks happen before modifying the queue, which prevents losing existing items or reporting an item that does not exist.

### 4. What are the time complexities of queue operations and of receive/processNext when the packet has length bytes?

Each queue operation takes O(1) time, excluding the cost of copying the stored type. It uses a fixed number of checks and index updates.

Successful receive and processNext operations take O(length) time because they copy the packet's bytes. Their queue and pool operations take O(1).

### 5. Why must processNext copy bytes out before returning the block to the pool?

After a block is returned, another packet may reuse it and overwrite its contents. Copying first preserves the processed packet's data in the caller's output vector.

## AI Use

I used ChatGPT to help draft the Queue, PacketQueue, demonstration, tests, and README, and to explain the code and setup steps. I reused the Assignment 1 Stack and MemoryPool, which were also developed with AI assistance.

I checked the result by compiling with C++23, running the demonstration, and running the additional student tests. The tests checked FIFO behavior, wraparound, binary data, invalid inputs, and memory pool counts.