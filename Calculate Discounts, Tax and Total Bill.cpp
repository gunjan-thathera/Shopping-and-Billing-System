#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// Structure
struct BillDetails {
    int id, quantity;
    string name;
    float price;
};

// Function Template
template <class T>
T calculateTax(T amount, T rate) {
    return amount * rate / 100;
}

// Class Template
template <class T>
class BillSummary {
public:
    T subtotal, discount, tax, total;

    BillSummary() {
        subtotal = discount = tax = total = 0;
    }

    void display() {
        cout << "\nSubtotal: " << subtotal;
        cout << "\nDiscount: " << discount;
        cout << "\nGST: " << tax;
        cout << "\nFinal Bill: " << total << endl;
    }
};

// Bill Item Class
class BillItem {
protected:
    BillDetails item;
    const int itemType;

public:
    static int totalItems;

    // Constructor
    BillItem() : itemType(1) {
        item = {0, 0, "", 0};
    }

    BillItem(int id, string name, int qty, float price)
        : itemType(1) {
        item = {id, qty, name, price};
    }

    // Destructor
    virtual ~BillItem() {}

    // Constant Member Function
    int getID() const { return item.id; }
    int getQuantity() const { return item.quantity; }
    float getPrice() const { return item.price; }
    string getName() const { return item.name; }

    float getSubtotal() const {
        return item.price * item.quantity;
    }

    // Operator Overloading
    float operator+(float amount) const {
        return getSubtotal() + amount;
    }

    BillItem& operator=(const BillItem &x) {
        if (this != &x)
            item = x.item;
        return *this;
    }

    virtual void display() const {
        cout << item.id << "\t" << item.name << "\t"
             << item.price << "\t" << item.quantity
             << "\t" << getSubtotal() << endl;
    }

    // Friend Function
    friend void showItem(const BillItem &x);

    // Static Member Function
    static void showTotalItems() {
        cout << "Total Items: " << totalItems << endl;
    }
};

int BillItem::totalItems = 0;

void showItem(const BillItem &x) {
    cout << "Product: " << x.item.name
         << "\nPrice: " << x.item.price << endl;
}

// Inheritance
class DiscountCalculator {
protected:
    float rate;

public:
    DiscountCalculator(float r) {
        rate = r;
    }

    virtual float calculateDiscount(float amount) {
        return 0;
    }

    virtual ~DiscountCalculator() {}
};

// Function Overriding
class PercentageDiscount : public DiscountCalculator {
public:
    PercentageDiscount(float r) : DiscountCalculator(r) {}

    float calculateDiscount(float amount) override {
        return amount * rate / 100;
    }
};

// Billing System
class BillingSystem {
    BillItem *items;             // Dynamic Memory Allocation
    int count, capacity;
    const string billName;

    static int totalBills;

public:
    BillingSystem() : billName("Shopping Bill") {
        capacity = 50;
        count = 0;
        items = new BillItem[capacity]; // Array of Objects
    }

    ~BillingSystem() {
        delete[] items;
    }

    // Function Overloading
    void addItem(int id, string name, int qty, float price) {
        BillItem x(id, name, qty, price);
        addItem(x);
    }

    void addItem(const BillItem &x) {
        if (x.getID() <= 0 || x.getQuantity() <= 0 ||
            x.getPrice() < 0)
            throw 1;

        if (count == capacity)
            throw 2;

        for (int i = 0; i < count; i++) {
            if (items[i].getID() == x.getID())
                throw 3;
        }

        items[count++] = x;
        BillItem::totalItems += x.getQuantity();

        cout << "Product added successfully!\n";
    }

    void displayItems() {
        cout << "\nID\tName\tPrice\tQty\tAmount\n";

        for (int i = 0; i < count; i++)
            items[i].display();
    }

