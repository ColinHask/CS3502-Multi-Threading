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
            std::cout << "                          |   [Account " << ID << "] " << amount << "$ widthdrawl sucessful" << std::endl;
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
        std::cout << "                          |   [Account " << ID << "] " << amount << "$ deposit sucessful" << std::endl;
        std::cout << "                          |   [Account " << ID << "]" << " Current Balance: " << Balance << std::endl;
        return true;
    }

    std::cout << "                          |   [Account " << ID << "]" << amount << "$ widthdrawl failed, other modification in process" << std::endl;
    return false;
    }

};


// helper method to validate user input with integers
// max = maximum valid int for input
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
            std::cout << "__________________________" << std::endl;
            std::cout << "[Bank Account Operations] |" << std::endl;
            std::cout << "Widthdrawal  -------- 0   |" << std::endl;
            std::cout << "Deposit      -------- 1   |" << std::endl;
            std::cout << "Transfer     -------- 2   |" << std::endl;
            std::cout << "==========================|" << std::endl;
            std::cout << "[Simulate Deadlock] - 3   |" << std::endl;
            std::cout << "[Resolve Deadlock] -- 4   |" << std::endl;
            std::cout << "                          |" << std::endl;
            // std::cout << "Enter choice:             |\n";
            // std::cin >> choice;
            choice = readInt("Enter choice:             |\n", 4);

            if (choice == 0){
                int amt;
                int acc;
                acc = readInt("Enter Account number:     |\n", 2);
                amt = readInt("Enter Widthdrawal amount: |\n", 2000000000);

                if (acc == 1){
                    std::thread t1(&BankAccount::withdraw, &acc1, amt);
                    t1.detach();
                }else if (acc == 2){
                    std::thread t1(&BankAccount::withdraw, &acc2, amt);
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
                    std::thread t1(&BankAccount::deposit, &acc1, amt);
                    t1.detach();
                }else if (acc == 2){
                    std::thread t1(&BankAccount::deposit, &acc2, amt);
                    t1.detach();
                }else{                                     
                    std::cout << "Invalid entry             |" << std::endl;
                }
                
            }else{
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