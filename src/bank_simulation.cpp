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
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            mtx.unlock();
            std::cout << "                          |   [Account " << ID << "]" << amount << "$ widthdrawl sucessful" << std::endl;
            std::cout << "                          |   [Account " << ID << "]" << " Current Balance: " << Balance << std::endl;
            return true;
        }

        std::cout << "                          |   [Account " << ID << "]" << amount << "$ widthdrawl failed, other modification in process" << std::endl;
        return false;

    }

    bool deposit(int amount) {
    if (mtx.try_lock()){
        // critical section
        Balance += amount;
        // delay inside critical section to simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        mtx.unlock();
        std::cout << "                          |   [Account " << ID << "]" << amount << "$ deposit sucessful" << std::endl;
        std::cout << "                          |   [Account " << ID << "]" << " Current Balance: " << Balance << std::endl;
        return true;
    }

    std::cout << "                          |   [Account " << ID << "]" << amount << "$ widthdrawl failed, other modification in process" << std::endl;
    return false;
    }

};

int main() {
    bool end = false;
    BankAccount acc1(1000, 1);
    BankAccount acc2(1000, 2);
    while (!end){
        int choice;
        std::cout << "[Bank Account Operations] |" << std::endl;
        std::cout << "Widthdrawal  -------- 0   |" << std::endl;
        std::cout << "Deposit      -------- 1   |" << std::endl;
        std::cout << "Transfer     -------- 2   |" << std::endl;
        std::cout << "==========================|" << std::endl;
        std::cout << "[Simulate Deadlock] - 3   |" << std::endl;
        std::cout << "[Resolve Deadlock] -- 4   |" << std::endl;
        std::cout << "                          |" << std::endl;
        std::cout << "Enter choice:             |\n";
        std::cin >> choice;
        if (choice == 0){
            int amt;
            std::cout << "Enter Widthdrawal amount: |\n";
            std::cin >> amt;


            std::thread t1(&BankAccount::withdraw, &acc1, amt);
            t1.detach();
        } else if (choice == 1){
            int amt;
            std::cout << "Enter Deposit amount:     |\n";
            std::cin >> amt;


            std::thread t1(&BankAccount::deposit, &acc1, amt);
            t1.detach();
        }
        
    }


    return 0;
}