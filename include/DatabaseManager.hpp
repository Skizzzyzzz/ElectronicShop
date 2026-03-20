#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>

using namespace std;

class User
{
private:
    int id;
    string username;
    string phone;
    string password;
    string role;
    string email;
    string address;

public:
    User() {}

    User(string username, string phone, string password, string role = "Customer", int id = 0, string email = "", string address = "")
    {
        this->id = id;
        this->username = username;
        this->phone = phone;
        this->password = password;
        this->role = role;
        this->email = email;
        this->address = address;
    }

    int getId() const { return id; }
    string getUsername() const { return username; }
    string getPhone() const { return phone; }
    string getPassword() const { return password; }
    string getRole() const { return role; }
    string getEmail() const { return email; }
    string getAddress() const { return address; }

    void setUsername(const string& u) { username = u; }
    void setPhone(const string& p) { phone = p; }
    void setPassword(const string& p) { password = p; }
    void setEmail(const string& e) { email = e; }
    void setAddress(const string& a) { address = a; }
};

class Product
{
private:
    int id;
    string name;
    double price;
    int count;
    double discount;
    string category;
    string description;
    string photo;

public:
    Product() {}

    Product(int id, string name, double price, int count, double discount, string category = "", string description = "", string photo = "")
    {
        this->id = id;
        this->name = name;
        this->price = price;
        this->count = count;
        this->discount = discount;
        this->category = category;
        this->description = description;
        this->photo = photo;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getCount() const { return count; }
    double getDiscount() const { return discount; }
    string getCategory() const { return category; }
    string getDescription() const { return description; }
    string getPhoto() const { return photo; }
};

struct CartItem {
    Product product;
    int quantity;
};

struct Order
{
    int id;
    string clientUsername;
    vector<pair<int, int>> productList;

    Order() {}

    Order(int id, string clientUsername, vector<pair<int, int>> productList)
    {
        this->id = id;
        this->clientUsername = clientUsername;
        this->productList = productList;
    }
};

class DatabaseManager
{
private:

    string usersFile = "users.csv";
    string productsFile = "products.csv";
    string ordersFile = "orders.csv";

    std::mutex dbMutex;

    vector<User> users;
    vector<Product> products;
    vector<Order> orders;

    vector<string> splitCSV(string line)
    {
        vector<string> result;
        stringstream ss(line);
        string item;

        while (getline(ss, item, ','))
            result.push_back(item);

        return result;
    }

    string serializeProducts(vector<pair<int, int>> list)
    {
        stringstream ss;

        for (int i = 0; i < list.size(); i++)
        {
            ss << list[i].first << ":" << list[i].second;

            if (i != list.size() - 1)
                ss << "|";
        }

        return ss.str();
    }

    vector<pair<int, int>> deserializeProducts(string data)
    {
        vector<pair<int, int>> result;

        stringstream ss(data);
        string token;

        while (getline(ss, token, '|'))
        {
            int pos = token.find(':');

            int id = stoi(token.substr(0, pos));
            int qty = stoi(token.substr(pos + 1));

            result.push_back({ id,qty });
        }

        return result;
    }

public:

    bool loadOrders()
    {
        orders.clear();

        ifstream file(ordersFile);

        if (!file)
            return false;

        string line;
        getline(file, line);

        while (getline(file, line))
        {
            auto f = splitCSV(line);

            orders.push_back(
                Order(
                    stoi(f[0]),
                    f[1],
                    deserializeProducts(f[2])
                )
            );
        }

        return true;
    }

    bool saveUsers()
    {
        ofstream file(usersFile);
        if (!file)
            return false;

        file << "username,phone,password,role,id,email,address\n";
        for (auto& u : users)
        {
            file << u.getUsername() << ","
                << u.getPhone() << ","
                << u.getPassword() << ","
                << u.getRole() << ","
                << u.getId() << ","
                << u.getEmail() << ","
                << u.getAddress() << "\n";
        }

        return true;
    }

    void saveOrders()
    {
        ofstream file(ordersFile);
        file << "id,clientUsername,productList\n";
        for (auto& o : orders) {
            file << o.id << "," << o.clientUsername << "," << serializeProducts(o.productList) << "\n";
        }
    }

