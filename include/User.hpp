is it a hpp file #ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Product.h" // Pulls in the Product class from your include folder

class User {
private:
    int userId;
    std::string username;
    
    // This replaces the "products" vector from your original ProductManager.
    // It acts as this specific user's cart.
    std::vector<Product> cartProducts; 

public:
    // Constructor
    User(int id = 0, std::string name = "Guest");

    // Getters for User info
    int getUserId() const;
    std::string getUsername() const;

    // --- Methods adapted directly from your original ProductManager ---
    
    void addProduct(Product product);
    void printProducts() const;
    void removeProduct(int id);
    void showallProducts() const;
    void makeOrder();
    void checkOrder() const;
    void confirmOrderReceipt() const;
};

#endif // USER_HPP