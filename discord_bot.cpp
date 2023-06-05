#include <iostream>
#include <random>
#include <string>
#include <climits>
#include <unordered_map>
#include <dpp/dpp.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
//#include "headers/exprtk.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>

void handleCEval(const dpp::slashcommand_t& event) {
    std::string code = std::get<std::string>(event.get_parameter("code"));
    std::ofstream file("temp.c");
    file << code;
    file.close();
    std::system("gcc temp.c -o temp");
    std::stringstream ss;
    ss << std::system("./temp");
    event.reply(ss.str());
}

namespace py = pybind11;
const std::string BOT_TOKEN = "MTExMDU3MzAyOTkyMDA5MjIwMQ.GgDWnX.H0RTmm6gDId37HB4t_0whgUMGvrkNHKQ7PsBSw";

void handlePing(const dpp::slashcommand_t& event) {
    event.reply("Pong!");
}

void handleRng(const dpp::slashcommand_t& event) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, INT_MAX);
    const int randomNum = distribution(generator);
    event.reply(std::to_string(randomNum));
}

void handleEcho(const dpp::slashcommand_t& event) {
    std::string message = std::get<std::string>(event.get_parameter("message"));
    event.reply(message);
}

/*
void handleEval(const dpp::slashcommand_t& event) {
    std::string expression = std::get<std::string>(event.get_parameter("expression"));
    exprtk::symbol_table<double> symbol_table;
    symbol_table.add_constants();
    exprtk::expression<double> expr;
    expr.register_symbol_table(symbol_table);
    exprtk::parser<double> parser;
    parser.compile(expression, expr);
    double result = expr.value();
    event.reply(std::to_string(result));
}
*/

void handlePyEval(const dpp::slashcommand_t& event) {
    std::string code = std::get<std::string>(event.get_parameter("code"));
    py::scoped_interpreter guard{};
    try {
        py::module sys = py::module::import("sys");
        py::object string_io = py::module::import("io").attr("StringIO")();
        sys.attr("stdout") = string_io;
        py::eval<py::eval_statements>(code);
        std::string resultStr = py::str(string_io.attr("getvalue")());
        event.reply("Input: " + code + "\nOutput: " + resultStr);
    } catch (const std::exception& e) {
        event.reply(e.what());
    }
}

void handleSlashCommand(const dpp::slashcommand_t& event) {
    static const std::unordered_map<std::string, void(*)(const dpp::slashcommand_t&)> commandHandlers = {
        {"ping", handlePing},
        {"rng", handleRng},
        {"echo", handleEcho},
        //{"eval", handleEval},
        {"pyeval", handlePyEval},
        {"ceval", handleCEval}
    };

    const auto it = commandHandlers.find(event.command.get_command_name());
    if (it != commandHandlers.end()) {
        it->second(event);
    }
}

void registerBotCommands(dpp::cluster& bot) {
    dpp::slashcommand echo("echo", "right back at you", bot.me.id);
    echo.add_option(dpp::command_option(dpp::co_string, "message", "The message to echo back"));
    bot.global_command_create(echo);

    bot.global_command_create(dpp::slashcommand("ping", "ping pong!", bot.me.id));
    bot.global_command_create(dpp::slashcommand("rng", "generate random integer", bot.me.id));

    dpp::slashcommand eval("eval", "evaluate a mathematical expression", bot.me.id);
    eval.add_option(dpp::command_option(dpp::co_string, "expression", "The expression to evaluate"));
    bot.global_command_create(eval);

    dpp::slashcommand pyeval("pyeval", "evaluate python code", bot.me.id);
    pyeval.add_option(dpp::command_option(dpp::co_string, "code", "The python code to evaluate"));
    bot.global_command_create(pyeval);

    dpp::slashcommand ceval("ceval", "evaluate C code", bot.me.id);
    ceval.add_option(dpp::command_option(dpp::co_string, "code", "The C code to evaluate"));
    bot.global_command_create(ceval);
    
}

int main() {
    dpp::cluster bot(BOT_TOKEN);
    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand(handleSlashCommand);

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            registerBotCommands(bot);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}