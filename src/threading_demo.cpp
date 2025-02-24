#include <iostream>
#include <thread>
#include <chrono>


// run with ./demo

// This function will be run by each thread.
// It takes an integer (threadID) to identify the thread.
void threadFunction(int threadID) {
    // Loop to simulate some work done by the thread.
    for (int i = 0; i < 5; ++i) {
        // Output the current thread ID and iteration.
        std::cout << "Thread " << threadID << " executing iteration " << i << std::endl;
        // Sleep for 500 milliseconds to simulate work and allow context switching.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    // Create two threads that execute 'threadFunction' concurrently.
    std::thread thread1(threadFunction, 1); // Pass thread ID 1
    std::thread thread2(threadFunction, 2); // Pass thread ID 2

    // 'join' makes the main thread wait until the threads have finished executing.
    thread1.join();
    thread2.join();

    // All threads have finished, so we can safely print a final message.
    std::cout << "Both threads have finished execution." << std::endl;

    return 0;
}
