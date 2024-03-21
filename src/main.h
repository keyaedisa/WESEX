#include <dpp/dpp.h>
#include <dpp/guild.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <functional>

using json = nlohmann::json;

const std::string BOT_TOKEN = "";

static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *userp);
void on_http_completion(const dpp::http_request_completion_t& completion);
std::string getSolanaWalletBalance(const json& jsonData);
void getAccountInfo(const std::string& walletAddress, std::function<void(const dpp::embed&)> callback);

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
  userp->append((char*)contents, size * nmemb);
  return size * nmemb;
}


std::string getSolanaWalletBalance(const json& jsonData) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        std::string jsonDataStr = jsonData.dump();

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8899");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonDataStr.c_str());
       
        // Set the headers
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);



        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    // Parse the JSON response
    nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

    // Extract the balance value from the "value" field
    std::string balance = std::to_string(jsonResponse["result"]["value"].get<long long>() / 1000000000.0) + " SOL";

    return balance;
}

const std::string walletAddr = "8wCUuBHDXatnMHLQqM5NRfkK9izifZ55pH5sX6eC8Woq";  
	
void on_http_completion(const dpp::http_request_completion_t& completion) {
    // Check if the request was successful
    if (completion.status == 200) {
      // Process the successful response
      std::cout << "Success! Response body: " << completion.body << std::endl;
  } else {
    // Handle errors
    std::cerr << "HTTP request failed with status code: " << completion.status << std::endl;
  }
}

void makeSolanaBalanceRequest(const std::string& walletAddress) {
   std::string postData = R"({"jsonrpc": "2.0", "id": 1, "method": "getBalance", "params": [")" + walletAddress + R"("]})";
   std::multimap<std::string, std::string> headers = {{"Content-Type", "application/json"}};

   // Create and send the HTTP request using the completion event callback
   dpp::http_request request(
       "http://localhost:8899",                // URL
       on_http_completion,                     // Completion event callback
       dpp::http_method::m_post,               // HTTP method as POST
       postData,                               // POST data
       "application/json",                     // MIME type
       headers                                 // Headers
   );
}

void getAccountInfo(const std::string& walletAddress, std::function<void(const dpp::embed&)> callback) {
    CURL *curl = curl_easy_init();
    if (curl) {
        std::string readBuffer;
        std::string postData = json{
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "getAccountInfo"},
            {"params", {walletAddress}}
        }.dump();

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8899");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            json response = json::parse(readBuffer);

            // Create a Discord embed with the JSON response
            dpp::embed embed;
            embed.set_title("Account Information");
            embed.set_description("Details for wallet address: " + walletAddress);

            // Iterate over the key-value pairs in the JSON response
            for (const auto& [key, value] : response["result"]["value"].items()) {
                std::string field_name = key;
                std::string field_value = value.dump();

                // Add the key-value pair as a field in the embed
                embed.add_field(field_name, field_value);
            }

            callback(embed);

            /*json response = json::parse(readBuffer);
            callback(response.dump()); // Pass the entire response JSON as a string */
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