    // Calculate Discount, Tax and Total
    BillSummary<float> calculateBill(DiscountCalculator *d) {
        if (count == 0)
            throw 4;

        BillSummary<float> bill;

        for (int i = 0; i < count; i++)
            bill.subtotal = items[i] + bill.subtotal;

        bill.discount = d->calculateDiscount(bill.subtotal);

        float taxableAmount = bill.subtotal - bill.discount;

        bill.tax = calculateTax(taxableAmount, 5.0f);

        bill.total = taxableAmount + bill.tax;

        return bill;
    }

    // Function Overloading
    void generateBill(DiscountCalculator *d) {
        cout << "\n========== FINAL BILL ==========\n";
        cout << "Product\tQty\tAmount\n";

        for (int i = 0; i < count; i++) {
            cout << items[i].getName() << "\t"
                 << items[i].getQuantity() << "\t"
                 << items[i].getSubtotal() << endl;
        }

        BillSummary<float> bill = calculateBill(d);
        bill.display();

        totalBills++;
    }

    void generateBill(float discountRate) {
        if (discountRate < 0 || discountRate > 100)
            throw 5;

        PercentageDiscount d(discountRate);
        generateBill(&d);
    }

    // File Handling
    void saveBill(float discountRate) {
        ofstream file("bill.txt");

        if (!file)
            throw 6;

        BillSummary<float> bill = calculateBill(
            new PercentageDiscount(discountRate)
        );

        file << billName << "\n";
        file << "Product\tQuantity\tPrice\n";

        for (int i = 0; i < count; i++) {
            file << items[i].getName() << "\t"
                 << items[i].getQuantity() << "\t"
                 << items[i].getPrice() << "\n";
        }

        file << "\nSubtotal: " << bill.subtotal;
        file << "\nDiscount: " << bill.discount;
        file << "\nGST: " << bill.tax;
        file << "\nFinal Bill: " << bill.total;

        file.close();
        cout << "Bill saved successfully!\n";
    }

    void loadItems() {
        ifstream file("billitems.txt");
        string line;

        if (!file)
            throw 6;

        while (getline(file, line)) {
            stringstream ss(line);
            int id, qty;
            string name;
            float price;

            ss >> id;
            ss.ignore(1);
            getline(ss, name, '|');
            ss >> qty;
            ss.ignore(1);
            ss >> price;

            if (ss && id > 0 && qty > 0 && price >= 0)
                addItem(id, name, qty, price);
        }

        file.close();
        cout << "Items loaded successfully!\n";
    }

    // Static Member Function
    static void showTotalBills() {
        cout << "Bills Generated: " << totalBills << endl;
    }
};

int BillingSystem::totalBills = 0;

// Main Function
int main() {
    BillingSystem bill;

    int choice, id, qty;
    string name;
    float price, discount;

    do {
        cout << "\n===== BILLING SYSTEM =====\n";
        cout << "1. Add Product\n";
        cout << "2. Display Products\n";
        cout << "3. Calculate Final Bill\n";
        cout << "4. Save Bill\n";
        cout << "5. Load Products\n";
        cout << "6. Display Total Items\n";
        cout << "7. Display Bills Generated\n";
        cout << "8. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        try {
            switch (choice) {
            case 1:
                cout << "Enter ID, Name, Quantity and Price: ";
                cin >> id >> name >> qty >> price;
                bill.addItem(id, name, qty, price);
                break;

            case 2:
                bill.displayItems();
                break;

            case 3:
                cout << "Enter Discount Percentage: ";
                cin >> discount;
                bill.generateBill(discount);
                break;

            case 4:
                cout << "Enter Discount Percentage: ";
                cin >> discount;

                if (discount < 0 || discount > 100)
                    throw 5;

                bill.saveBill(discount);
                break;

            case 5:
                bill.loadItems();
                break;

            case 6:
                BillItem::showTotalItems();
                break;

            case 7:
                BillingSystem::showTotalBills();
                break;

            case 8:
                cout << "Thank you!\n";
                break;

            default:
                cout << "Invalid choice!\n";
            }
        }
        catch (int e) {
            cout << "Error! Error Code: " << e << endl;
        }

    } while (choice != 0);

    return 0;
}
