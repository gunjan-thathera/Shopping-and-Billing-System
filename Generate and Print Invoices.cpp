#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// Structure
struct InvoiceDetails {
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
class InvoiceTotal {
public:
    T subtotal, discount, tax, total;

    InvoiceTotal() {
        subtotal = discount = tax = total = 0;
    }

    void display() {
        cout << "\nSubtotal: Rs. " << subtotal;
        cout << "\nDiscount: Rs. " << discount;
        cout << "\nGST: Rs. " << tax;
        cout << "\nFinal Amount: Rs. " << total << endl;
    }
};

// Invoice Item Class
class InvoiceItem {
protected:
    InvoiceDetails item;
    const int itemType;

public:
    static int totalItems;

    // Constructors
    InvoiceItem() : itemType(1) {
        item = {0, 0, "", 0};
    }

    InvoiceItem(int id, string name, int qty, float price)
        : itemType(1) {
        item = {id, qty, name, price};
    }

    // Destructor
    virtual ~InvoiceItem() {}

    // Constant Member Functions
    int getID() const { return item.id; }
    int getQuantity() const { return item.quantity; }
    float getPrice() const { return item.price; }
    string getName() const { return item.name; }

    float getAmount() const {
        return item.price * item.quantity;
    }

    // Operator Overloading
    float operator+(float amount) const {
        return getAmount() + amount;
    }

    InvoiceItem& operator=(const InvoiceItem &x) {
        if (this != &x)
            item = x.item;
        return *this;
    }

    virtual void display(ostream &out) const {
        out << item.id << "\t" << item.name << "\t"
            << item.quantity << "\t" << item.price
            << "\t" << getAmount() << endl;
    }

    // Friend Function
    friend void showCustomer(const class Invoice &x);

    // Static Member Function
    static void showTotalItems() {
        cout << "Total Item Units: " << totalItems << endl;
    }
};

int InvoiceItem::totalItems = 0;

// Inheritance and Function Overriding
class InvoicePrinter {
public:
    virtual void printHeader(ostream &out) const {
        out << "INVOICE\n";
    }

    virtual ~InvoicePrinter() {}
};

// Invoice Class
class Invoice : public InvoicePrinter {
    InvoiceItem *items;
    int count, capacity;

    string customerName, phone;
    const string invoiceTitle;

    static int invoiceCount;

public:
    // Constructor
    Invoice(string name, string number)
        : customerName(name), phone(number),
          invoiceTitle("SHOPPING AND BILLING SYSTEM") {
        capacity = 50;
        count = 0;
        items = new InvoiceItem[capacity];
    }

    // Destructor
    ~Invoice() {
        for (int i = 0; i < count; i++)
            InvoiceItem::totalItems -= items[i].getQuantity();

        delete[] items;
    }

    // Function Overloading
    void addItem(int id, string name, int qty, float price) {
        InvoiceItem x(id, name, qty, price);
        addItem(x);
    }

    void addItem(const InvoiceItem &x) {
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
        InvoiceItem::totalItems += x.getQuantity();

        cout << "Product added successfully!\n";
    }

    // Function Overriding
    void printHeader(ostream &out) const override {
        out << "\n====================================\n";
        out << invoiceTitle << endl;
        out << "             TAX INVOICE\n";
        out << "====================================\n";
    }

    // Calculate Invoice
    InvoiceTotal<float> calculateInvoice(float discountRate) {
        if (count == 0)
            throw 4;

        if (discountRate < 0 || discountRate > 100)
            throw 5;

        InvoiceTotal<float> bill;

        for (int i = 0; i < count; i++)
            bill.subtotal = items[i] + bill.subtotal;

        bill.discount = bill.subtotal * discountRate / 100;

        float taxableAmount = bill.subtotal - bill.discount;

        bill.tax = calculateTax(taxableAmount, 5.0f);

        bill.total = taxableAmount + bill.tax;

        return bill;
    }