    DatabaseManager(string u, string p, string o) {
        usersFile = u;
        productsFile = p;
        ordersFile = o;
        loadUsers();
        loadProducts();
        loadOrders();
        if (users.empty()) {
            users.push_back(User("admin", "", "admin", "Admin", 1, "admin@example.com", "Admin Address"));
        }
    }
    User* authenticateUser(string u, string p) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& user : users) {
            if (user.getUsername() == u && user.getPassword() == p) {
                return new User(user.getUsername(), user.getPhone(), user.getPassword(), user.getRole(), user.getId());
            }
        }
        return nullptr;
    }
    bool registerUser(string u, string phone, string p, string r, string email = "", string address = "") {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& user : users) {
            if (user.getUsername() == u) return false;
        }
        int id = users.size() + 1;
        users.push_back(User(u, phone, p, r, id, email, address));
        saveUsers();
        return true;
    }
    User* getUserById(int id) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& user : users) {
            if (user.getId() == id) {
                return &user;
            }
        }
        return nullptr;
    }
    bool updateUser(int id, const string& name, const string& email, const string& phone, const string& address, const string& password) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& user : users) {
            if (user.getId() == id) {
                user.setUsername(name);
                user.setEmail(email);
                user.setPhone(phone);
                user.setAddress(address);
                if (!password.empty()) user.setPassword(password);
                saveUsers();
                return true;
            }
        }
        return false;
    }
    vector<Product> getAllProducts() {
        std::lock_guard<std::mutex> lock(dbMutex);
        return products;
    }
    vector<Order> getAllOrders() {
        std::lock_guard<std::mutex> lock(dbMutex);
        return orders;
    }
    Product* getProductById(int id) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& p : products) {
            if (p.getId() == id) return &p;
        }
        return nullptr;
    }
    void saveOrder(int userId, vector<CartItem> items, double total) {
        std::lock_guard<std::mutex> lock(dbMutex);
        int id = orders.size() + 1;
        Order o;
        o.id = id;
        o.clientUsername = "user" + to_string(userId);
        for (auto& item : items) {
            o.productList.push_back({item.product.getId(), item.quantity});
        }
        orders.push_back(o);
        saveOrders();
    }
    void updateStock(vector<CartItem> items) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& item : items) {
            for (auto& p : products) {
                if (p.getId() == item.product.getId()) {
                    p = Product(p.getId(), p.getName(), p.getPrice(), p.getCount() - item.quantity, p.getDiscount());
                    break;
                }
            }
        }
        // saveProducts();
    }

    void addProduct(string name, double price, int stock, string category = "", string description = "", string photo = "") {
        std::lock_guard<std::mutex> lock(dbMutex);
        int id = products.size() + 1;
        products.push_back(Product(id, name, price, stock, 0, category, description, photo));
        // saveProducts();
    }

    bool loadUsers()
    {
        users.clear();

        ifstream file(usersFile);

        if (!file)
            return false;

        string line;
        getline(file, line);

        while (getline(file, line))
        {
            auto fields = splitCSV(line);

            if (fields.size() >= 7) {
                users.push_back(
                    User(fields[0], fields[1], fields[2], fields[3], stoi(fields[4]), fields[5], fields[6])
                );
            }
        }

        return true;
    }

    bool loadProducts()
    {
        products.clear();

        ifstream file(productsFile);

        if (!file)
            return false;

        string line;
        getline(file, line);

        while (getline(file, line))
        {
            auto f = splitCSV(line);

            products.push_back(
                Product(
                    stoi(f[0]),
                    f[1],
                    stod(f[2]),
                    stoi(f[3]),
                    stod(f[4]),
                    f.size() > 5 ? f[5] : "",
                    f.size() > 6 ? f[6] : "",
                    f.size() > 7 ? f[7] : ""
                )
            );
        }

        return true;
    }

    bool saveProducts()
    {
        ofstream file(productsFile);

        file << "id,name,price,count,discount,category,description,photo\n";

        for (auto& p : products)
        {
            file << p.getId() << ","
                << p.getName() << ","
                << p.getPrice() << ","
                << p.getCount() << ","
                << p.getDiscount() << ","
                << p.getCategory() << ","
                << p.getDescription() << ","
                << p.getPhoto() << "\n";
        }

        return true;
    }

    Product* findProduct(int id)
    {
        for (auto& p : products)
            if (p.getId() == id)
                return &p;

        return nullptr;
    }

    void showProducts()
    {
        for (auto& p : products)
        {
            cout << p.getId() << " "
                << p.getName() << " "
                << p.getPrice() << " "
                << p.getCount() << " "
                << p.getDiscount() << endl;
        }
    }
};