#include<iostream>
#include<fstream>
#include<string>

#define red "\x1B[31m"
#define reset "\033[0m"
#define blue "\x1B[34m"
#define green "\x1B[32m"
#define yellow "\x1B[33m"
using namespace std;

// Structures to hold data
struct Bank
{
    int bankID;
    string bankname;
    string address;
};
struct BankBranch
{
    int branchID;
    string branchaddress;
    int bankID;
};
struct Account
{
    int accID;
    string acctype;
    int balance;
    int branchID;
};
struct Loan
{
    int loanID;
    string loantype;
    int amount;
    int branchID;
};
struct Customer
{
    int custID;
    string name;
    string phone;
    string address;
};
struct Cust_acc
{
    int custID;
    int accID;
    string date;
};
struct Cust_loan
{
    int custID;
    int loanID;
    string date;
};

bool ActiveLoan(int custID, int branchID);
void BackToMainMenu();

void CheckAndRemoveCustomerRelations(int customerID);
void UpdateCustomerRelations(int lastCustomerID, int newCustomerID);
void ShowHighBalanceCustomers();
void ShowCustomersWithoutLoans();
void ShowBranchesWithLoans();
void ShowLoansByCustomer();
void UserMenu();
void AdminMenu();
void BackToMainMenu();
int currentMenu = 0;

