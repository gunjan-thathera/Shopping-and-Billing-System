
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// Structure to store product details
struct ProductDetails
{
    int id;
    string name;
    string category;
    float price;
    int quantity;
};

// Base class
class Product
{
private:
    ProductDetails details;

public:
    const int productType = 1;
    static int totalProducts;

    // Default constructor
    Product()
    {
        details = {0, "", "", 0.0f, 0};
    }

    // Parameterized constructor
    Product(int id, string name, string category,
            float price, int quantity)
    {
        details = {id, name, category, price, quantity};
    }

    // Destructor
    virtual ~Product() {}

    // Custom assignment operator
    Product& operator=(const Product &p)
    {
        if (this != &p)
            details = p.details;

        return *this;
    }

    // Static member function
    static void showTotalProducts()
    {
        cout << "\nTotal Products: "
             << totalProducts << endl;
    }

    // Getter functions
    int getId() const
    {
        return details.id;
    }

    string getName() const
    {
        return details.name;
    }

    string getCategory() const
    {
        return details.category;
    }

    float getPrice() const
    {
        return details.price;
    }

    int getQuantity() const
    {
        return details.quantity;
    }

    // Set product details
    void setDetails(int id, string name, string category,
                    float price, int quantity)
    {
        details = {id, name, category, price, quantity};
    }

    // Virtual function
    virtual void display() const
    {
        cout << "ID: " << details.id
             << " | Name: " << details.name
             << " | Category: " << details.category
             << " | Price: " << details.price
             << " | Quantity: " << details.quantity << endl;
    }

    // Function overloading
    void updatePrice(float price)
    {
        if (price < 0)
            throw 3;

        details.price = price;
    }

    void updatePrice(float price, int quantity)
    {
        if (price < 0 || quantity < 0)
            throw 3;

        details.price = price;
        details.quantity = quantity;
    }

    // Operator overloading
    bool operator<(const Product &p) const
    {
        return details.price < p.details.price;
    }

    // Friend function
    friend void showProduct(const Product &p);
};

// Initialize static member
int Product::totalProducts = 0;

// Friend function
void showProduct(const Product &p)
{
    cout << "\nProduct Details:";
    cout << "\nID: " << p.details.id;
    cout << "\nName: " << p.details.name;
    cout << "\nCategory: " << p.details.category;
    cout << "\nPrice: " << p.details.price;
    cout << "\nQuantity: " << p.details.quantity << endl;
}

// Inheritance
class DiscountedProduct : public Product
{
private:
    float discount;

public:
    DiscountedProduct(int id, string name, string category,
                      float price, int quantity, float discount)
        : Product(id, name, category, price, quantity)
    {
        if (discount < 0 || discount > 100)
            throw 3;

        this->discount = discount;
    }

    // Function overriding
    void display() const override
    {
        Product::display();

        cout << "Discount: " << discount << "%" << endl;

        cout << "Discounted Price: "
             << getPrice() - (getPrice() * discount / 100)
             << endl;
    }
};

// Function template
template <class T>
void showMessage(T message)
{
    cout << message << endl;
}

// Class template
template <class T>
class ProductBox
{
private:
    T value;

public:
    ProductBox(T v) : value(v) {}

    void displayBox()
    {
        cout << "Template Value: " << value << endl;
    }
};

// Product Catalogue class
class ProductCatalogue
{
private:
    int capacity;
    int count;
    Product *products;

    const string catalogueName;

public:
    // Constructor
    ProductCatalogue(int size = 50)
        : capacity(size), count(0), products(0),
          catalogueName("Shopping and Billing System")
    {
        if (capacity <= 0)
            throw 1;

        products = new Product[capacity];
    }

    // Destructor
    ~ProductCatalogue()
    {
        delete[] products;
    }

    // Prevent unsafe copying of dynamic array
    ProductCatalogue(const ProductCatalogue &) = delete;

    ProductCatalogue& operator=(const ProductCatalogue &) = delete;

    // Function overloading - Add product
    void addProduct(int id, string name, string category,
                    float price, int quantity)
    {
        if (count >= capacity)
            throw 2;

        if (id <= 0 || price < 0 || quantity < 0)
            throw 3;

        if (searchProduct(id) != -1)
            throw 4;

        products[count].setDetails(
            id, name, category, price, quantity);

        count++;
        Product::totalProducts++;

        cout << "\nProduct added successfully!\n";
    }

    // Second overloaded function
    void addProduct(const Product &p)
    {
        addProduct(p.getId(), p.getName(),
                   p.getCategory(), p.getPrice(),
                   p.getQuantity());
    }

    // Search product
    int searchProduct(int id) const
    {
        for (int i = 0; i < count; i++)
        {
            if (products[i].getId() == id)
                return i;
        }

        return -1;
    }

    // Display all products
    void displayProducts() const
    {
        if (count == 0)
        {
            cout << "\nNo products available!\n";
            return;
        }

        cout << "\n===== " << catalogueName << " =====\n";
        cout << "\n----- PRODUCT CATALOGUE -----\n";

        for (int i = 0; i < count; i++)
        {
            products[i].display();
        }
    }

    // Display single product
    void displayProduct(int index) const
    {
        if (index >= 0 && index < count)
        {
            showProduct(products[index]);
        }
    }

