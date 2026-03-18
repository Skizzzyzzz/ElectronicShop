#include "../include/AdminPanel.hpp"
#include <iostream>
#include <iomanip>

// Constructor
AdminPanel::AdminPanel(DatabaseManager& db) : dbManager(db) {}

void AdminPanel::displayMenu() {
    int choice;
    do {
        std::cout << "\n--- Admin Dashboard ---\n";
        std::cout << "1. View All Products\n";
        std::cout << "2. Add New Product\n";
        std::cout << "3. Remove Product\n";
        std::cout << "4. View Registered Users\n";
        std::cout << "5. View Order History\n";
        std::cout << "0. Logout\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: showInventory(); break;
            case 2: addProduct(); break;
            case 3: removeProduct(); break;
            case 4: showUsers(); break;
            case 0: std::cout << "Exiting Admin Panel...\n"; break;
            default: std::cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void AdminPanel::addProduct() {
    std::string name, category;
    double price;
    int stock;

    std::cout << "Enter Product Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Enter Category: ";
    std::getline(std::cin, category);
    std::cout << "Enter Price: ";
    std::cin >> price;
    std::cout << "Enter Stock Quantity: ";
    std::cin >> stock;

    // Logic to push to DatabaseManager
    Product newProd(name, category, price, stock);
    if(dbManager.saveProduct(newProd)) {
        std::cout << "Product added successfully!\n";
    } else {
        std::cerr << "Error: Could not save product to CSV.\n";
    }
}

void AdminPanel::showInventory() {
    auto products = dbManager.getAllProducts();
    std::cout << "\nID | Name | Category | Price | Stock\n";
    std::cout << "------------------------------------------\n";
    for (const auto& p : products) {
        std::cout << p.getId() << " | " << p.getName() << " | " 
                  << p.getCategory() << " | $" << p.getPrice() << " | " << p.getStock() << "\n";
    }
}

void AdminPanel::showUsers() {
    auto users = dbManager.getAllUsers();
    std::cout << "\nUsername | Email | Role\n";
    std::cout << "---------------------------\n";
    for (const auto& u : users) {
        std::cout << u.getUsername() << " | " << u.getEmail() << " | " << u.getRole() << "\n";
    }
}