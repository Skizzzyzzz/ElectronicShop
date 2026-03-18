#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Product {
    int id;
    string name;
    double price;

public:
    Product(int id, const string &name, double price)
        : id(id), name(name), price(price) {}

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
};

class ProductManager {
    vector<Product> products;

public:
    void addProduct(Product product) {
        products.push_back(product);
    }

    void printProducts() {
        for (const auto &product : products) {
            cout << "ID: " << product.getId() << endl;
            cout << "Name: " << product.getName() << endl;
            cout << "Price: " << product.getPrice() << endl;
            cout << endl;
        }
    }

    void removeProduct(int id) {
        for (auto it = products.begin(); it != products.end(); ++it) {
            if (it->getId() == id) {
                products.erase(it);
                return;
            }
        }
        cout << "Product not found.\n";
    }

    void showallProducts() {
        cout << "All Products:\n";
        printProducts();
    }

    void makeOrder() {
        if (products.empty()) {
            cout << "No products available to order.\n";
            return;
        }
        cout << "Order placed for the following products:\n";
        printProducts();
    }

    void checkOrder() {
        cout << "Checking order...\n";
        printProducts();
    }

    void confirmOrderReceipt() {
        cout << "Order receipt confirmed.\n";
    }
    
};

int main() {
    ProductManager manager;
    manager.addProduct(Product(1, "Laptop", 999.99));
    manager.addProduct(Product(2, "Smartphone", 499.99));
    manager.addProduct(Product(3, "Headphones", 199.99));

    while (true) {
        cout << "===== User Panel =====\n";
        cout << "1. Show Product List\n";
        cout << "2. Add Product to Cart\n";
        cout << "3. Remove Product from Cart\n";
        cout << "4. Make Order\n";
        cout << "5. Check Order\n";
        cout << "6. Confirm Order Receipt\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            manager.showallProducts();
        }
        else if (choice == 2) {
            int id;
            cout << "Enter product ID to add: ";
            cin >> id;
            manager.addProduct(Product(id, "New Product", 10.0));
        }
        else if (choice == 3) {
            int id;
            cout << "Enter product ID to remove: ";
            cin >> id;
            manager.removeProduct(id);
        }
        else if (choice == 4) {
            manager.makeOrder();
        }
        else if (choice == 5) {
            manager.checkOrder();
        }
        else if (choice == 6) {
            manager.confirmOrderReceipt();
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
