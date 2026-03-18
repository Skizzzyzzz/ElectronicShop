#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class User
{
private:
    string username;
    string phone;
    string password;

public:
    User() {}

    User(string username, string phone, string password)
    {
        this->username = username;
        this->phone = phone;
        this->password = password;
    }

    string getUsername() const { return username; }
    string getPhone() const { return phone; }
    string getPassword() const { return password; }
};

class Product
{
private:
    int id;
    string name;
    double price;
    int count;
    double discount;

public:
    Product() {}

    Product(int id, string name, double price, int count, double discount)
    {
        this->id = id;
        this->name = name;
        this->price = price;
        this->count = count;
        this->discount = discount;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getCount() const { return count; }
    double getDiscount() const { return discount; }
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

            users.push_back(
                User(fields[0], fields[1], fields[2])
            );
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
                    stod(f[4])
                )
            );
        }

        return true;
    }

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

    bool saveProducts()
    {
        ofstream file(productsFile);

        file << "id,name,price,count,discount\n";

        for (auto& p : products)
        {
            file << p.getId() << ","
                << p.getName() << ","
                << p.getPrice() << ","
                << p.getCount() << ","
                << p.getDiscount() << "\n";
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

int main()
{
    DatabaseManager db;

    db.loadUsers();
    db.loadProducts();
    db.loadOrders();

    cout << "Products:\n";
    db.showProducts();

    return 0;
}