bool IsBankExist(int id)
{
    ifstream file("banks.txt");
    Bank* bank = new Bank;
    while (file >> bank->bankID >> bank->bankname >> bank->address)
    {
        if (bank->bankID == id)
        {
            delete bank;
            return true;
        }
    }
    delete bank;
    file.close();
    return false;
}
bool IsCustomerExist(int id)
{
    ifstream file("customers.txt");
    Customer* customer = new Customer;
    while (file >> customer->custID >> customer->name >> customer->phone >> customer->address)
    {
        if (customer->custID == id)
        {
            delete customer;
            return true;
        }
    }
    delete customer;
    file.close();
    return false;
}
bool IsBranchExist(int id)
{
    ifstream file("branches.txt");
    BankBranch* branch = new BankBranch;
    while (file >> branch->branchID >> branch->branchaddress >> branch->bankID)
    {
        if (branch->branchID == id)
        {
            delete branch;
            return true;
        }
    }
    delete branch;
    file.close();
    return false;
}
bool IsAccountExist(int id)
{
    ifstream file("accounts.txt");
    Account* account = new Account;
    while (file >> account->accID >> account->acctype >> account->balance >> account->branchID)
    {
        if (account->accID == id)
        {
            delete account;
            return true;
        }
    }
    delete account;
    file.close();
    return false;
}
bool IsLoanExist(int id)
{
    ifstream file("loans.txt");
    Loan* loan = new Loan;
    while (file >> loan->loanID >> loan->loantype >> loan->amount >> loan->branchID)
    {
        if (loan->loanID == id)
        {
            delete loan;
            return true;
        }
    }
    delete loan;
    file.close();
    return false;
}
bool IsLoanInBranch(int loanID, int branchID)
{
    ifstream loanFile("loans.txt");
    Loan loan;
    while (loanFile >> loan.loanID >> loan.amount >> loan.branchID)
    {
        if (loan.loanID == loanID && loan.branchID == branchID)
        {
            loanFile.close();
            return true;
        }
    }
    loanFile.close();
    return false;
}
bool ActiveLoan(int custID, int branchID)
{
    ifstream file("cust_loan.txt");
    if (!file)
    {
        cerr << "Error: Unable to open file!" << endl;
        return false;
    }

    Cust_loan custloan;
    while (file >> custloan.custID >> custloan.loanID >> custloan.date)
    {
        if (custloan.custID == custID && IsLoanInBranch(custloan.loanID, branchID))
        {
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

class UniqueMethods
{
public:
    void CheckAndRemoveCustomerRelations(int custID)
    {
        // Removing related accounts from cust_acc.txt
        ifstream accFile("cust_acc.txt");
        ofstream tempAccFile("temp_acc.txt");
        Cust_acc* custAcc = new Cust_acc;

        bool accountRemoved = false;

        while (accFile >> custAcc->custID >> custAcc->accID >> custAcc->date)
        {
            if (custAcc->custID == custID)
            {
                accountRemoved = true;
                cout << green << "Account " << custAcc->accID << " for Customer " << custID << " has been removed." << reset << endl;
            }
            else
            {
                tempAccFile << custAcc->custID << "   " << custAcc->accID << "   " << custAcc->date << endl;
            }
        }

        accFile.close();
        tempAccFile.close();

        if (accountRemoved)
        {
            remove("cust_acc.txt");
            rename("temp_acc.txt", "cust_acc.txt");
        }
        else
        {
            remove("temp_acc.txt");
            cout << "No accounts found for Customer " << custID << endl;
        }

        // Removing related loans from cust_loan.txt
        ifstream loanFile("cust_loan.txt");
        ofstream tempLoanFile("temp_loan.txt");
        Cust_loan* custLoan = new Cust_loan;

        bool loanRemoved = false;

        while (loanFile >> custLoan->custID >> custLoan->loanID >> custLoan->date)
        {
            if (custLoan->custID == custID)
            {
                loanRemoved = true;
                cout << green << "Loan " << custLoan->loanID << " for Customer " << custID << " has been removed." << reset << endl;
            }
            else
            {
                tempLoanFile << custLoan->custID << "   " << custLoan->loanID << "   " << custLoan->date << endl;
            }
        }

        loanFile.close();
        tempLoanFile.close();

        if (loanRemoved)
        {
            remove("cust_loan.txt");
            rename("temp_loan.txt", "cust_loan.txt");
        }
        else
        {
            remove("temp_loan.txt");
            cout << "No loans found for Customer " << custID << endl;
        }

        delete custAcc;
        delete custLoan;
    }

    void UpdateCustomerRelations(int lastCustomerID, int newCustomerID)
    {
        ifstream accFile("cust_acc.txt");
        ofstream tempAccFile("temp_acc.txt");
        Cust_acc* custAcc = new Cust_acc;

        while (accFile >> custAcc->custID >> custAcc->accID >> custAcc->date)
        {
            if (custAcc->custID == lastCustomerID)
            {
                custAcc->custID = newCustomerID;
                tempAccFile << custAcc->custID << "   " << custAcc->accID << "   " << custAcc->date << endl;
            }
        }

        accFile.close();
        tempAccFile.close();
        remove("cust_acc.txt");
        rename("temp_acc.txt", "cust_acc.txt");

        ifstream loanFile("cust_loan.txt");
        ofstream tempLoanFile("temp_loan.txt");
        Cust_loan* custLoan = new Cust_loan;

        while (loanFile >> custLoan->custID >> custLoan->loanID >> custLoan->date)
        {
            tempLoanFile << custLoan->custID << "   " << custLoan->loanID << "   " << custLoan->date << endl;
        }

        loanFile.close();
        tempLoanFile.close();
        remove("cust_loan.txt");
        rename("temp_loan.txt", "cust_loan.txt");

        delete custAcc;
        delete custLoan;
    }

    void ShowHighBalanceCustomers()
    {
        ifstream accFile("accounts.txt");
        ifstream custFile("customer.txt");
        ifstream custAccFile("cust_acc.txt");

        if (!accFile || !custFile || !custAccFile)
        {
            cerr << red << "Error: Unable to open files!" << reset << endl;
            return;
        }

        Account* account = new Account;
        Customer* customer = new Customer;
        Cust_acc* custAcc = new Cust_acc;

        cout << green << "Customers with balance above 250000:\n" << reset;

        while (accFile >> account->accID >> account->acctype >> account->balance >> account->branchID)
        {
            if (account->balance > 250000)
            {
                ifstream custAccFileInner("cust_acc.txt");
                while (custAccFileInner >> custAcc->custID >> custAcc->accID >> custAcc->date)
                {
                    if (custAcc->accID == account->accID)
                    {
                        ifstream custFileInner("customer.txt");
                        while (custFileInner >> customer->custID >> customer->name >> customer->phone >> customer->address)
                        {
                            if (customer->custID == custAcc->custID)
                            {
                                cout << "Customer ID: " << customer->custID
                                    << " | Name: " << customer->name
                                    << " | Phone: " << customer->phone
                                    << " | Address: " << customer->address
                                    << " | Balance: " << account->balance << endl;
                                break;
                            }
                        }
                        custFileInner.close();
                    }
                }
                custAccFileInner.close();
            }
        }

        delete account;
        delete customer;
        delete custAcc;

        accFile.close();
        custFile.close();
        custAccFile.close();

        BackToMainMenu();
    }

    void ShowCustomersWithoutLoans()
    {
        ifstream custFile("customer.txt");
        ifstream custLoanFile("cust_loan.txt");

        if (!custFile || !custLoanFile)
        {
            cerr << red << "Error: Unable to open files!" << reset << endl;
            return;
        }

        Customer* customer = new Customer;
        Cust_loan* custLoan = new Cust_loan;
        bool hasLoan = false;

        cout << green << "Customers without loans:\n" << reset;

        // Read each customer
        while (custFile >> customer->custID >> customer->name >> customer->phone >> customer->address)
        {
            // Reset the flag for each customer
            hasLoan = false;

            // Check if this customer ID exists in cust_loan.txt
            custLoanFile.clear();
            custLoanFile.seekg(0); // Reset the file pointer to the beginning
            while (custLoanFile >> custLoan->custID >> custLoan->loanID >> custLoan->date)
            {
                if (custLoan->custID == customer->custID)
                {
                    hasLoan = true;
                    break;
                }
            }

            // If customer has no loans, display their details
            if (!hasLoan)
            {
                cout << "Customer ID: " << customer->custID
                    << " | Name: " << customer->name
                    << " | Phone: " << customer->phone
                    << " | Address: " << customer->address << endl;
            }
        }

        delete customer;
        delete custLoan;

        custFile.close();
        custLoanFile.close();
    }

    void ShowBranchesWithLoans()
    {
        ifstream branchFile("branches.txt");
        ifstream loanFile("loans.txt");

        if (!branchFile || !loanFile)
        {
            cerr << red << "Error: Unable to open files!" << reset << endl;
            return;
        }

        BankBranch* branch = new BankBranch;
        Loan* loan = new Loan;
        bool hasLoan = false;

        cout << green << "Branches that have given loans:\n" << reset;

        // Read each branch
        while (branchFile >> branch->branchID >> branch->branchaddress >> branch->bankID)
        {
            // Reset the flag for each branch
            hasLoan = false;

            // Check if this branch ID exists in loans.txt
            loanFile.clear();
            loanFile.seekg(0); // Reset the file pointer to the beginning
            while (loanFile >> loan->loanID >> loan->loantype >> loan->amount >> loan->branchID)
            {
                if (loan->branchID == branch->branchID)
                {
                    hasLoan = true;
                    break;
                }
            }

            // If the branch has given loans, display its details
            if (hasLoan)
            {
                cout << "Branch ID: " << branch->branchID
                    << " | Address: " << branch->branchaddress
                    << " | Bank ID: " << branch->bankID << endl;
            }
        }

        delete branch;
        delete loan;

        branchFile.close();
        loanFile.close();
    }

    void ShowLoansByCustomer()
    {
        int custID;
        cout << blue << "Enter target customer: " << reset;
        cin >> custID;
        ifstream loanFile("loans.txt");
        ifstream custLoanFile("cust_loan.txt");

        if (!loanFile || !custLoanFile)
        {
            cerr << red << "Error: Unable to open files!" << reset << endl;
            return;
        }

        Loan* loan = new Loan;
        Cust_loan* custLoan = new Cust_loan;
        bool loanFound = false;

        cout << green << "Loans for Customer ID " << custID << ":\n" << reset;

        // Check each loan linked to the customer
        while (custLoanFile >> custLoan->custID >> custLoan->loanID >> custLoan->date)
        {
            if (custLoan->custID == custID)
            {
                loanFound = true;

                // Search for the loan details in loans.txt
                loanFile.clear();
                loanFile.seekg(0); // Reset the file pointer
                while (loanFile >> loan->loanID >> loan->loantype >> loan->amount >> loan->branchID)
                {
                    if (loan->loanID == custLoan->loanID)
                    {
                        cout << "Loan ID: " << loan->loanID
                            << " | Type: " << loan->loantype
                            << " | Amount: " << loan->amount
                            << " | Branch ID: " << loan->branchID
                            << " | Date: " << custLoan->date << endl;
                        break;
                    }
                }
            }
        }

        if (!loanFound)
        {
            cout << red << "No loans found for Customer ID " << custID << "." << reset << endl;
        }

        delete loan;
        delete custLoan;

        loanFile.close();
        custLoanFile.close();
    }
};
class BankManagment
{
public:
    // Add a bank
    void AddBank()
    {
        Bank* bank = new Bank;
        ofstream file("banks.txt", ios::app);
        if (!file)
        {
            cerr << "Error!!!!" << endl;
            delete bank;
            return;
        }
        cout << blue << "Enter a bank ID:" << reset;
        cin >> bank->bankID;

        if (IsBankExist(bank->bankID))
        {
            cout << red << "Bank has already asigned!" << reset << endl;
            BackToMainMenu();
            return;
        }

        cout << blue << "Enter bank name:" << reset;
        cin >> bank->bankname;
        cout << blue << "Enter the address:" << reset;
        cin >> bank->address;

        file << bank->bankID << "   " << bank->bankname << "  " << bank->address << endl;
        file.close();

        cout << green << "Bank added successfully" << reset << endl;
        delete bank;
        BackToMainMenu();
    }

    // Edit a bank
    void EditBank()
    {
        int bankID;
        cout << blue << "Enter bank ID to edit:" << reset;
        cin >> bankID;

        ifstream file("banks.txt");
        ofstream tempFile("temp_banks.txt");
        Bank bank;
        bool found = false;

        while (file >> bank.bankID >> bank.bankname >> bank.address)
        {
            if (bank.bankID == bankID)
            {
                found = true;
                cout << blue << "Enter new bank name:" << reset;
                cin >> bank.bankname;
                cout << blue << "Enter new bank address:" << reset;
                cin >> bank.address;
            }
            tempFile << bank.bankID << "   " << bank.bankname << "   " << bank.address << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("banks.txt");
            rename("temp_banks.txt", "banks.txt");
            cout << green << "Bank updated successfully" << reset;
        }
        else
        {
            cout << red << "Bank not found!" << reset;
        }

        BackToMainMenu();
    }

    // Remove a bank
    void RemoveBank()
    {
        int bankID;
        cout << blue << "Enter bank ID to remove:" << reset;
        cin >> bankID;

        ifstream file("banks.txt");
        ofstream tempFile("temp_banks.txt");
        Bank bank;
        bool found = false;

        while (file >> bank.bankID >> bank.bankname >> bank.address)
        {
            if (bank.bankID == bankID)
            {
                found = true;
                continue; // Skip the bank to remove it
            }
            tempFile << bank.bankID << "   " << bank.bankname << "   " << bank.address << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("banks.txt");
            rename("temp_banks.txt", "banks.txt");
            cout << green << "Bank removed successfully" << reset;
        }
        else
        {
            cout << red << "Bank not found!" << reset;
        }
        BackToMainMenu();
    }

    // View all banks
    void ViewBank()
    {
        Bank* bank = new Bank;
        ifstream file("banks.txt");
        if (!file)
        {
            cerr << "nothing found!" << endl;
            delete bank;
            return;
        }
        cout << "Banks:" << endl;
        while (file >> bank->bankID >> bank->bankname >> bank->address)
        {
            cout << "  ID: " << bank->bankID << " | Name: " << bank->bankname << " | Address: " << bank->address << endl;
        }
        file.close();

        delete bank;
        BackToMainMenu();
    }

    // Add a branch
    void AddBranch()
    {
        BankBranch* branch = new BankBranch;
        ofstream file("branches.txt", ios::app);
        if (!file)
        {
            cerr << "Error!!!!" << endl;
            delete branch;
            return;
        }
        cout << blue << "Enter branch ID:" << reset;
        cin >> branch->branchID;

        if (IsBranchExist(branch->branchID))
        {
            cout << red << "Bank Branch has already asigned!" << reset << endl;
            BackToMainMenu();
            return;
        }
        cout << blue << "Enter branch address:" << reset;
        cin >> branch->branchaddress;
        cout << blue << "Enter bank ID for the branch:" << reset;
        cin >> branch->bankID;

        file << branch->branchID << "   " << branch->branchaddress << "   " << branch->bankID << endl;
        file.close();

        cout << green << "Branch added successfully" << reset << endl;
        delete branch;
        BackToMainMenu();
    }

    // Edit a branch
    void EditBranch()
    {
        int branchID;
        cout << blue << "Enter branch ID to edit:" << reset;
        cin >> branchID;

        ifstream file("branches.txt");
        ofstream tempFile("temp_branches.txt");
        BankBranch branch;
        bool found = false;

        while (file >> branch.branchID >> branch.branchaddress >> branch.bankID)
        {
            if (branch.branchID == branchID)
            {
                found = true;
                cout << blue << "Enter new branch address:" << reset;
                cin >> branch.branchaddress;
                cout << blue << "Enter new bank ID for the branch:" << reset;
                cin >> branch.bankID;
            }
            tempFile << branch.branchID << "   " << branch.branchaddress << "   " << branch.bankID << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("branches.txt");
            rename("temp_branches.txt", "branches.txt");
            cout << green << "Branch updated successfully" << reset;
        }
        else
        {
            cout << red << "Branch not found!" << reset;
        }
        BackToMainMenu();
    }

    // Remove a branch
    void RemoveBranch()
    {
        int branchID;
        cout << blue << "Enter branch ID to remove:" << reset;
        cin >> branchID;

        ifstream file("branches.txt");
        ofstream tempFile("temp_branches.txt");
        BankBranch branch;
        bool found = false;

        while (file >> branch.branchID >> branch.branchaddress >> branch.bankID)
        {
            if (branch.branchID == branchID)
            {
                found = true;
                continue; // Skip the branch to remove it
            }
            tempFile << branch.branchID << "   " << branch.branchaddress << "   " << branch.bankID << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("branches.txt");
            rename("temp_branches.txt", "branches.txt");
            cout << green << "Branch removed successfully" << reset;
        }
        else
        {
            cout << red << "Branch not found!" << reset;
        }
        BackToMainMenu();
    }

    // View all branches
    void ViewBranch()
    {
        BankBranch* branch = new BankBranch;
        ifstream file("branches.txt");
        if (!file)
        {
            cerr << "nothing found!" << endl;
            delete branch;
            return;
        }
        cout << "Branches:" << endl;
        while (file >> branch->branchID >> branch->branchaddress >> branch->bankID)
        {
            cout << "  Branch ID: " << branch->branchID << " | Address: " << branch->branchaddress << " | Bank ID: " << branch->bankID << endl;
        }
        file.close();

        delete branch;
        BackToMainMenu();
    }
};
class CustomerManagment
{
public:
    // Add a customer
    void AddCustomer()
    {
        Customer* customer = new Customer;
        cout << blue << "Enter customer ID:" << reset;
        cin >> customer->custID;

        if (IsCustomerExist(customer->custID))
        {
            cout << red << "Customer has already asigned!" << reset << endl;
            BackToMainMenu();
            return;
        }

        cout << blue << "Enter name:" << reset;
        cin >> customer->name;
        cout << blue << "Enter phone:" << reset;
        cin >> customer->phone;
        cout << blue << "Enter address:" << reset;
        cin >> customer->address;

        ofstream file("customer.txt", ios::app);
        if (file)
        {
            file << customer->custID << "   " << customer->name << "   " << customer->phone << "    " << customer->address << endl;
            file.close();
            system("cls");
            cout << green << "Customer added successfully" << reset << endl;
        }
        delete customer;
        BackToMainMenu();
    }

    // Edit a customer
    void EditCustomer()
    {
        UniqueMethods unique_methods;
        int custID;
        cout << blue << "Enter customer ID to edit:" << reset;
        cin >> custID;

        ifstream file("customer.txt");
        ofstream tempFile("temp_customer.txt");
        Customer customer;
        bool found = false;

        while (file >> customer.custID >> customer.name >> customer.phone >> customer.address)
        {
            if (customer.custID == custID)
            {
                found = true;
                cout << blue << "Enter new customer ID:" << reset;
                cin >> customer.custID;
                cout << blue << "Enter new customer name:" << reset;
                cin >> customer.name;
                cout << blue << "Enter new phone number:" << reset;
                cin >> customer.phone;
                cout << blue << "Enter new address:" << reset;
                cin >> customer.address;
            }
            tempFile << customer.custID << "   " << customer.name << "   " << customer.phone << "    " << customer.address << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("customer.txt");
            rename("temp_customer.txt", "customer.txt");
            cout << green << "Customer updated successfully" << reset;
           unique_methods.UpdateCustomerRelations(custID, customer.custID);
        }
        else
        {
            cout << red << "Customer not found!" << reset;
        }
        BackToMainMenu();
    }

    // Remove a customer
    void RemoveCustomer()
    {
        UniqueMethods unique_methods;
        int custID;
        cout << blue << "Enter customer ID to remove:" << reset;
        cin >> custID;

        ifstream file("customer.txt");
        ofstream tempFile("temp_customer.txt");
        Customer customer;
        bool found = false;

        while (file >> customer.custID >> customer.name >> customer.phone >> customer.address)
        {
            if (customer.custID == custID)
            {
                found = true;
                continue; // Skip the customer to remove it
            }
            tempFile << customer.custID << "   " << customer.name << "   " << customer.phone << "    " << customer.address << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("customer.txt");
            rename("temp_customer.txt", "customer.txt");
            cout << green << "Customer removed successfully" << reset << endl;
        }
        else
        {
            cout << red << "Customer not found!" << reset;
        }

       unique_methods.CheckAndRemoveCustomerRelations(custID);
        BackToMainMenu();
    }

    // View all customers
    void ViewCustomer()
    {
        Customer* customer = new Customer;
        ifstream file("customer.txt");
        if (!file)
        {
            cerr << "nothing found!" << endl;
            delete customer;
            return;
        }
        cout << "Customers:" << endl;
        while (file >> customer->custID >> customer->name >> customer->phone >> customer->address)
        {
            cout << "  ID: " << customer->custID << " | Name: " << customer->name << " | Phone: " << customer->phone << " | Address: " << customer->address << endl;
        }
        file.close();

        delete customer;
        BackToMainMenu();
    }
};
class AccountManagment
{
public:
    // Add an account
    void AddAccount()
    {
        Account* account = new Account;
        cout << blue << "Enter Account ID:" << reset;
        cin >> account->accID;

        if (IsAccountExist(account->accID))
        {
            cout << red << "Account has already asigned!" << reset << endl;
            BackToMainMenu();
            return;
        }

        cout << blue << "Enter Account type:" << reset;
        cin >> account->acctype;
        cout << blue << "Enter balance:" << reset;
        cin >> account->balance;

        cout << blue << "Enter branch ID:" << reset;
        cin >> account->branchID;

        int custID;
        cout << blue << "Enter customer ID:" << reset;
        cin >> custID;

        if (IsCustomerExist(custID))
        {
            cout << red << "Error: Customer not found " << reset << endl;
            delete account;
            return;
        }

        ofstream file("accounts.txt", ios::app);
        file << account->accID << "   " << account->acctype << "   " << account->balance << "   " << account->branchID << endl;
        file.close();

        ofstream cudtaccfile("cust_acc.txt", ios::app);
        cudtaccfile << custID << "   " << account->accID << "   " << "2024/12/25" << endl;
        cudtaccfile.close();

        cout << green << "Account added successfully" << reset << endl;

        delete account;
        BackToMainMenu();
    }

    // Edit an account
    void EditAccount()
    {
        int accID;
        cout << blue << "Enter account ID to edit:" << reset;
        cin >> accID;

        ifstream file("accounts.txt");
        ofstream tempFile("temp_accounts.txt");
        Account account;
        bool found = false;

        while (file >> account.accID >> account.acctype >> account.balance >> account.branchID)
        {
            if (account.accID == accID)
            {
                found = true;
                cout << blue << "Enter new account type:" << reset;
                cin >> account.acctype;
                cout << blue << "Enter new balance:" << reset;
                cin >> account.balance;
                cout << blue << "Enter new branch ID:" << reset;
                cin >> account.branchID;
            }
            tempFile << account.accID << "   " << account.acctype << "   " << account.balance << "   " << account.branchID << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("accounts.txt");
            rename("temp_accounts.txt", "accounts.txt");
            cout << green << "Account updated successfully" << reset;
        }
        else
        {
            cout << red << "Account not found!" << reset;
        }
        BackToMainMenu();
    }

    // Remove an account
    void RemoveAccount()
    {
        int accID;
        cout << blue << "Enter account ID to remove:" << reset;
        cin >> accID;

        ifstream file("accounts.txt");
        ofstream tempFile("temp_accounts.txt");
        Account account;
        bool found = false;

        while (file >> account.accID >> account.acctype >> account.balance >> account.branchID)
        {
            if (account.accID == accID)
            {
                found = true;
                continue; // Skip the account to remove it
            }
            tempFile << account.accID << "   " << account.acctype << "   " << account.balance << "   " << account.branchID << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("accounts.txt");
            rename("temp_accounts.txt", "accounts.txt");
            cout << green << "Account removed successfully" << reset;
        }
        else
        {
            cout << red << "Account not found!" << reset;
        }
        BackToMainMenu();
    }

    // View all accounts
    void ViewAccount()
    {
        Account* account = new Account;
        ifstream file("accounts.txt");
        if (!file)
        {
            cerr << "nothing found!" << endl;
            delete account;
            return;
        }
        cout << "Accounts:" << endl;
        while (file >> account->accID >> account->acctype >> account->balance >> account->branchID)
        {
            cout << "  Account ID: " << account->accID << " | Type: " << account->acctype << " | Balance: " << account->balance << " | Branch ID: " << account->branchID << endl;
        }
        file.close();

        delete account;
        BackToMainMenu();
    }
};
class LoanManagment
{
public:
    // Add a loan
    void AddLoan()
    {

        Cust_loan* custloan = new Cust_loan;



        Loan* loan = new Loan;
        cout << blue << "Enter loan ID:" << reset;
        cin >> loan->loanID;

        if (IsLoanExist(loan->loanID))
        {
            cout << red << "Loan has already asigned!" << reset << endl;
            BackToMainMenu();
            return;
        }

        cout << blue << "Enter customer ID:" << reset;
        cin >> custloan->custID;

        if (IsCustomerExist(custloan->custID))
        {
            cout << red << "Customer not found" << reset << endl;
            BackToMainMenu();
            return;
        }

        cout << blue << "Enter loan type:" << reset;
        cin >> loan->loantype;
        cout << blue << "Enter loan amount:" << reset;
        cin >> loan->amount;
        cout << blue << "Enter branch ID:" << reset;
        cin >> loan->branchID;
        cout << blue << "Enter date:" << reset;
        cin >> custloan->date;

        if (!IsBranchExist(loan->branchID))
        {
            cout << red << "Unkown Branch ID" << reset << endl;
            BackToMainMenu();
            return;
        }

        if (ActiveLoan(custloan->custID,loan->branchID))
        {
            cout << red << "The loan has asigned by the customer with this branchID" << reset << endl;
            BackToMainMenu();
            return;
        }

        ofstream file("loans.txt", ios::app);
        if (file)
        {
            file << loan->loanID << "   " << loan->loantype << "   " << loan->amount << "   " << loan->branchID << endl;
            file.close();
            system("cls");
            cout << green << "Loan added successfully" << reset << endl;
        }

        ofstream file2("cust_loan.txt", ios::app);
        file << custloan->custID << "   " << loan->loanID << "   " << custloan->date << endl;
        file.close();
        delete loan;
        delete custloan;
        BackToMainMenu();
    }

    // Edit a loan
    void EditLoan()
    {
        int loanID;
        cout << blue << "Enter loan ID to edit:" << reset;
        cin >> loanID;

        ifstream file("loans.txt");
        ofstream tempFile("temp_loans.txt");
        Loan loan;
        bool found = false;

        while (file >> loan.loanID >> loan.loantype >> loan.amount >> loan.branchID)
        {
            if (loan.loanID == loanID)
            {
                found = true;
                cout << blue << "Enter new loan type:" << reset;
                cin >> loan.loantype;
                cout << blue << "Enter new loan amount:" << reset;
                cin >> loan.amount;
                cout << blue << "Enter new branch ID:" << reset;
                cin >> loan.branchID;
            }
            tempFile << loan.loanID << "   " << loan.loantype << "   " << loan.amount << "   " << loan.branchID << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("loans.txt");
            rename("temp_loans.txt", "loans.txt");
            cout << green << "Loan updated successfully" << reset;
        }
        else
        {
            cout << red << "Loan not found!" << reset;
        }
        BackToMainMenu();
    }

    // Remove a loan
    void RemoveLoan()
    {
        int loanID;
        cout << blue << "Enter loan ID to remove:" << reset;
        cin >> loanID;

        ifstream file("loans.txt");
        ofstream tempFile("temp_loans.txt");
        Loan loan;
        bool found = false;

        while (file >> loan.loanID >> loan.loantype >> loan.amount >> loan.branchID)
        {
            if (loan.loanID == loanID)
            {
                found = true;
                continue; // Skip the loan to remove it
            }
            tempFile << loan.loanID << "   " << loan.loantype << "   " << loan.amount << "   " << loan.branchID << endl;
        }
        file.close();
        tempFile.close();

        if (found)
        {
            remove("loans.txt");
            rename("temp_loans.txt", "loans.txt");
            cout << green << "Loan removed successfully" << reset;
        }
        else
        {
            cout << red << "Loan not found!" << reset;
        }
        BackToMainMenu();
    }

    // View all loans
    void ViewLoan()
    {
        Loan* loan = new Loan;
        ifstream file("loans.txt");
        if (!file)
        {
            cerr << "nothing found!" << endl;
            delete loan;
            return;
        }
        cout << "Loans:" << endl;
        while (file >> loan->loanID >> loan->loantype >> loan->amount >> loan->branchID)
        {
            cout << "  Loan ID: " << loan->loanID << " | Type: " << loan->loantype << " | Amount: " << loan->amount << " | Branch ID: " << loan->branchID << endl;
        }
        file.close();

        delete loan;
        BackToMainMenu();
    }
};
class UserInterface
{

public:
    // Bank menu
    void BankMenu()
    {
        BankManagment bank_managment;
        UniqueMethods unique_methods;
        system("cls");
        int choice;
        cout << yellow << "\n------Bank Menu------\n" << reset;
        cout << "1. Add Bank\n2. View Banks\n3. Edit Banks\n4. Remove Banks\n"
                "5. Add Branch\n6. Edit Branches\n7. Remove Branches\n8. View Branches\n9. Show Given Loan Branches"
                "\n0. Back" << endl;
        cout << blue << "Enter your choice: " << reset;
        cin >> choice;

        switch (choice)
        {
        case 1: bank_managment.AddBank();
            break;
        case 2: bank_managment.ViewBank();
            break;
        case 3: bank_managment.EditBank();
            break;
        case 4: bank_managment.RemoveBank();
            break;
        case 5: bank_managment.AddBranch();
            break;
        case 6: bank_managment.EditBranch();
            break;
        case 7: bank_managment.RemoveBank();
            break;
        case 8: bank_managment.ViewBranch();
            break;
        case 9:unique_methods.ShowBranchesWithLoans();
            break;
        case 0: BackToMainMenu();
            break;
        default: cout << red << "Invalid choice, try again." << reset << endl;
            BankMenu();
            break;
        }
    }

    // Customer menu
    void CustomerMenu()
    {
        CustomerManagment customer_managment;
        UniqueMethods unique_methods;
        system("cls");
        int choice;
        cout << yellow << "\n------Customer Menu------\n" << reset;
        cout << "1. Add Customer\n2. View Customers\n3. Edit Customers\n4. Remove Customers"
            "\n5. Show Customers Above 25000 Balance\n6. Show Customers Without Loan\n0. Back" << endl;
        cout << blue << "Enter your choice: " << reset;
        cin >> choice;

        switch (choice)
        {
        case 1: customer_managment.AddCustomer();
            break;
        case 2: customer_managment.ViewCustomer();
            break;
        case 3: customer_managment.EditCustomer();
            break;
        case 4: customer_managment.RemoveCustomer();
            break;
        case 5: unique_methods.ShowHighBalanceCustomers();
            break;
        case 6: unique_methods.ShowCustomersWithoutLoans();
            break;
        case 0: BackToMainMenu();
            break;
        default: cout << red << "Invalid choice, try again." << reset << endl;
            CustomerMenu();
            break;
        }
    }

    // Account menu
    void AccountMenu()
    {
        AccountManagment account_managment;

        system("cls");
        int choice;
        cout << yellow << "\n------Account Menu------\n" << reset;
        cout << "1. Add Account\n2. View Accounts\n3. Edit Accounts\n4. Remove Accounts\n0. Back" << endl;
        cout << blue << "Enter your choice: " << reset;
        cin >> choice;

        switch (choice)
        {
        case 1: account_managment.AddAccount();
            break;
        case 2: account_managment.ViewAccount();
            break;
        case 3: account_managment.EditAccount();
            break;
        case 4: account_managment.RemoveAccount();
            break;
        case 0: BackToMainMenu();
            break;
        default: cout << red << "Invalid choice, try again." << reset << endl;
            AccountMenu();
            break;
        }
    }

    // Loan menu
    void LoanMenu()
    {
        LoanManagment loan_managment;
        UniqueMethods unique_methods;
        system("cls");
        int choice;
        cout << yellow << "\n------Loan Menu------\n" << reset;
        cout << "1. Add Loan\n2. View Loans\n3. Edit Loans\n4. Remove Loans\n5. Given Loans\n0. Back" << endl;
        cout << blue << "Enter your choice: " << reset;
        cin >> choice;

        switch (choice)
        {
        case 1: loan_managment.AddLoan();
            break;
        case 2: loan_managment.ViewLoan();
            break;
        case 3: loan_managment.EditLoan();
            break;
        case 4: loan_managment.RemoveLoan();
            break;
        case 5: unique_methods.ShowLoansByCustomer();
            break;
        case 0: BackToMainMenu();
            break;
        default: cout << red << "Invalid choice, try again." << reset << endl;
            LoanMenu();
            break;
        }
    }

    // Admin menu
    void AdminMenu()
    {
        system("cls");

        int choice;
        cout << yellow << "\n------------Main Menu------------\n" << reset;
        cout << "1. Bank Menu\n2. Customer Menu\n3. Account Menu\n4. Loan Menu\n0. Exit" << endl;
        cout << blue << "Enter your choice: " << reset;
        cin >> choice;

        switch (choice)
        {
        case 1: BankMenu();
            break;
        case 2: CustomerMenu();
            break;
        case 3: AccountMenu();
            break;
        case 4: LoanMenu();
            break;
        case 0: exit(0);
            break;
        default: cout << red << "\nInvalid choice" << reset << endl;
            AdminMenu();
            break;
        }
    }

    void UserMenu()
    {
        BankManagment bank_managment;
        CustomerManagment customer_managment;
        AccountManagment account_managment;
        LoanManagment loan_managment;
        system("cls");
        int choice;
        cout << yellow << "\n------------User Menu------------\n" << reset;
        cout << "1. View Banks\n2. View Branches\n3. View Customers\n4. View Accounts\n5. View Loans\n0. Exit" << endl;
        cout << blue << "Enter your choice: " << reset;
        cin >> choice;

        switch (choice)
        {
        case 1: bank_managment.ViewBank();
            break;
        case 2: bank_managment.ViewBranch();
            break;
        case 3: customer_managment.ViewCustomer();
            break;
        case 4: account_managment.ViewAccount();
            break;
        case 5: loan_managment.ViewLoan();
            break;
        case 0: exit(0);
            break;
        default: cout << red << "\nInvalid choice" << reset << endl;
            UserMenu();
            break;
        }
    }

    int LogIn()
    {
        string username, password;
        cout << blue << "Enter your username:" << reset;
        cin >> username;
        cout << blue << "Enter your password:" << reset;
        cin >> password;

        if (username == "Admin" && password == "12345")
        {
            currentMenu = 0;
            return 1;
        }
        else if (username == "User" && password == "54321")
        {
            currentMenu = 1;
            return 2;
        }
        else
        {
            cout << red << "Invalid password or username, please try again later!" << reset << endl;
            return 0;
        }
    }
};

void BackToMainMenu()
{
    UserInterface user_interface;
    int choice;
    cout << blue << "Enter '0' to go back to the main menu: " << reset;
    cin >> choice;
    if (choice == 0)
    {
        if (currentMenu == 0)
        {
           user_interface.AdminMenu();
        }
        else if (currentMenu == 1)
        {
           user_interface.UserMenu();
        }
    }
    else
    {
        cout << red << "Invalid value, please try again later" << reset;
    }
}

void main()
{
    UserInterface user_interface;
    int entry = user_interface.LogIn();
    if (entry == 1)
    {
       user_interface.AdminMenu();
    }
    else if (entry == 2)
    {
       user_interface.UserMenu();
    }
}
