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

    // method assumes lock is aquired, DO NOT USE WITHOUT LOCKING
    void withdraw(int amount){
        Balance -= amount;
    }
    // method assumes lock is aquired, DO NOT USE WITHOUT LOCKING
    void deposit(int amount){
        Balance += amount;
    }

    // safe withdrawal to be used without pre-locking
    bool tryWithdraw(int amount) {
        if (mtx.try_lock()){
            // critical section
            Balance -= amount;
            //delay to simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            mtx.unlock();
            std::cout << "                          |   [Account " << ID << "] $" << amount << " widthdrawal sucessful" << std::endl;
            std::cout << "                          |   [Account " << ID << "]" << " Current Balance: " << Balance << std::endl;
            return true;
        }

        std::cout << "                          |   [Account " << ID << "] $" << amount << " widthdrawl failed, other modification in process" << std::endl;
        return false;

    }
    // safe deposit to be used without pre-locking
    bool tryDeposit(int amount) {
    if (mtx.try_lock()){
        // critical section
        Balance += amount;
        //delay to simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        mtx.unlock();
        std::cout << "                          |   [Account " << ID << "] $" << amount << " deposit sucessful" << std::endl;
        std::cout << "                          |   [Account " << ID << "]" << " Current Balance: " << Balance << std::endl;
        return true;
    }

    std::cout << "                          |   [Account " << ID << "] $" << amount << " deposit failed, other modification in process" << std::endl;
    return false;
    }

};


