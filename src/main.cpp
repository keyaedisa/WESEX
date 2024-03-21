#include <dpp/dpp.h>
#include <dpp/guild.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <functional>
#include "main.h"


using json = nlohmann::json;



	int main() {
      json jRequest;
      jRequest["jsonrpc"] = "2.0";
      jRequest["id"] = 1;
      jRequest["method"] = "getBalance";
      jRequest["params"] = {"8wCUuBHDXatnMHLQqM5NRfkK9izifZ55pH5sX6eC8Woq"};

	    dpp::cluster bot(BOT_TOKEN);

      //std::string outputChannelID;
	 
	    bot.on_log(dpp::utility::cout_logger());
	 
	    bot.on_slashcommand([&bot, jRequest](const dpp::slashcommand_t& event) {
          if (event.command.get_command_name() == "ping") {
	            event.reply("Pong!");
	        } else if (event.command.get_command_name() == "getwalletbalance") {
              std::string walletAddress = std::get<std::string>(event.get_parameter("wallet_address"));  
              std::string balance = getSolanaWalletBalance(jRequest);

              dpp::embed embed;
              embed.set_description(std::string("The balance for this wallet is: ") + balance);
              dpp::message msg;
              msg.add_embed(embed);
              dpp::interaction_response response(dpp::ir_channel_message_with_source, msg);
              bot.interaction_response_create(event.command.id, event.command.token, response);

	        } else if (event.command.get_command_name() == "getaccountinfo") {
              std::string walletAddress = std::get<std::string>(event.get_parameter("wallet_address"));
              
              getAccountInfo(walletAddress, [&bot, &event](const dpp::embed& embed) {
                  // Prepare the interaction response with the embed
                  dpp::message msg;
                  msg.add_embed(embed); // Add the embed to the message
                  dpp::interaction_response response(dpp::ir_channel_message_with_source, msg);
                  bot.interaction_response_create(event.command.id, event.command.token, response);
              });
	        } else if (event.command.get_command_name() == "setchannel") {
            // Assuming isAdmin logic is implemented elsewhere
            // Fetch the guild_member object to access roles and check permissions
              // Assuming we have a way to get the guild (server) ID from the event
            std::string outputChannelID;
            dpp::snowflake guild_id = event.command.guild_id;
            bool isAdmin = false;
            // Get member from the event
            dpp::permission perms = event.command.get_resolved_permission(event.command.usr.id);
            // Check if member has the admin role
            isAdmin = perms.has(dpp::p_administrator);

            if (isAdmin) {
                // Extract channel ID from the command and update outputChannelID
                dpp::snowflake channelID = std::get<dpp::snowflake>(event.get_parameter("channel_id"));
                outputChannelID = std::to_string(channelID);
                event.reply("Output channel set successfully!");
            } else {
                event.reply("You do not have permission to use this command.");
            }
          } else if (event.command.get_command_name() == "cock") {
              event.reply("cum!!");
          };
      });	 
	    bot.on_ready([&bot, jRequest](const dpp::ready_t& event) {
        std::string mypostdata = R"({"jsonrpc": "2.0", "id": 1, "method": "getBalance", "params": ["8wCUuBHDXatnMHLQqM5NRfkK9izifZ55pH5sX6eC8Woq"]})";
	        if (dpp::run_once<struct register_bot_commands>()) {
              dpp::slashcommand getwalletbalance("getwalletbalance", "gets a wallet balance!!", bot.me.id);
              getwalletbalance.add_option(dpp::command_option(dpp::co_string, "wallet_address", "The address of the wallet", true));
              dpp::slashcommand pingcommand("ping", "Ping pong!!!", bot.me.id);
              dpp::slashcommand getaccountinfocommand("getaccountinfo", "Get account information", bot.me.id);
              getaccountinfocommand.add_option(dpp::command_option(dpp::co_string, "wallet_address", "The Solana wallet address", true));
              dpp::slashcommand setchannel("setchannel", "Sets the bot's operational channel", bot.me.id);
              setchannel.add_option(dpp::command_option(dpp::co_channel, "channel_id", "The channel the bot should operate in", true));
              dpp::slashcommand cock("cock", "get cock!!", bot.me.id);
              bot.global_bulk_command_create({ getwalletbalance, pingcommand, getaccountinfocommand, setchannel, cock });
	        }
	    });
	 
	    bot.start(dpp::st_wait);

      return 0;
  }


