#include <bits/stdc++.h>
#include <dpp/dpp.h>
// using namespace std;

const std::string BOT_TOKEN = "MTExMDU3MzAyOTkyMDA5MjIwMQ.G2gU53.gKCb4WctcsU5YbLOu_d_4UjjciUF3GoeEKC9wg";
int main() {
    dpp::cluster bot(BOT_TOKEN);
    bot.on_log(dpp::utility::cout_logger());
    bot.on_slashcommand([](const dpp::slashcommand_t& event){
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
        if (event.command.get_command_name() == "rng") {
            event.reply(std::to_string(rand()));
        }
        if (event.command.get_command_name() == "echo") {
            //dpp::command_value
            event.get_parameter("")
        }
    });
    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            dpp::slashcommand echo("echo", "right back at you", bot.me.id);
            echo.add_option(
                dpp::command_option(dpp::co_string, "text", "your message", true).
                add_choice
            )
            bot.global_command_create(dpp::slashcommand("ping", "ping pong!", bot.me.id));
            bot.global_command_create(dpp::slashcommand("rng", "generate random integer", bot.me.id));
            //bot.global_command_create(dpp::slashcommand("", "generate random integer", bot.me.id));
        }
    });
    bot.start(dpp::st_wait);
    return 0;
}
