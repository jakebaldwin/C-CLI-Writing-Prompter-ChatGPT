//
//  prompt.cpp
//  writing prompter
//
//  Created by Jake Baldwin on 3/25/23.
//

#ifndef prompt_cpp
#define prompt_cpp

#include "../include/prompt.h"
#include "../include/ChatGPT.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <sqlite3.h>
#include <cstdlib>

static int selectCallback(void* data, int argc, char** argv, char** colName) {
  auto promptsAndResponses = static_cast<std::map<std::string, std::map<std::string, std::string>>*>(data);

  // Check if prompt already exists in the map
  auto it = promptsAndResponses->find(argv[0]);
  if (it == promptsAndResponses->end()) {
    // Create new sub-map if prompt does not exist in the map
    (*promptsAndResponses)[argv[0]] = std::map<std::string, std::string>{{argv[1], argv[2]}};
  } else {
    // Add to existing sub-map if prompt already exists in the map
    it->second[argv[1]] = argv[2];
  }

  return 0;
}

void Prompts::loadData() {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("prompts.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    //sqlite3_exec(db, "DROP TABLE IF EXISTS prompts_and_responses;", nullptr, nullptr, nullptr);
    const char *query = "SELECT name FROM sqlite_master WHERE type='table' AND name='prompts_and_responses'";
    bool tableExists = false;
    rc = sqlite3_exec(db, query, [](void *data, int argc, char **argv, char **colName) -> int {
        *(bool*)data = true;
        return 0;
    }, &tableExists, nullptr);
    
    if (tableExists) {
        // load values unless table is empty
        int count = 0;
        rc = sqlite3_exec(db, "SELECT COUNT(*) FROM prompts_and_responses", [](void* count_ptr, int argc, char** argv, char** azColName) -> int {
            if (argc > 0) {
                *((int*)count_ptr) = std::atoi(argv[0]);
            }
            return 0;
        }, &count, &zErrMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Error checking table count: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            return;
        }

        if (count == 0) {
            std::cout << "Table is empty. Starting Program..." << std::endl;
        } else {
            std::cout << "Table is not empty, loading values = " << count << std::endl;
            std::string sql = "SELECT prompt, date_key, response_value FROM prompts_and_responses;";
            char* errMsg;
            rc = sqlite3_exec(db, sql.c_str(), selectCallback, &prompts_and_responses, &errMsg);
            if (rc != SQLITE_OK) {
                std::cerr << "Error executing SELECT statement: " << errMsg << std::endl;
                sqlite3_free(errMsg);
                sqlite3_close(db);
            }
            
            for (auto const &pair : prompts_and_responses) {
                int_to_prompt[std::to_string(numPrompts++)] = pair.first;
            }
        }
        
    } else {
        std::cout << "Table not found, creating...";
        std::string sql = "CREATE TABLE IF NOT EXISTS prompts_and_responses ("
                          "  prompt TEXT,"
                          "  date_key TEXT,"
                          "  response_value TEXT"
                          ");";

        rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Error creating table: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            return;
        }
    }
    
}

void Prompts::saveData() {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("prompts.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    for (const auto& prompt_entry : prompts_and_responses) {
        const std::string& prompt = prompt_entry.first;
        const std::map<std::string, std::string>& responses = prompt_entry.second;

        for (const auto& response_entry : responses) {
            const std::string& date_key = response_entry.first;
            const std::string& response_value = response_entry.second;

            std::string sql = "INSERT INTO prompts_and_responses (prompt, date_key, response_value) "
                  "VALUES ('" + prompt + "', '" + date_key + "', '" + response_value + "');";

            rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
            if (rc != SQLITE_OK) {
                std::cerr << "SQL error: " << zErrMsg << std::endl;
                sqlite3_free(zErrMsg);
                sqlite3_close(db);
                return;
            }
        }
    }

    sqlite3_close(db);
}


Prompts::Prompts() {
    
    loadData();
}

std::string todaysDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::ctime(&now_c);
    std::string date_string = ss.str();
    
    return date_string;
}

std::string Prompts::random_prompt() {
    // function should use chatgpt api to generate random prompt
    // store prompt in int_to_
    std::string prompt = chat.get_prompt();
    std::string output = std::to_string(numPrompts) + ". " + prompt;
    
    std::map<std::string, std::string> date_and_response;
    prompts_and_responses.insert(std::make_pair(prompt, date_and_response));
    int_to_prompt[std::to_string(numPrompts++)] = prompt;
    
    return output;
}

std::string Prompts::random_prompt(std::string inspiration_string) {
    // Chat GPT-4's response to "Generate a writing prompt given the inspiration string: inspiration_string"
    std::string prompt = chat.get_prompt(inspiration_string);
    std::string output = std::to_string(numPrompts) + ". " + prompt;
    
    std::map<std::string, std::string> date_and_response;
    prompts_and_responses.insert(std::make_pair(prompt, date_and_response));
    int_to_prompt[std::to_string(numPrompts++)] = prompt;
    
    return output;
}

bool Prompts::record_response(std::string prompt_id, std::string response) {
    // should write the response to the map using todays date helper function
    std::string prompt = int_to_prompt[prompt_id];
    auto iter = prompts_and_responses.find(prompt);
    if (iter != prompts_and_responses.end()) {
        iter->second[todaysDate()] = response;
    } else {
        std::map<std::string, std::string> date_response_pair;
        date_response_pair[todaysDate()] = response;
        prompts_and_responses.insert(std::make_pair(prompt, date_response_pair));
        int_to_prompt[std::to_string(numPrompts++)];
    }
    
    return true;
}

std::string Prompts::get_prompts() {
    std::string output = "\n";
    for (auto const &pair: int_to_prompt) {
        output.append(pair.first);
        output.append(": ");
        output.append(pair.second);
        output.append(".\n");
    }
    output.append("\n");
    return output;
}

std::string Prompts::get_response(std::string prompt_id) {
    std::string prompt = int_to_prompt[prompt_id];
    std::string output;
    
    for (auto const &pair: prompts_and_responses[prompt]) {
        output.append("On ");
        output.append(pair.first);
        output.append("you wrote:");
        output.append(pair.second);
        output.append("\n");
    }
    
    return output;
}


#endif
