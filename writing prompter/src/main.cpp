//
//  main.cpp
//  writing prompter
//
//  Created by Jake Baldwin on 3/25/23.
//

#include <iostream>
#include <cstring>
#include "../include/prompt.h"

int main(int argc, const char * argv[]) {
    
    Prompts prompts;
    std::string command;
    
    while (true) {
        
        
        std::cout << ">>";
        getline(std::cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        // Remove trailing whitespace from the command
        command.erase(command.find_last_not_of(" \t\n\r\f\v") + 1);
        
        
        if (!strcmp(command.c_str(), "help")) {
            
            std::cout << "Commands:\n`rp` == Random Prompt: it will give you a random prompt and prompt you for your response\n`rpi` == Random Prompt with Inspiration: will prompt you for inspirational text then generate a random prompt\n`gp` == Get Prompt: will list you all the prompts and their corresponding ids, (the numbers)\n`wr` == Write Response: enter the prompt id and your response\n`exit` == Exit the program\n";
            
        } else if (!strcmp(command.c_str(), "rp")) {
            
            std::cout << "Random Prompt\n" << prompts.random_prompt() << "\n";
            
        } else if (!strcmp(command.c_str(), "rpi")) {
            
            std::cout << "Random Prompt with Inspiration\nWrite your inspiration to Chat GPT:";
            
            std::string inspiration_string;
            getline(std::cin, inspiration_string);
            
            std::cout << prompts.random_prompt(inspiration_string);
            
            
        } else if (!strcmp(command.c_str(), "gp")) {
            
            std::cout << "Get Prompts\n" << prompts.get_prompts() << "\n";
            
        } else if (!strcmp(command.c_str(), "wr")) {
            std::string prompt;
            std::string response;
            
            std::cout << "Record Response\nWhat is the prompt id for which you are responding (1, 2, 3...):";
            std::cin >> prompt;
    
            std::cout << "Write your response below:\n";
            getline(std::cin, response);
            
            if (prompts.record_response(prompt, response)) {
                std::cout << "Successfully Recorded Prompt";
            } else {
                std::cout << "Unsuccessfully recorded prompt";
            }
            
        } else if (!strcmp(command.c_str(), "save")) {
            prompts.saveData();
            
        } else if (!strcmp(command.c_str(), "rr")) {
            std::cout << "Read Response\nEnter Prompt Number or type view: ";
            std::string resp = "view";
            std::cin >> resp;
            
            while (!strcmp(command.c_str(), "view")) {
                std::cout << prompts.get_prompts() << "\nRead Response\nEnter Prompt Number or type view: ";
                std::cin >> resp;
            }
            std::cout << prompts.get_response(resp) << "\n";
            
            
        } else if (!strcmp(command.c_str(), "exit")) {
            std::cout << "Exiting\n";
            break;
        }
        
    }
    
    std::cout << "The end of the wonderful program\n\n";
    return 0;
}
