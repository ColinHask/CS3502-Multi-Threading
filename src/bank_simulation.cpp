#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

class BankAccount{
    public:
    int Balance;
    int ID;
    std::mutex mtx;

    BankAccount(int bal, int i){
        Balance = bal;
        ID = i;
    }

    bool withdraw(int amount) {
        if (mtx.try_lock()){
            // critical section
            Balance -= amount;
            // delay inside critical section to simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            mtx.unlock();
            std::cout << "[Account " << ID << "]" << amount << "$ widthdrawl sucessful" << std::endl;
            std::cout << "[Account " << ID << "]" << " Current Balance: " << Balance << std::endl;
            return true;
        }

        std::cout << "[Account " << ID << "]" << amount << "$ widthdrawl failed" << std::endl;
        return false;

    }

};

int main() {

    BankAccount acc1(1000, 0);

    std::thread t1(&BankAccount::withdraw, &acc1, 10);
    std::thread t2(&BankAccount::withdraw, &acc1, 10);

    t1.join();
    t2.join();

    return 0;
}