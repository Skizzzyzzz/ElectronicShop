#include "../include/Interface.hpp"
#include "../include/DatabaseManager.hpp"
#include "../include/User.hpp"
#include "../include/Product.hpp"
#include "../include/Cart.hpp"
#include "../include/AdminPanel.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace std;
Interface::Interface() {
    dbManager = new DatabaseManager("data/users.csv", "data/products.csv", "data/orders.csv");
    currentUser = NULL; 
    cart = new Cart();
}

Interface::~Interface() {
    delete dbManager;
    delete cart;
    if (currentUser != NULL) {
        delete currentUser;
    }
}
//Main loop
void Interface::start() {
    cout << "Welcome to the Shop!" << endl;
    bool keepRunning = true;

    while (keepRunning == true) {
        if (currentUser == NULL) {
            keepRunning = showAuthMenu();
        }
        else {
            if (currentUser->getRole() == "Admin") {
                keepRunning = showAdminMenu();
            }
            else {
                keepRunning = showMainMenu();
            }
        }
    }
    cout << "Goodbye! Thanks for shopping." << endl;
}
//Menu for login and register
bool Interface::showAuthMenu() {
    cout << endl << "1. Login" << endl;
    cout << "2. Register" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    string u, p;

    if (choice == 1) {
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;

        currentUser = dbManager->authenticateUser(u, p);

        if (currentUser != NULL) {
            cout << "Login success! Welcome " << currentUser->getUsername() << endl;
        }
        else {
            cout << "Wrong username or password. Try again." << endl;
        }
        return true;
    }
    else if (choice == 2) {
        cout << "Pick a Username: ";
        cin >> u;
        cout << "Pick a Password: ";
        cin >> p;

        bool ok = dbManager->registerUser(u, p, "Customer");
        if (ok) {
            cout << "Registered! You can login now." << endl;
        }
        else {
            cout << "Error. Username taken." << endl;
        }
        return true;
    }
    else if (choice == 3) {
        return false;
    }
    else {
        cout << "Bad input!" << endl;
        return true;
    }
}
//Menu for admin
bool Interface::showMainMenu() {
    cout << endl << "--- Main Menu ---" << endl;
    cout << "1. View Products" << endl;
    cout << "2. View Cart" << endl;
    cout << "3. Checkout" << endl;
    cout << "4. Logout" << endl;
    cout << "5. Exit App" << endl;
    cout << "Choice: ";

    int c;
    cin >> c;

    switch (c) {
    case 1:
        showProductCatalog();
        return true;
    case 2:
        showCart();
        return true;
    case 3:
        processCheckout();
        return true;
    case 4:
        delete currentUser;
        currentUser = NULL;
        cart->clearCart();
        cout << "Logged out." << endl;
        return true;
    case 5:
        return false;
    default:
        cout << "Invalid choice." << endl;
        return true;
    }
}
//Print all products
void Interface::showProductCatalog() {
    vector<Product> prodList = dbManager->getAllProducts();

    cout << endl << "--- Products ---" << endl;
 //for loop to print all products
    for (int i = 0; i < prodList.size(); i++) {
        cout << prodList[i].getId() << ". "
            << prodList[i].getName() << " - $"
            << prodList[i].getPrice() << " ("
            << prodList[i].getStock() << " left)" << endl;
    }

    cout << endl << "Enter Product ID to add to cart (or 0 to exit): ";
    int id;
    cin >> id;
    if (id != 0) {
        Product* p = dbManager->getProductById(id);
        if (p != NULL && p->getStock() > 0) {
            cart->addProduct(*p);
            cout << "Added to cart!" << endl;
        }
        else {
            cout << "Not found or out of stock!" << endl;
        }
    }
}
//in the cart
void Interface::showCart() {
    cout << endl << "--- Cart ---" << endl;
    cart->displayContents();
    cout << "Total Price: $" << cart->calculateTotal() << endl;
}
//Buy
void Interface::processCheckout() {
    if (cart->isEmpty()) {
        cout << "Cart is empty." << endl;
        return;
    }
    cout << "Total is $" << cart->calculateTotal() << endl;
    cout << "Buy now? (y/n): ";
    char ans;
    cin >> ans;

    if (ans == 'y' || ans == 'Y') {
        dbManager->saveOrder(currentUser->getId(), cart->getItems(), cart->calculateTotal());
        dbManager->updateStock(cart->getItems());
        cart->clearCart();
        cout << "Checkout complete! Thanks!" << endl;
    }
    else {
        cout << "Checkout canceled." << endl;
    }
}
//admin panel
bool Interface::showAdminMenu() {
    AdminPanel admin(dbManager);
    return admin.showMenu(currentUser);
}