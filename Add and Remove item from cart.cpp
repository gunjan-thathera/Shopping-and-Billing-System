
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// Structure
struct CartDetails {
    int id, quantity;
    string name, category;
    float price;
};

// Function Template
template <class T>
T calculateAmount(T price, int quantity) {
    return price * quantity;
}

// Class Template
template <class T>
class CartSummary {
public:
    T total;
    CartSummary() { total = 0; }
    void add(T amount) { total += amount; }
    void display() { cout << "Total Amount: " << total << endl; }
};

// Base Class
class CartItem {
protected:
    CartDetails item;
    const int itemType;

public:
    static int totalItems;

    CartItem() : itemType(1) {
        item = {0, 0, "", "", 0};
    }

    CartItem(int id, string name, string cat, float price, int qty)
        : itemType(1) {
        item = {id, qty, name, cat, price};
    }

    CartItem(const CartItem &x) : itemType(1) {
        item = x.item;
    }

    virtual ~CartItem() {}

    CartItem& operator=(const CartItem &x) {
        if (this != &x)
            item = x.item;
        return *this;
    }

    int getID() const { return item.id; }
    int getQuantity() const { return item.quantity; }
    float getPrice() const { return item.price; }
    string getName() const { return item.name; }

    void setQuantity(int q) { item.quantity = q; }

    virtual float getSubtotal() const {
        return calculateAmount(item.price, item.quantity);
    }

    // Operator Overloading
    CartItem operator+(const CartItem &x) {
        CartItem temp(*this);
        temp.item.quantity += x.item.quantity;
        return temp;
    }

    // Virtual Function
    virtual void display() const {
        cout << item.id << "\t" << item.name << "\t"
             << item.price << "\t" << item.quantity
             << "\t" << getSubtotal() << endl;
    }

    // Friend Function
    friend void showCartItem(const CartItem &x);

    static void showTotalItems() {
        cout << "Total Item Units: " << totalItems << endl;
    }
};

int CartItem::totalItems = 0;

void showCartItem(const CartItem &x) {
    cout << "Item Name: " << x.item.name << endl;
    cout << "Category: " << x.item.category << endl;
}

// Inheritance
class DiscountedCartItem : public CartItem {
    float discount;

public:
    DiscountedCartItem(int id, string name, string cat,
                       float price, int qty, float d)
        : CartItem(id, name, cat, price, qty) {
        discount = d;
    }

    // Function Overriding
    float getPrice() const {
        return item.price - (item.price * discount / 100);
    }

    float getSubtotal() const override {
        return getPrice() * item.quantity;
    }

    void display() const override {
        cout << item.id << "\t" << item.name << "\t"
             << getPrice() << "\t" << item.quantity
             << "\t" << getSubtotal() << endl;
    }
};

// Shopping Cart Class
class ShoppingCart {
    CartItem *items;
    int count;
    const string cartName;
    int capacity;

public:
    ShoppingCart() : cartName("My Shopping Cart") {
        capacity = 50;
        count = 0;
        items = new CartItem[capacity]; // Dynamic allocation
    }

    ~ShoppingCart() {
        CartItem::totalItems -= CartItem::totalItems < 0
                                ? 0 : 0;
        for (int i = 0; i < count; i++)
            CartItem::totalItems -= items[i].getQuantity();
        delete[] items;
    }

    // Function Overloading
    void addItem(int id, string name, string cat,
                 float price, int qty) {
        CartItem x(id, name, cat, price, qty);
        addItem(x);
    }

    void addItem(const CartItem &x) {
        if (x.getID() <= 0 || x.getQuantity() <= 0 ||
            x.getPrice() < 0)
            throw 1;

        for (int i = 0; i < count; i++) {
            if (items[i].getID() == x.getID()) {
                if (items[i].getPrice() != x.getPrice())
                    throw 2;

                items[i] = items[i] + x;
                CartItem::totalItems += x.getQuantity();
                cout << "Item quantity updated!\n";
                return;
            }
        }

        if (count == capacity)
            throw 3;

        items[count++] = x;
        CartItem::totalItems += x.getQuantity();
        cout << "Item added successfully!\n";
    }

    int searchItem(int id) {
        for (int i = 0; i < count; i++)
            if (items[i].getID() == id)
                return i;
        return -1;
    }