// Transfers funds between 2 accounts. Mutex lock attempts expire after 25 seconds.
// FROM: acc1 TO: acc2
// Safe mode: waits for mutex locks in order of ID.
// Unsafe mode: waits for mutex locks in order given.
bool tryTransfer(BankAccount &acc1, BankAccount &acc2, int amount, bool safe_mode){


    if (safe_mode){
    // safe mode
    // acquire mutex locks in order of ID
        if (acc1.ID < acc2.ID){
            // start time for locking account 1
            auto start1 = std::chrono::steady_clock::now();
            // get lock 1
            while(!acc1.mtx.try_lock()){
                std::cout << "                          |   [Waiting on account" << acc1.ID <<" availability] " << std::endl;
                // delay to limit cout spamming
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                // Check how much time has elapsed since 'start1'
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start1);
                    
                if (elapsed.count() >= 25) {
                    std::cout << "                          |   [Account" << acc1.ID <<" waiting timed out] " << std::endl;
                    std::cout << "                          |   [Transfer aborted] " << std::endl;
                    return false;
                }
            }
            std::cout << "                          |   [Account" << acc1.ID <<" access granted] " << std::endl; 
            // delay to simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            // get lock 2
            // start time for locking account 2
            auto start2 = std::chrono::steady_clock::now();
            while(!acc2.mtx.try_lock()){
                std::cout << "                          |   [Waiting on account" << acc2.ID <<" availability] " << std::endl;
                // delay to limit cout spamming
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                // Check how much time has elapsed since 'start2'
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start2);
                    
                if (elapsed.count() >= 25) {
                    std::cout << "                          |   [Account" << acc2.ID <<" waiting timed out] " << std::endl;
                    std::cout << "                          |   [Transfer aborted] " << std::endl;
                    acc1.mtx.unlock();
                    return false;
                }
            }
            std::cout << "                          |   [Account" << acc2.ID <<" access granted] " << std::endl;
        } else {
            // get lock 2
            // start time for locking account 2
            auto start1 = std::chrono::steady_clock::now();
            while(!acc2.mtx.try_lock()){
                std::cout << "                          |   [Waiting on account" << acc2.ID <<" availability] " << std::endl;
                // delay to limit cout spamming
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                // Check how much time has elapsed since 'start1'
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start1);
                    
                if (elapsed.count() >= 25) {
                    std::cout << "                          |   [Account" << acc2.ID <<" waiting timed out] " << std::endl;
                    std::cout << "                          |   [Transfer aborted] " << std::endl;
                    return false;
                }
            }
            std::cout << "                          |   [Account" << acc2.ID <<" access granted] " << std::endl;
            // delay to simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            // get lock 1
            // start time for locking account 1
            auto start2 = std::chrono::steady_clock::now();
            while(!acc1.mtx.try_lock()){
                std::cout << "                          |   [Waiting on account" << acc1.ID <<" availability] " << std::endl;
                // delay to limit cout spamming
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                // Check how much time has elapsed since 'start2'
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start2);
                    
                if (elapsed.count() >= 25) {
                    std::cout << "                          |   [Account" << acc1.ID <<" waiting timed out] " << std::endl;
                    std::cout << "                          |   [Transfer aborted] " << std::endl;
                    acc2.mtx.unlock();
                    return false;
                }
            }
            std::cout << "                          |   [Account" << acc1.ID <<" access granted] " << std::endl; 
        }
    } else {
        // unsafe mode 
        // acquire locks by given order
        // start time for locking account 1
        auto start1 = std::chrono::steady_clock::now();
        // get lock 1
        while(!acc1.mtx.try_lock()){
            std::cout << "                          |   [Waiting on account" << acc1.ID <<" availability] " << std::endl;
            // delay to limit cout spamming
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            // Check how much time has elapsed since 'start1'
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start1);
                
            if (elapsed.count() >= 25) {
                std::cout << "                          |   [Account" << acc1.ID <<" waiting timed out] " << std::endl;
                std::cout << "                          |   [Transfer aborted] " << std::endl;
                return false;
            }
        }
        std::cout << "                          |   [Account" << acc1.ID <<" access granted] " << std::endl; 
        // delay to simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        // get lock 2
        // start time for locking account 2
        auto start2 = std::chrono::steady_clock::now();
        while(!acc2.mtx.try_lock()){
            std::cout << "                          |   [Waiting on account" << acc2.ID <<" availability] " << std::endl;
            // delay to limit cout spamming
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            // Check how much time has elapsed since 'start2'
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start2);
                
            if (elapsed.count() >= 25) {
                std::cout << "                          |   [Account" << acc2.ID <<" waiting timed out] " << std::endl;
                std::cout << "                          |   [Transfer aborted] " << std::endl;
                acc1.mtx.unlock();
                return false;
            }
        }
        std::cout << "                          |   [Account" << acc2.ID <<" access granted] " << std::endl;
    }

    // critical section
    // do transfer
    acc1.withdraw(amount);
    acc2.deposit(amount);

    acc1.mtx.unlock();
    acc2.mtx.unlock();

    std::cout << "                          |   [Transfer complete] " << std::endl;
    std::cout << "                          |   [Account" << acc2.ID << " Current Balance: " << acc2.Balance <<" ]" << std::endl;
    std::cout << "                          |   [Account" << acc1.ID << " Current Balance: " << acc1.Balance <<" ]" << std::endl;
    return true;

}


// helper method to validate user input with integers.
// max: maximum valid int for input.
// prompt: will be displayed to user.
int readInt(const std::string& prompt, int max){
    int value;
    while(true){
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()){
            std::cin.clear(); //clear err flags

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //discard invalid input
            std::cout << "\033[31m" <<"[Invalid input]           |" << std::endl;
            std::cout << "[Please enter a number]   |" << "\033[0m" << std::endl;
        }else if(value > max){
            std::cout << "\033[31m" << "["<< value << " is not a valid entry]\n[Please try again]" << "\033[0m" << std::endl;
        }
        else{
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //clear input
            return value;
        }
    }
}


