#ifndef INTERFACE_H
#define INTERFACE_H

// Include the classes we need pointers for
#include "DatabaseManager.hpp"
#include "Cart.hpp"
#include <map>
#include <mutex>

class Interface {
private:
    // Pointers to our main objects
    DatabaseManager* dbManager;
    User* currentUser;
    std::map<int, Cart*> userCarts;
    std::mutex sessionMutex; // guards currentUser and userCarts for web API calls

    Cart* getCartForCurrentUser();

    // Private functions so the user can't call them from main.cpp directly
    bool showAuthMenu();
    bool showMainMenu();
    void showProductCatalog();
    void showCart();
    void processCheckout();
    bool showAdminMenu();

public:
    // Constructor and Destructor
    Interface();
    ~Interface();

    // The only public function needed to run the app
    void start();

    bool isLoggedIn() const { return currentUser != nullptr; }

    // API methods
    User* apiLogin(const std::string& username, const std::string& password);
    bool apiSetCurrentUser(int userId);
    bool apiRegister(const std::string& username, const std::string& password, const std::string& email = "", const std::string& address = "");
    bool apiAddUser(const std::string& username, const std::string& phone, const std::string& password, const std::string& email, const std::string& address, const std::string& role);
    bool apiAddProduct(const std::string& name, double price, int stock, const std::string& category = "", const std::string& description = "", const std::string& photo = "");
    bool apiUpdateUser(int user_id, const std::string& name, const std::string& email, const std::string& phone, const std::string& address, const std::string& password);
    User* apiGetUserById(int id);
    std::vector<Order> apiGetOrders();
    std::vector<Product> apiGetProducts();
    bool apiAddToCart(int productId);
    std::vector<CartItem> apiGetCartItems();
    std::string apiGetCartContents();
    double apiGetCartTotal();
    bool apiCheckout();
    bool apiRemoveFromCart(int productId);
    bool apiUpdateCartQuantity(int productId, int quantity);
    void apiLogout();
};

#endif // INTERFACE_H
