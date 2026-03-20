/**
 * Project: Electronic Shop Management System
 * File: AdminPanel.h
 * Author: Oleksandr
 * Date: March 2026 (Week 2 - Header Implementation)
 */

#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include "DatabaseManager.hpp"
#include <vector>
#include <string>

class AdminPanel {
private:
    // Reference to the shared database manager (managed by Pasha)
    DatabaseManager& dbManager;

    // Internal helper methods for the admin UI logic
    void showInventory();
    void addProduct();
    void updateStock();
    void removeProduct();
    void showUsers();

public:
    /**
     * @brief Constructor for AdminPanel
     * @param db A reference to an existing DatabaseManager object
     */
    AdminPanel(DatabaseManager& db);

    /**
     * @brief Launches the interactive CLI menu for administrative tasks
     */
    void displayMenu();

    // Destructor
    ~AdminPanel() = default;
};

#endif // ADMINPANEL_H