int main() {
    bool end = false;
    BankAccount acc1(1000, 1);
    BankAccount acc2(1000, 2);
    
    while (!end){
        
        try{
            int choice;
            std::cout << "__________________________|___________ LOG __________" << std::endl;
            std::cout << "[Bank Account Operations] |" << std::endl;
            std::cout << "Widthdrawal  -------- 0   |" << std::endl;
            std::cout << "Deposit      -------- 1   |" << std::endl;
            std::cout << "Transfer     -------- 2   |" << std::endl;
            std::cout << "Show Balances    ---- 3   |" << std::endl;
            std::cout << "==========================|" << std::endl;
            std::cout << "[Simulate Deadlock] - 4   |" << std::endl;
            std::cout << "[Simulate Transfers]- 5   |" << std::endl;

            choice = readInt("Enter choice:             |\n", 5);

            if (choice == 0){
                int amt;
                int acc;
                acc = readInt("Enter Account number:     |\n", 2);
                amt = readInt("Enter Widthdrawal amount: |\n", 2000000000);

                if (acc == 1){
                    std::thread t1(&BankAccount::tryWithdraw, &acc1, amt);
                    t1.detach();
                }else if (acc == 2){
                    std::thread t1(&BankAccount::tryWithdraw, &acc2, amt);
                    t1.detach();
                }else{                                     
                    throw(acc);
                }
                
            } else if (choice == 1){
                int amt;
                int acc;
                acc = readInt("Enter Account number:     |\n", 2);
                amt = readInt("Enter Deposit amount:     |\n", 2000000000);

                if (acc == 1){
                    std::thread t1(&BankAccount::tryDeposit, &acc1, amt);
                    t1.detach();
                }else if (acc == 2){
                    std::thread t1(&BankAccount::tryDeposit, &acc2, amt);
                    t1.detach();
                }else{                                     
                    std::cout << "Invalid entry             |" << std::endl;
                }
                
            }else if (choice == 2){
                int option;
                int amt;
                std::cout << "    [Transfer Options]    |" << std::endl;
                std::cout << "Account1 -> Account2 -- 1 |" << std::endl;
                std::cout << "Account2 -> Account1 -- 2 |" << std::endl;
                option = readInt("Enter choice:             |\n", 2);
                amt = readInt("Enter Transfer amount:    |\n", 2000000000);
                if (option == 1){
                    std::thread t1(tryTransfer, std::ref(acc1), std::ref(acc2), amt, true);
                    t1.detach();
                }else if (option == 2){
                    std::thread t1(tryTransfer, std::ref(acc2), std::ref(acc1), amt, true);
                    t1.detach();
                }else{                                     
                    std::cout << "Invalid entry             |" << std::endl;
                }
                

            }else if(choice == 3){
                std::cout << "[Account1 Balance: $" << acc1.Balance << "]" << std::endl;
                std::cout << "[Account2 Balance: $" << acc2.Balance << "]" << std::endl;
                
            }else if (choice == 4){
                std::cout << "[initiating 2 transfers in unsafe-tranfer mode]" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                std::thread t1(tryTransfer, std::ref(acc1), std::ref(acc2), 200, false);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::thread t2(tryTransfer, std::ref(acc2), std::ref(acc1), 200, false);
                t1.join();
                t2.join();
                 
            }else if (choice == 5){
                std::cout << "[initiating 12 transfers in safe-tranfer mode]" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                std::thread t1(tryTransfer, std::ref(acc1), std::ref(acc2), 1, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t2(tryTransfer, std::ref(acc2), std::ref(acc1), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t3(tryTransfer, std::ref(acc1), std::ref(acc2), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t4(tryTransfer, std::ref(acc2), std::ref(acc1), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t5(tryTransfer, std::ref(acc2), std::ref(acc1), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t6(tryTransfer, std::ref(acc1), std::ref(acc2), 2, true);
                t1.join();
                t2.join();
                t3.join();
                t4.join();
                t5.join();
                t6.join();
                std::thread t7(tryTransfer, std::ref(acc2), std::ref(acc1), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t8(tryTransfer, std::ref(acc1), std::ref(acc2), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t9(tryTransfer, std::ref(acc2), std::ref(acc1), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t10(tryTransfer, std::ref(acc1), std::ref(acc2), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t11(tryTransfer, std::ref(acc1), std::ref(acc2), 2, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::thread t12(tryTransfer, std::ref(acc2), std::ref(acc1), 1, true);
                t7.join();
                t8.join();
                t9.join();
                t10.join();
                t11.join();
                t12.join();

            }
            else {
                throw(choice);
            }

            std::cout <<"\n\n\n\n" << std::endl;
        }
        catch(int invalid){
            std::cout <<"\n\n\n\n" << "\033[31m" << "["<< invalid << " is not a valid entry]\n[Please try again]" << "\033[0m" << std::endl;
        }
        
    }


    return 0;
}