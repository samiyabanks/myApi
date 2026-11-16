#include <httplib.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;

// A user record for the API.
struct User {
    int id;
    std::string name;
    std::string email;
};

// In-memory data store for learning purposes.
// In a real app, this would be replaced by MongoDB, SQLite, or a database.
std::vector<User> users = {
    {1, "jenna", "jenna@example.com"},
    {2, "Bob", "bob@example.com"}
};

// Keeps track of the next user ID so every new user gets a unique id.
int nextId = 3;

int main() {
    // Create the HTTP server object.
    httplib::Server svr;

    // GET /health
    // Returns a simple status response so the client can check if the API is alive.
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        json body = {
            {"status", "ok"}
        };
        res.set_content(body.dump(), "application/json");
    });

    // GET /users
    // Returns every user currently stored in memory as a JSON array.
    svr.Get("/users", [](const httplib::Request&, httplib::Response& res) {
        json arr = json::array();

        for (const auto& user : users) {
            arr.push_back({
                {"id", user.id},
                {"name", user.name},
                {"email", user.email}
            });
        }

        res.set_content(arr.dump(), "application/json");
    });

    // GET /users/:id
    // Finds and returns a single user by numeric id.
    svr.Get(R"(/users/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        const std::string idText = req.matches[1].str();
        const int id = std::stoi(idText);

        auto it = std::find_if(users.begin(), users.end(), [&](const User& u) {
            return u.id == id;
        });

        if (it == users.end()) {
            res.status = 404;
            res.set_content(R"({"error":"user not found"})", "application/json");
            return;
        }

        json body = {
            {"id", it->id},
            {"name", it->name},
            {"email", it->email}
        };

        res.set_content(body.dump(), "application/json");
    });

    // POST /users
    // Creates a new user from JSON body data like {"name":"Charlie","email":"charlie@example.com"}.
    svr.Post("/users", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            if (!body.contains("name") || !body["name"].is_string()) {
                res.status = 400;
                res.set_content(R"({"error":"name is required"})", "application/json");
                return;
            }

            if (!body.contains("email") || !body["email"].is_string()) {
                res.status = 400;
                res.set_content(R"({"error":"email is required"})", "application/json");
                return;
            }

            User user{
                nextId++,
                body["name"].get<std::string>(),
                body["email"].get<std::string>()
            };

            users.push_back(user);

            json response = {
                {"id", user.id},
                {"name", user.name},
                {"email", user.email}
            };

            res.status = 201;
            res.set_content(response.dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content(R"({"error":"invalid JSON"})", "application/json");
        }
    });

    // PUT /users/:id
    // Updates an existing user by id. It accepts partial or full JSON updates.
    svr.Put(R"(/users/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        try {
            const std::string idText = req.matches[1].str();
            const int id = std::stoi(idText);
            json body = json::parse(req.body);

            auto it = std::find_if(users.begin(), users.end(), [&](const User& u) {
                return u.id == id;
            });

            if (it == users.end()) {
                res.status = 404;
                res.set_content(R"({"error":"user not found"})", "application/json");
                return;
            }

            if (body.contains("name") && body["name"].is_string()) {
                it->name = body["name"].get<std::string>();
            }

            if (body.contains("email") && body["email"].is_string()) {
                it->email = body["email"].get<std::string>();
            }

            json response = {
                {"id", it->id},
                {"name", it->name},
                {"email", it->email}
            };

            res.set_content(response.dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content(R"({"error":"invalid JSON or bad id"})", "application/json");
        }
    });

    // DELETE /users/:id
    // Removes a user from memory by id.
    svr.Delete(R"(/users/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        const std::string idText = req.matches[1].str();
        const int id = std::stoi(idText);

        auto it = std::find_if(users.begin(), users.end(), [&](const User& u) {
            return u.id == id;
        });

        if (it == users.end()) {
            res.status = 404;
            res.set_content(R"({"error":"user not found"})", "application/json");
            return;
        }

        users.erase(it);
        res.status = 204;
        res.set_content("", "application/json");
    });

    // Prints the server address so you know where to send requests.
    std::cout << "Server running on http://localhost:8080\n";

    // Starts listening for incoming HTTP requests on port 8080.
    svr.listen("0.0.0.0", 8080);
}