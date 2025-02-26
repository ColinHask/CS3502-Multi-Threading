#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

// Global mutex to protect the shared counter.
std::mutex mtx;

// Function that increments the counter.
// It accepts a thread ID for logging, a reference to the counter, and the number of iterations.
void incrementCounter(int threadId, int &counter, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        // Print the current iteration.
        std::cout << "[Thread " << threadId << "] iteration " << i << std::endl;
        
        // Try to acquire the mutex using try_lock in a loop.
        while (!mtx.try_lock()) {
            // If try_lock fails, print that the thread is waiting.
            std::cout << "[Thread " << threadId << "] waiting for lock" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay to avoid busy-waiting.
        }
        
        // Once the lock is acquired, print a message.
        std::cout << "[Thread " << threadId << "] acquired lock" << std::endl;
        
        // Critical section: safely increment the counter.
        ++counter;
        std::cout << "[Thread " << threadId << "] counter incremented to " << counter << std::endl;
        
        // Simulate some work while holding the lock.
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        
        // Release the lock and print a message.
        mtx.unlock();
        std::cout << "[Thread " << threadId << "] released lock" << std::endl;
        
        // Delay outside the critical section to simulate additional work.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    int counter = 0;
    int iterations = 10;
    
    // Create two threads with different IDs.
    std::thread t1(incrementCounter, 1, std::ref(counter), iterations);
    std::thread t2(incrementCounter, 2, std::ref(counter), iterations);
    
    // Wait for both threads to finish.
    t1.join();
    t2.join();
    
    // Output the final counter value.
    std::cout << "Final counter value: " << counter << std::endl;
    return 0;
}
