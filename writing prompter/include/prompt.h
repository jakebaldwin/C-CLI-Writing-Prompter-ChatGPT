//
//  prompt.h
//  writing prompter
//
//  Created by Jake Baldwin on 3/25/23.
//

#ifndef prompt_h
#define prompt_h

#include <string>
#include <map>
#include "ChatGPT.h"

class Prompts {
    private:
	
		std::map<std::string, std::map<std::string, std::string>> prompts_and_responses;
		std::map<std::string, std::string> int_to_prompt;
		int numPrompts = 1;
		ChatGPT chat = ChatGPT("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");
		void loadData();
		

        
    public:
        Prompts();
		std::string random_prompt();
		std::string random_prompt(std::string inspiration_string);
		bool record_response(std::string prompt_id, std::string response);
		std::string get_prompts();
		std::string get_response(std::string prompt_id);
		void saveData();
	
};


#endif /* prompt_h */