    // Update product
    void updateProduct(int id)
    {
        int index = searchProduct(id);

        if (index == -1)
            throw 5;

        float price;
        int quantity;

        cout << "Enter new price: ";

        if (!(cin >> price))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            throw 3;
        }

        cout << "Enter new quantity: ";

        if (!(cin >> quantity))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            throw 3;
        }

        products[index].updatePrice(price, quantity);

        cout << "\nProduct updated successfully!\n";
    }

    // Delete product
    void deleteProduct(int id)
    {
        int index = searchProduct(id);

        if (index == -1)
            throw 5;

        for (int i = index; i < count - 1; i++)
        {
            products[i] = products[i + 1];
        }

        count--;
        Product::totalProducts--;

        cout << "\nProduct deleted successfully!\n";
    }

    // Save products to file
    void saveToFile()
    {
        ofstream file("products.txt");

        if (!file)
            throw 6;

        for (int i = 0; i < count; i++)
        {
            file << products[i].getId() << "|"
                 << products[i].getName() << "|"
                 << products[i].getCategory() << "|"
                 << products[i].getPrice() << "|"
                 << products[i].getQuantity() << endl;
        }

        file.close();

        cout << "\nProducts saved successfully!\n";
    }

    // Load products from file
    void loadFromFile()
    {
        ifstream file("products.txt");

        if (!file)
            throw 7;

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string id, name, category, price, quantity;

            if (!getline(ss, id, '|') ||
                !getline(ss, name, '|') ||
                !getline(ss, category, '|') ||
                !getline(ss, price, '|') ||
                !getline(ss, quantity, '|'))
                continue;

            stringstream a(id), b(price), c(quantity);

            int pid, qty;
            float p;

            if (!(a >> pid) || !(b >> p) || !(c >> qty))
                continue;

            try
            {
                addProduct(pid, name, category, p, qty);
            }
            catch (int error)
            {
                cout << "Record skipped "
                     << "(duplicate, invalid, or catalogue full).\n";
            }
        }

        file.close();

        cout << "\nProducts loaded from file.\n";
    }
};

// Main function
int main()
{
    try
    {
        ProductCatalogue catalogue(50);

        int choice, id;

        do
        {
            cout << "\n================================";
            cout << "\n      MANAGE PRODUCT CATALOGUE";
            cout << "\n================================";
            cout << "\n1. Add Product";
            cout << "\n2. Display Products";
            cout << "\n3. Search Product";
            cout << "\n4. Update Product";
            cout << "\n5. Delete Product";
            cout << "\n6. Save Products to File";
            cout << "\n7. Load Products from File";
            cout << "\n8. Display Total Products";
            cout << "\n0. Exit";

            cout << "\nEnter your choice: ";

            if (!(cin >> choice))
            {
                cin.clear();
                cin.ignore(10000, '\n');

                cout << "\nPlease enter a valid number.\n";
                continue;
            }

            try
            {
                switch (choice)
                {
                case 1:
                {
                    string name, category;
                    float price;
                    int quantity;

                    cout << "Enter Product ID: ";

                    if (!(cin >> id))
                        throw 3;

                    cin.ignore(10000, '\n');

                    cout << "Enter Product Name: ";
                    getline(cin, name);

                    cout << "Enter Category: ";
                    getline(cin, category);

                    cout << "Enter Price: ";

                    if (!(cin >> price))
                        throw 3;

                    cout << "Enter Quantity: ";

                    if (!(cin >> quantity))
                        throw 3;

                    catalogue.addProduct(
                        id, name, category, price, quantity);

                    break;
                }

                case 2:
                    catalogue.displayProducts();
                    break;

                case 3:
                {
                    cout << "Enter Product ID: ";

                    if (!(cin >> id))
                        throw 3;

                    int index = catalogue.searchProduct(id);

                    if (index != -1)
                    {
                        cout << "\nProduct Found!\n";
                        catalogue.displayProduct(index);
                    }
                    else
                    {
                        cout << "\nProduct not found!\n";
                    }

                    break;
                }

                case 4:
                    cout << "Enter Product ID: ";

                    if (!(cin >> id))
                        throw 3;

                    catalogue.updateProduct(id);
                    break;

                case 5:
                    cout << "Enter Product ID: ";

                    if (!(cin >> id))
                        throw 3;

                    catalogue.deleteProduct(id);
                    break;

                case 6:
                    catalogue.saveToFile();
                    break;

                case 7:
                    catalogue.loadFromFile();
                    break;

                case 8:
                    Product::showTotalProducts();
                    break;

                case 0:
                    cout << "\nExiting Product Catalogue...\n";
                    break;

                default:
                    cout << "\nInvalid choice!\n";
                }
            }
            catch (int error)
            {
                if (error == 2)
                    cout << "\nCatalogue is full!\n";

                else if (error == 3)
                    cout << "\nInvalid product details or input!\n";

                else if (error == 4)
                    cout << "\nProduct ID already exists!\n";

                else if (error == 5)
                    cout << "\nProduct not found!\n";

                else if (error == 6)
                    cout << "\nUnable to save file!\n";

                else if (error == 7)
                    cout << "\nUnable to open file!\n";
            }

        } while (choice != 0);
    }
    catch (int error)
    {
        cout << "\nCatalogue initialization failed!\n";
    }

    return 0;
}