    // Function Overloading
    void removeItem(int id) {
        removeItem(id, 0);
    }

    void removeItem(int id, int qty) {
        int i = searchItem(id);

        if (i == -1)
            throw 4;

        if (qty <= 0 || qty >= items[i].getQuantity()) {
            CartItem::totalItems -= items[i].getQuantity();

            for (int j = i; j < count - 1; j++)
                items[j] = items[j + 1];

            count--;
            cout << "Item removed!\n";
        } else {
            items[i].setQuantity(items[i].getQuantity() - qty);
            CartItem::totalItems -= qty;
            cout << "Quantity removed!\n";
        }
    }

    void displayCart() {
        CartSummary<float> summary;

        cout << "\nID\tName\tPrice\tQty\tAmount\n";

        for (int i = 0; i < count; i++) {
            items[i].display();
            summary.add(items[i].getSubtotal());
        }

        summary.display();
    }

    void searchAndDisplay(int id) {
        int i = searchItem(id);

        if (i == -1)
            throw 4;

        showCartItem(items[i]);
    }

    // File Handling
    void saveToFile() {
        ofstream file("cart.txt");

        if (!file)
            throw 5;

        for (int i = 0; i < count; i++) {
            file << items[i].getID() << "|"
                 << items[i].getName() << "|"
                 << items[i].getPrice() << "|"
                 << items[i].getQuantity() << "\n";
        }

        file.close();
        cout << "Cart saved successfully!\n";
    }

    void loadFromFile() {
        ifstream file("cart.txt");
        string line;

        if (!file)
            throw 5;

        while (getline(file, line)) {
            stringstream ss(line);
            string name;
            int id, qty;
            float price;

            ss >> id;
            ss.ignore(1);
            getline(ss, name, '|');
            ss >> price;
            ss.ignore(1);
            ss >> qty;

            if (ss && id > 0 && qty > 0 && price >= 0)
                addItem(id, name, "General", price, qty);
        }

        file.close();
        cout << "Cart loaded successfully!\n";
    }
};

// Main Function
int main() {
    ShoppingCart cart;
    int choice, id, qty;
    string name, category;
    float price, discount;

    do {
        cout << "\n===== SHOPPING CART =====\n";
        cout << "1. Add Item\n";
        cout << "2. Add Discounted Item\n";
        cout << "3. Display Cart\n";
        cout << "4. Search Item\n";
        cout << "5. Remove Complete Item\n";
        cout << "6. Remove Item Quantity\n";
        cout << "7. Display Total Items\n";
        cout << "8. Save Cart\n";
        cout << "9. Load Cart\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        try {
            switch (choice) {
            case 1:
            case 2:
                cout << "Enter ID, Name, Category, Price, Quantity: ";
                cin >> id >> name >> category >> price >> qty;

                if (choice == 1) {
                    cart.addItem(id, name, category, price, qty);
                } else {
                    cout << "Enter Discount (%): ";
                    cin >> discount;

                    if (discount < 0 || discount > 100)
                        throw 1;

                    DiscountedCartItem d(id, name, category,
                                         price, qty, discount);

                    CartItem *ptr = &d;
                    ptr->display(); // Virtual function

                    CartItem x(id, name, category,
                               d.getPrice(), qty);
                    cart.addItem(x);
                }
                break;

            case 3:
                cart.displayCart();
                break;

            case 4:
                cout << "Enter Item ID: ";
                cin >> id;
                cart.searchAndDisplay(id);
                break;

            case 5:
                cout << "Enter Item ID: ";
                cin >> id;
                cart.removeItem(id);
                break;

            case 6:
                cout << "Enter Item ID and Quantity: ";
                cin >> id >> qty;
                cart.removeItem(id, qty);
                break;

            case 7:
                CartItem::showTotalItems();
                break;

            case 8:
                cart.saveToFile();
                break;

            case 9:
                cart.loadFromFile();
                break;

            case 0:
                cout << "Thank you!\n";
                break;

            default:
                cout << "Invalid choice!\n";
            }
        }
        catch (int e) {
            cout << "Error occurred! Error Code: " << e << endl;
        }

    } while (choice != 0);

    return 0;
}
