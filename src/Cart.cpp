#include "../include/Cart.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

Cart::Cart() : userId(0) {}

Cart::Cart(int userId) : userId(userId) {}

void Cart::addItem(const Product& product, int quantity) {
    if (quantity <= 0) {
        std::cout << "[Cart] Error: Quantity must be greater than 0." << std::endl;
        return;
    }

    if (quantity > product.getCount()) {
        std::cout << "[Cart] Error: Not enough stock for \""
                  << product.getName() << "\". Available: "
                  << product.getCount() << std::endl;
        return;
    }

    CartItem* existing = findItem(product.getId());
    if (existing) {
        int newQty = existing->quantity + quantity;
        if (newQty > product.getCount()) {
            std::cout << "[Cart] Error: Adding " << quantity
                      << " would exceed stock (" << product.getCount()
                      << ") for \"" << product.getName() << "\"." << std::endl;
            return;
        }
        existing->quantity = newQty;
        std::cout << "[Cart] Updated \"" << product.getName()
                  << "\" quantity to " << newQty << "." << std::endl;
    } else {
        CartItem item;
        item.product = product;
        item.quantity = quantity;
        items.push_back(item);
        std::cout << "[Cart] Added \"" << product.getName()
                  << "\" x" << quantity << " to cart." << std::endl;
    }
}

void Cart::removeItem(int productId) {
    auto it = std::remove_if(items.begin(), items.end(),
        [productId](const CartItem& item) {
            return item.product.getId() == productId;
        });

    if (it != items.end()) {
        std::cout << "[Cart] Removed \"" << it->product.getName()
                  << "\" from cart." << std::endl;
        items.erase(it, items.end());
    } else {
        std::cout << "[Cart] Error: Product with ID "
                  << productId << " not found in cart." << std::endl;
    }
}

void Cart::updateQuantity(int productId, int newQuantity) {
    CartItem* item = findItem(productId);
    if (!item) {
        std::cout << "[Cart] Error: Product with ID "
                  << productId << " not found in cart." << std::endl;
        return;
    }

    if (newQuantity <= 0) {
        removeItem(productId);
        return;
    }

    if (newQuantity > item->product.getCount()) {
        std::cout << "[Cart] Error: Requested quantity (" << newQuantity
                  << ") exceeds available stock ("
                  << item->product.getCount() << ") for \""
                  << item->product.getName() << "\"." << std::endl;
        return;
    }

    item->quantity = newQuantity;
    std::cout << "[Cart] Updated \"" << item->product.getName()
              << "\" quantity to " << newQuantity << "." << std::endl;
}

void Cart::clear() {
    items.clear();
    std::cout << "[Cart] Cart cleared." << std::endl;
}

int Cart::getItemCount() const {
    return static_cast<int>(items.size());
}

int Cart::getTotalQuantity() const {
    int total = 0;
    for (int i = 0; i < items.size(); i++) {
        total += items[i].quantity;
    }
    return total;
}

double Cart::getTotalPrice() const {
    double total = 0.0;
    for (int i = 0; i < items.size(); i++) {
        total += items[i].product.getPrice() * items[i].quantity;
    }
    return total;
}

int Cart::getUserId() const {
    return userId;
}

std::vector<CartItem> Cart::getItems() const {
    return items;
}

void Cart::displayCart() const {
    if (items.empty()) {
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "  Your cart is empty." << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "                     YOUR SHOPPING CART" << std::endl;
    std::cout << "============================================================" << std::endl;

    std::cout << std::left
              << std::setw(6)  << "ID"
              << std::setw(25) << "Product"
              << std::setw(12) << "Price"
              << std::setw(10) << "Qty"
              << std::setw(12) << "Subtotal"
              << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    for (int i = 0; i < items.size(); i++) {
        double subtotal = items[i].product.getPrice() * items[i].quantity;
        std::cout << std::left
                  << std::setw(6)  << items[i].product.getId()
                  << std::setw(25) << items[i].product.getName()
                  << "$" << std::setw(11) << std::fixed << std::setprecision(2)
                  << items[i].product.getPrice()
                  << std::setw(10) << items[i].quantity
                  << "$" << std::fixed << std::setprecision(2) << subtotal
                  << std::endl;
    }

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "  Items: " << getItemCount()
              << "  |  Total Qty: " << getTotalQuantity()
              << "  |  TOTAL: $" << std::fixed << std::setprecision(2)
              << getTotalPrice() << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;
}

std::string Cart::getCartAsString() const {
    std::stringstream ss;
    if (items.empty()) {
        ss << std::endl;
        ss << "========================================" << std::endl;
        ss << "  Your cart is empty." << std::endl;
        ss << "========================================" << std::endl;
        ss << std::endl;
        return ss.str();
    }

    ss << std::endl;
    ss << "============================================================" << std::endl;
    ss << "                     YOUR SHOPPING CART" << std::endl;
    ss << "============================================================" << std::endl;

    ss << std::left
       << std::setw(6)  << "ID"
       << std::setw(25) << "Product"
       << std::setw(12) << "Price"
       << std::setw(10) << "Qty"
       << std::setw(12) << "Subtotal"
       << std::endl;
    ss << "------------------------------------------------------------" << std::endl;

    for (int i = 0; i < items.size(); i++) {
        double subtotal = items[i].product.getPrice() * items[i].quantity;
        ss << std::left
           << std::setw(6)  << items[i].product.getId()
           << std::setw(25) << items[i].product.getName()
           << "$" << std::setw(11) << std::fixed << std::setprecision(2)
           << items[i].product.getPrice()
           << std::setw(10) << items[i].quantity
           << "$" << std::fixed << std::setprecision(2) << subtotal
           << std::endl;
    }

    ss << "------------------------------------------------------------" << std::endl;
    ss << "  Items: " << getItemCount()
       << "  |  Total Qty: " << getTotalQuantity()
       << "  |  TOTAL: $" << std::fixed << std::setprecision(2)
       << getTotalPrice() << std::endl;
    ss << "============================================================" << std::endl;
    ss << std::endl;
    return ss.str();
}

bool Cart::isEmpty() const {
    return items.empty();
}

void Cart::clearCart() {
    items.clear();
}

void Cart::addProduct(const Product& p) {
    addItem(p, 1);
}

bool Cart::containsProduct(int productId) const {
    for (int i = 0; i < items.size(); i++) {
        if (items[i].product.getId() == productId) {
            return true;
        }
    }
    return false;
}

CartItem* Cart::findItem(int productId) {
    for (int i = 0; i < items.size(); i++) {
        if (items[i].product.getId() == productId) {
            return &items[i];
        }
    }
    return nullptr;
}

std::vector<std::string> Cart::toOrderCSV(int orderId) const {
    std::vector<std::string> lines;

    for (int i = 0; i < items.size(); i++) {
        std::ostringstream oss;
        oss << orderId << ","
            << userId << ","
            << items[i].product.getId() << ","
            << items[i].product.getName() << ","
            << items[i].quantity << ","
            << std::fixed << std::setprecision(2)
            << (items[i].product.getPrice() * items[i].quantity);
        lines.push_back(oss.str());
    }

    return lines;
}
