#include <dpp/dpp.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <functional>


using json = nlohmann::json;

const std::string BOT_TOKEN = "";

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
	 
  void getSolanaWalletBalance2(const std::string& wallet_address, std::function<void(const std::string&)> callback) {
    CURL *curl = curl_easy_init();
    if (curl) {
        std::string readBuffer;
        std::string postData = json{
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "getBalance"},
            {"params", {wallet_address}}
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
            long long lamports = response["result"]["value"].get<long long>();
            double sol = lamports / 1000000000.0;
            callback(std::to_string(sol) + " SOL");
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

	int main() {
      json jRequest;
      jRequest["jsonrpc"] = "2.0";
      jRequest["id"] = 1;
      jRequest["method"] = "getBalance";
      jRequest["params"] = {"8wCUuBHDXatnMHLQqM5NRfkK9izifZ55pH5sX6eC8Woq"};

	    dpp::cluster bot(BOT_TOKEN);
	 
	    bot.on_log(dpp::utility::cout_logger());
	 
	    bot.on_slashcommand([&bot, jRequest](const dpp::slashcommand_t& event) {
	        if (event.command.get_command_name() == "ping") {
	            event.reply("Pong!");
	        } else if (event.command.get_command_name() == "getwalletbalance") {
              std::string walletAddress = std::get<std::string>(event.get_parameter("wallet_address"));  
              std::string balance = getSolanaWalletBalance(jRequest);                         

	            event.reply(std::string("The balance for this wallet is:") + balance);
	        } else if (event.command.get_command_name() == "getbalance") {
              std::string walletAddress = std::get<std::string>(event.get_parameter("wallet_address"));

              getSolanaWalletBalance2(walletAddress, [&bot, &event](const std::string& balance) {
                dpp::interaction_response response(dpp::ir_channel_message_with_source, dpp::message("Wallet balance: " + balance));
                bot.interaction_response_create(event.command.id, event.command.token, response);

              });
	        };
      });	 
	    bot.on_ready([&bot, jRequest](const dpp::ready_t& event) {
        std::string bal2 = getSolanaWalletBalance(jRequest2);
        std::string mypostdata = R"({"jsonrpc": "2.0", "id": 1, "method": "getBalance", "params": ["8wCUuBHDXatnMHLQqM5NRfkK9izifZ55pH5sX6eC8Woq"]})";
	        if (dpp::run_once<struct register_bot_commands>()) {
              dpp::slashcommand getwalletbalance("getwalletbalance", "gets a wallet balance!!", bot.me.id);
              getwalletbalance.add_option(dpp::command_option(dpp::co_string, "wallet_address", "The address of the wallet", true));
              dpp::slashcommand pingCommand("ping", "Ping pong!!!", bot.me.id);
              dpp::slashcommand getBalanceCMD("getbalance", "Get Solana wallet balance", bot.me.id);
              getBalanceCMD.add_option(dpp::command_option(dpp::co_string, "wallet_address", "The Solana wallet address", true));
              bot.global_bulk_command_create({ getwalletbalance, pingCommand, getBalanceCMD });       
	        }
	    });
	 
	    bot.start(dpp::st_wait);

      return 0;
  }


