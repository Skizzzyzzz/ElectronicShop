#ifndef INTERFACE_H
#define INTERFACE_H

// Include the classes we need pointers for
#include "DatabaseManager.h"
#include "User.h"
#include "Cart.h"

class Interface {
private:
    // Pointers to our main objects
    DatabaseManager* dbManager;
    User* currentUser;
    Cart* cart;

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
};

#endif // INTERFACE_H
