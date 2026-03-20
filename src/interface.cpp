#include "../include/Interface.hpp"
#include "../include/DatabaseManager.hpp"
#include "../include/Cart.hpp"
#include "../include/AdminPanel.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
using namespace std;
Interface::Interface() {
    dbManager = new DatabaseManager("data/users.csv", "data/products.csv", "data/orders.csv");
    currentUser = NULL; 
}

Interface::~Interface() {
    delete dbManager;
    for (auto& pair : userCarts) {
        delete pair.second;
    }
    if (currentUser != NULL) {
        delete currentUser;
    }
}

Cart* Interface::getCartForCurrentUser() {
    if (currentUser == nullptr) return nullptr;
    int id = currentUser->getId();
    auto it = userCarts.find(id);
    if (it == userCarts.end() || it->second == nullptr) {
        userCarts[id] = new Cart(id);
        return userCarts[id];
    }
    return it->second;
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
            getCartForCurrentUser();
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

        bool ok = dbManager->registerUser(u, "", p, "Customer");
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
        if (currentUser) {
            Cart* cart = getCartForCurrentUser();
            if (cart) cart->clearCart();
        }
        delete currentUser;
        currentUser = NULL;
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
            << prodList[i].getCount() << " left)" << endl;
    }

    cout << endl << "Enter Product ID to add to cart (or 0 to exit): ";
    int id;
    cin >> id;
    if (id != 0) {
        Product* p = dbManager->getProductById(id);
        if (p != NULL && p->getCount() > 0) {
            Cart* cart = getCartForCurrentUser();
            if (cart) cart->addProduct(*p);
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
    if (currentUser) {
        Cart* cart = getCartForCurrentUser();
        if (cart) {
            cart->displayCart();
            cout << "Total Price: $" << cart->getTotalPrice() << endl;
        }
    }
}
//Buy
void Interface::processCheckout() {
    Cart* cart = getCartForCurrentUser();
    if (currentUser == nullptr || cart == nullptr || cart->isEmpty()) {
        cout << "Cart is empty." << endl;
        return;
    }
    cout << "Total is $" << cart->getTotalPrice() << endl;
    cout << "Buy now? (y/n): ";
    char ans;
    cin >> ans;

    if (ans == 'y' || ans == 'Y') {
        dbManager->saveOrder(currentUser->getId(), cart->getItems(), cart->getTotalPrice());
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
    // AdminPanel admin(*dbManager);
    // admin.displayMenu();
    cout << "Admin menu not implemented." << endl;
    return true;
}

// API methods
User* Interface::apiLogin(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentUser != nullptr) {
        delete currentUser;
    }
    currentUser = dbManager->authenticateUser(username, password);
    if (currentUser) {
        if (userCarts.find(currentUser->getId()) == userCarts.end()) {
            userCarts[currentUser->getId()] = new Cart(currentUser->getId());
        }
    }
    return currentUser;
}

bool Interface::apiSetCurrentUser(int userId) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (userId <= 0) return false;
    User* found = dbManager->getUserById(userId);
    if (!found) return false;
    if (currentUser != nullptr) {
        delete currentUser;
    }
    currentUser = new User(found->getUsername(), found->getPhone(), found->getPassword(), found->getRole(), found->getId(), found->getEmail(), found->getAddress());
    getCartForCurrentUser();
    return true;
}

bool Interface::apiRegister(const std::string& username, const std::string& password, const std::string& email, const std::string& address) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return dbManager->registerUser(username, "", password, "Customer", email, address);
}

bool Interface::apiAddUser(const std::string& username, const std::string& phone, const std::string& password, const std::string& email, const std::string& address, const std::string& role) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return dbManager->registerUser(username, phone, password, role, email, address);
}

bool Interface::apiAddProduct(const std::string& name, double price, int stock, const std::string& category, const std::string& description, const std::string& photo) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    dbManager->addProduct(name, price, stock, category, description, photo);
    return true;
}

bool Interface::apiUpdateUser(int user_id, const std::string& name, const std::string& email, const std::string& phone, const std::string& address, const std::string& password) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return dbManager->updateUser(user_id, name, email, phone, address, password);
}

User* Interface::apiGetUserById(int id) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return dbManager->getUserById(id);
}

std::vector<Order> Interface::apiGetOrders() {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return dbManager->getAllOrders();
}

std::vector<Product> Interface::apiGetProducts() {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return dbManager->getAllProducts();
}

bool Interface::apiAddToCart(int productId) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentUser == nullptr) return false;
    Cart* cart = getCartForCurrentUser();
    if (cart == nullptr) return false;
    Product* p = dbManager->getProductById(productId);
    if (p != nullptr && p->getCount() > 0) {
        cart->addProduct(*p);
        return true;
    }
    return false;
}

std::vector<CartItem> Interface::apiGetCartItems() {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentUser == nullptr) return {};
    Cart* cart = getCartForCurrentUser();
    return cart ? cart->getItems() : std::vector<CartItem>{};
}

std::string Interface::apiGetCartContents() {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentUser == nullptr) return "";
    Cart* cart = getCartForCurrentUser();
    return cart ? cart->getCartAsString() : "";
}

double Interface::apiGetCartTotal() {
    std::lock_guard<std::mutex> lock(sessionMutex);
    Cart* cart = getCartForCurrentUser();
    if (currentUser == nullptr || cart == nullptr) return 0.0;
    return cart->getTotalPrice();
}

bool Interface::apiCheckout() {
    std::lock_guard<std::mutex> lock(sessionMutex);
    Cart* cart = getCartForCurrentUser();
    if (currentUser == nullptr || cart == nullptr || cart->isEmpty()) return false;
    dbManager->saveOrder(currentUser->getId(), cart->getItems(), cart->getTotalPrice());
    dbManager->updateStock(cart->getItems());
    cart->clearCart();
    return true;
}

bool Interface::apiRemoveFromCart(int productId) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentUser == nullptr) return false;
    Cart* cart = getCartForCurrentUser();
    if (cart) cart->removeItem(productId);
    return true;
}

bool Interface::apiUpdateCartQuantity(int productId, int quantity) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentUser == nullptr) return false;
    Cart* cart = getCartForCurrentUser();
    if (cart) cart->updateQuantity(productId, quantity);
    return true;
}

void Interface::apiLogout() {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentUser != nullptr) {
        Cart* cart = getCartForCurrentUser();
        if (cart) cart->clearCart();
        delete currentUser;
        currentUser = nullptr;
    }
}