    // Generate and Display Invoice
    void generateInvoice(float discountRate) {
        InvoiceTotal<float> bill = calculateInvoice(discountRate);

        printHeader(cout);

        cout << "Customer Name: " << customerName << endl;
        cout << "Phone Number: " << phone << endl;

        cout << "\nID\tName\tQty\tPrice\tAmount\n";

        for (int i = 0; i < count; i++)
            items[i].display(cout);

        bill.display();

        cout << "====================================\n";
        cout << "       Thank You for Shopping!\n";
        cout << "====================================\n";

        invoiceCount++;
    }

    // File Handling
    void saveInvoice(float discountRate) {
        InvoiceTotal<float> bill = calculateInvoice(discountRate);

        ofstream file("invoice.txt");

        if (!file)
            throw 6;

        printHeader(file);

        file << "Customer Name: " << customerName << endl;
        file << "Phone Number: " << phone << endl;

        file << "\nID\tName\tQty\tPrice\tAmount\n";

        for (int i = 0; i < count; i++)
            items[i].display(file);

        file << "\nSubtotal: Rs. " << bill.subtotal;
        file << "\nDiscount: Rs. " << bill.discount;
        file << "\nGST: Rs. " << bill.tax;
        file << "\nFinal Amount: Rs. " << bill.total;

        file.close();

        invoiceCount++;
        cout << "Invoice saved in invoice.txt successfully!\n";
    }

    // Load Product Details
    void loadItems() {
        ifstream file("invoiceitems.txt");
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
        cout << "Products loaded successfully!\n";
    }

    // Static Member Function
    static void showInvoiceCount() {
        cout << "Invoices Generated: " << invoiceCount << endl;
    }

    // Friend Function can access customer details
    friend void showCustomer(const Invoice &x);
};

int Invoice::invoiceCount = 0;

// Friend Function
void showCustomer(const Invoice &x) {
    cout << "\nCustomer Name: " << x.customerName;
    cout << "\nPhone Number: " << x.phone << endl;
}

// Main Function
int main() {
    string name, phone;
    int choice, id, qty;
    string product;
    float price, discount;

    cout << "Enter Customer Name: ";
    cin >> name;

    cout << "Enter Phone Number: ";
    cin >> phone;

    Invoice invoice(name, phone);

    do {
        cout << "\n===== INVOICE MENU =====\n";
        cout << "1. Add Product\n";
        cout << "2. Display Products\n";
        cout << "3. Show Customer Details\n";
        cout << "4. Generate Invoice\n";
        cout << "5. Save Invoice\n";
        cout << "6. Load Products\n";
        cout << "7. Display Total Items\n";
        cout << "8. Display Invoice Count\n";
        cout << "0. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;

        try {
            switch (choice) {
            case 1:
                cout << "Enter ID, Product Name, Quantity, Price: ";
                cin >> id >> product >> qty >> price;
                invoice.addItem(id, product, qty, price);
                break;

            case 2:
                cout << "\nID\tName\tQty\tPrice\tAmount\n";
                invoice.generateInvoice(0);
                break;

            case 3:
                showCustomer(invoice);
                break;

            case 4:
                cout << "Enter Discount Percentage: ";
                cin >> discount;
                invoice.generateInvoice(discount);
                break;

            case 5:
                cout << "Enter Discount Percentage: ";
                cin >> discount;
                invoice.saveInvoice(discount);
                break;

            case 6:
                invoice.loadItems();
                break;

            case 7:
                InvoiceItem::showTotalItems();
                break;

            case 8:
                Invoice::showInvoiceCount();
                break;

            case 0:
                cout << "Thank You!\n";
                break;

            default:
                cout << "Invalid Choice!\n";
            }
        }
        catch (int e) {
            cout << "Error! Error Code: " << e << endl;
        }

    } while (choice != 0);

    return 0;
}
