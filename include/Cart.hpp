#pragma once

#include <vector>
#include <string>
#include "Product.hpp"

struct CartItem {
    Product product;
    int quantity;
};

class Cart {
public:
    Cart();
    Cart(int userId);

    void addItem(const Product& product, int quantity);
    void removeItem(int productId);
    void updateQuantity(int productId, int newQuantity);
    void clear();

    int getItemCount() const;
    int getTotalQuantity() const;
    double getTotalPrice() const;
    int getUserId() const;
    std::vector<CartItem> getItems() const;

    void displayCart() const;
    bool containsProduct(int productId) const;

    std::vector<std::string> toOrderCSV(int orderId) const;

private:
    int userId;
    std::vector<CartItem> items;

    CartItem* findItem(int productId);
};
