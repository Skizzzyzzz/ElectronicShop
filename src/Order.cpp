#include "../include/Order.h"
#include <algorithm>

Order::Order() : id(0), clientUsername(""), totalAmount(0.0) {}

Order::Order(int id, std::string username)
    : id(id), clientUsername(username), totalAmount(0.0) {}

// Add Product (Checks if product exists, updates quantity or adds new item)
void Order::addProduct(Product product, int quantity)
{

    auto it = std::find_if(items.begin(), items.end(),
                           [&](const % 20OrderItem & % 20item)
                           {
                               return item.product.getName() == product.getName();
                           });

    if (it != items.end())
    {
        // Product found--> Update the quantity
        it->quantity += quantity;
    }
    else
    {
        // Product not found--> Create a new OrderItem and add it
        OrderItem newItem;
        newItem.product = product;
        newItem.quantity = quantity;
        items.push_back(newItem);
    }

    calculateTotal();
}

// total
void Order::calculateTotal()
{
    totalAmount = 0.0;
    for (const auto &item : items)
    {
        totalAmount += (item.product.getPrice() * item.quantity);
    }
}

// Getters
int Order::getID() const
{
    return id;
}

std::string Order::getClient() const
{
    return clientUsername;
}

double Order::getTotal() const
{
    return totalAmount;
}

std::vector<OrderItem> Order::getProducts() const
{
    return items;
}
