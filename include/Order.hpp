#ifndef ORDER_H
#define ORDER_H

#include <vector>
#include <string>
#include "Product.h"

// helper struct to store product + quantity
struct OrderItem
{
    Product product;
    int quantity;
};

class Order
{
private:
    int id;
    std::string clientUsername;

    std::vector<OrderItem> items;

    double totalAmount;

public:
    Order();
    Order(int id, std::string username);

    void addProduct(Product product, int quantity); // change of quantity

    // price * quantity
    void calculateTotal();

    // Added 'const' to getters (Best Practice)
    int getID() const;
    std::string getClient() const;
    double getTotal() const;                    // New getter for the price
    std::vector<OrderItem> getProducts() const; // Changed return type
};

#endif
