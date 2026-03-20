#include "../include/Interface.hpp"
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    Interface app;
    httplib::Server svr;

    // Run handlers on a single worker to avoid races on shared session state.
    svr.new_task_queue = [] { return new httplib::ThreadPool(1); };

    auto getUserIdFromJson = [](const nlohmann::json& j) -> int {
        if (!j.contains("userId")) return 0;
        try {
            if (j["userId"].is_number_integer()) return j["userId"].get<int>();
            if (j["userId"].is_string()) return std::stoi(j["userId"].get<std::string>());
        } catch (...) {
            return 0;
        }
        return 0;
    };

    auto log = [](const std::string& msg) {
        std::cout << "[WEB] " << msg << std::endl;
    };

    // Авторизация
    svr.Post("/auth/login", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            std::string username = j["username"];
            std::string password = j["password"];
            User* user = app.apiLogin(username, password);
            if (user) {
                nlohmann::json response = {{"status", "success"}, {"message", "Login successful"}, {"role", user->getRole()}, {"user_id", user->getId()}};
                res.set_content(response.dump(), "application/json");
                log("login success user=" + username + " id=" + std::to_string(user->getId()));
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Invalid username or password\"}", "application/json");
                log("login failed user=" + username);
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
            log("login parse error");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Post("/auth/register", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            std::string username = j["username"];
            std::string password = j["password"];
            std::string email = j.value("email", "");
            std::string address = j.value("address", "");
            if (app.apiRegister(username, password, email, address)) {
                res.set_content("{\"status\": \"success\", \"message\": \"Registration successful\"}", "application/json");
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Username already exists\"}", "application/json");
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });


    // Products
    svr.Get("/products", [&](const httplib::Request& req, httplib::Response& res) {
        auto products = app.apiGetProducts();
        nlohmann::json j = nlohmann::json::array();
        for (const auto& p : products) {
            j.push_back({{"id", p.getId()}, {"name", p.getName()}, {"price", p.getPrice()}, {"stock", p.getCount()}, {"category", p.getCategory()}, {"description", p.getDescription()}, {"photo", p.getPhoto()}});
        }
        res.set_content(j.dump(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    // Cart API
    svr.Get("/api/cart", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("user_id")) {
            int userId = std::stoi(req.get_param_value("user_id"));
            app.apiSetCurrentUser(userId);
            log("cart fetch for user_id=" + std::to_string(userId));
        } else {
            log("cart fetch anonymous/no user_id");
        }
        // Get cart items
        auto cartItems = app.apiGetCartItems();
        nlohmann::json items = nlohmann::json::array();
        for (auto& item : cartItems) {
            items.push_back({
                {"product", {{"id", item.product.getId()}, {"name", item.product.getName()}, {"price", item.product.getPrice()}}},
                {"quantity", item.quantity}
            });
        }
        double total = app.apiGetCartTotal();
        nlohmann::json j = {{"items", items}, {"total", total}};
        res.set_content(j.dump(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Post("/cart/items", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            int productId = j["productId"];
            int userId = getUserIdFromJson(j);
            if (userId > 0) {
                app.apiSetCurrentUser(userId);
            }
            if (app.apiAddToCart(productId)) {
                res.set_content("{\"status\": \"success\", \"message\": \"Item added to cart\"}", "application/json");
                log("add-to-cart ok user=" + std::to_string(userId) + " product=" + std::to_string(productId));
            } else {
                std::string reason = "Failed to add item";
                if (!app.isLoggedIn()) reason += " - not logged in";
                else reason += " - other reason";
                res.set_content("{\"status\": \"error\", \"message\": \"" + reason + "\"}", "application/json");
                log("add-to-cart fail user=" + std::to_string(userId) + " product=" + std::to_string(productId) + " reason=" + reason);
            }
        } catch (const std::exception& e) {
            res.set_content("{\"status\": \"error\", \"message\": \"Server error: " + std::string(e.what()) + "\"}", "application/json");
            log(std::string("add-to-cart exception: ") + e.what());
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Unknown server error\"}", "application/json");
            log("add-to-cart unknown exception");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Delete("/cart/items", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            int productId = j["productId"];
            int userId = getUserIdFromJson(j);
            if (userId > 0) {
                app.apiSetCurrentUser(userId);
            }
            if (app.apiRemoveFromCart(productId)) {
                res.set_content("{\"status\": \"success\", \"message\": \"Item removed from cart\"}", "application/json");
                log("remove-from-cart ok user=" + std::to_string(userId) + " product=" + std::to_string(productId));
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Failed to remove item\"}", "application/json");
                log("remove-from-cart fail user=" + std::to_string(userId) + " product=" + std::to_string(productId));
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
            log("remove-from-cart parse error");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Put("/cart/items", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            int productId = j["productId"];
            int quantity = j["quantity"];
            int userId = getUserIdFromJson(j);
            if (userId > 0) {
                app.apiSetCurrentUser(userId);
            }
            if (app.apiUpdateCartQuantity(productId, quantity)) {
                res.set_content("{\"status\": \"success\", \"message\": \"Quantity updated\"}", "application/json");
                log("update-qty ok user=" + std::to_string(userId) + " product=" + std::to_string(productId) + " qty=" + std::to_string(quantity));
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Failed to update quantity\"}", "application/json");
                log("update-qty fail user=" + std::to_string(userId) + " product=" + std::to_string(productId) + " qty=" + std::to_string(quantity));
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
            log("update-qty parse error");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Post("/admin/import-products", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            std::string csvData = j["csvData"];
            try {
                std::stringstream ss(csvData);
                std::string line;
                std::getline(ss, line); // skip header
                int count = 0;
                while (std::getline(ss, line)) {
                    std::stringstream lineStream(line);
                    std::string name, cost, stock, category, description, photo;
                    std::getline(lineStream, name, ',');
                    std::getline(lineStream, cost, ',');
                    std::getline(lineStream, stock, ',');
                    std::getline(lineStream, category, ',');
                    std::getline(lineStream, description, ',');
                    std::getline(lineStream, photo, ',');
                    double price = std::stod(cost);
                    int stk = std::stoi(stock);
                    if (app.apiAddProduct(name, price, stk, category, description, photo)) {
                        count++;
                    }
                }
                res.set_content("{\"status\": \"success\", \"message\": \"Imported " + std::to_string(count) + " products\"}", "application/json");
            } catch (...) {
                res.set_content("{\"status\": \"error\", \"message\": \"Invalid CSV data\"}", "application/json");
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    // Checkout
    svr.Post("/checkout", [&](const httplib::Request& req, httplib::Response& res) {
        // Allow stateless calls by accepting user_id query param or body
        try {
            if (req.has_param("user_id")) {
                int userId = std::stoi(req.get_param_value("user_id"));
                app.apiSetCurrentUser(userId);
                log("checkout with user_id param=" + std::to_string(userId));
            } else if (!req.body.empty()) {
                auto j = nlohmann::json::parse(req.body);
                int userId = getUserIdFromJson(j);
                if (userId > 0) app.apiSetCurrentUser(userId);
                log("checkout with body userId=" + std::to_string(userId));
            }
        } catch (...) {
            // ignore parsing errors, checkout will fail if not logged in
            log("checkout parse error");
        }
        if (app.apiCheckout()) {
            res.set_content("{\"status\": \"success\", \"message\": \"Checkout complete\"}", "application/json");
            log("checkout success");
        } else {
            res.set_content("{\"status\": \"error\", \"message\": \"Checkout failed\"}", "application/json");
            log("checkout failed");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    // Admin
    svr.Get("/admin/products", [&](const httplib::Request& req, httplib::Response& res) {
        // app.showAdminMenu(); // console
        res.set_content("{\"admin\": \"Admin data\"}", "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Post("/admin/products", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            std::string name = j["name"];
            double price = j["price"];
            int stock = j["stock"];
            std::string category = j.value("category", "");
            std::string description = j.value("description", "");
            std::string photo = j.value("photo", "");
            if (app.apiAddProduct(name, price, stock, category, description, photo)) {
                res.set_content("{\"status\": \"success\", \"message\": \"Product added\"}", "application/json");
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Failed to add product\"}", "application/json");
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Get("/admin/orders", [&](const httplib::Request& req, httplib::Response& res) {
        auto orders = app.apiGetOrders();
        auto products = app.apiGetProducts();
        nlohmann::json j = nlohmann::json::array();
        for (auto& o : orders) {
            double total = 0.0;
            for (auto& p : o.productList) {
                for (auto& prod : products) {
                    if (prod.getId() == p.first) {
                        total += prod.getPrice() * p.second;
                        break;
                    }
                }
            }
            // Extract userId from clientUsername "userX"
            int userId = std::stoi(o.clientUsername.substr(4));
            auto user = app.apiGetUserById(userId);
            std::string userName = user ? user->getUsername() : "Unknown";
            std::string address = user ? user->getAddress() : "Unknown";
            j.push_back({
                {"id", o.id},
                {"userName", userName},
                {"address", address},
                {"total", total}
            });
        }
        res.set_content(j.dump(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Get("/admin", [&](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("frontend/layouts/admin.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_content(content, "text/html");
    });

    svr.Get("/account", [&](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("frontend/layouts/account.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_content(content, "text/html");
    });

    svr.Get("/cart", [&](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("frontend/layouts/cart.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_content(content, "text/html");
    });

    svr.Post("/admin/users", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            std::string username = j["username"];
            std::string phone = j["phone"];
            std::string password = j["password"];
            std::string email = j["email"];
            std::string address = j["address"];
            std::string role = j["role"];
            if (app.apiAddUser(username, phone, password, email, address, role)) {
                res.set_content("{\"status\": \"success\", \"message\": \"User added\"}", "application/json");
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Username already exists\"}", "application/json");
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Post("/admin/products", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            std::string name = j["name"];
            double price = j["price"];
            int stock = j["stock"];
            if (app.apiAddProduct(name, price, stock)) {
                res.set_content("{\"status\": \"success\", \"message\": \"Product added\"}", "application/json");
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Failed to add product\"}", "application/json");
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Get("/user/info", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("user_id")) {
            int user_id = std::stoi(req.get_param_value("user_id"));
            auto user = app.apiGetUserById(user_id);
            if (user) {
                nlohmann::json j = {{"id", user->getId()}, {"name", user->getUsername()}, {"email", user->getEmail()}, {"phone", user->getPhone()}, {"address", user->getAddress()}};
                res.set_content(j.dump(), "application/json");
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"User not found\"}", "application/json");
            }
        } else {
            res.set_content("{\"status\": \"error\", \"message\": \"Missing user_id\"}", "application/json");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    svr.Post("/user/update", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = nlohmann::json::parse(req.body);
            int user_id = j["id"];
            std::string name = j["name"];
            std::string email = j["email"];
            std::string phone = j["phone"];
            std::string address = j["address"];
            std::string password = j["password"];
            if (app.apiUpdateUser(user_id, name, email, phone, address, password)) {
                res.set_content("{\"status\": \"success\", \"message\": \"User updated\"}", "application/json");
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Failed to update user\"}", "application/json");
            }
        } catch (...) {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        });

    // Serve frontend
    svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_redirect("/catalog");
    });

    svr.Get("/signin", [&](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("frontend/layouts/signin.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_content(content, "text/html");
    });

    svr.Get("/signup", [&](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("frontend/layouts/signup.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_content(content, "text/html");
    });

    svr.Get("/catalog", [&](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("frontend/layouts/catalog.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_content(content, "text/html");
    });

    std::cout << "Server started!" << std::endl;
    svr.listen("0.0.0.0", 3000);

    return 0;
}