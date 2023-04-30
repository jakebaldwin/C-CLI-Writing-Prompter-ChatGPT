//
//  ChatGPT.h
//  writing prompter
//
//  Created by Jake Baldwin on 3/27/23.
//

#ifndef ChatGPT_h
#define ChatGPT_h

#include <curl/curl.h>
#include <string>

class ChatGPT {
    
    private:
        std::string api_key_;
        std::string api_url_ = "https://api.openai.com/v1/";

        static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);

        std::string perform_request(const std::string& endpoint, const std::string& post_data);
    
    public:
        ChatGPT(const std::string& api_key);

        std::string get_prompt();
    
        std::string get_prompt(const std::string& input_text);
    
};


#endif /* ChatGPT_h */
