//
//  ChatGPT.cpp
//  writing prompter
//
//  Created by Jake Baldwin on 3/27/23.
//

#include <stdio.h>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include "../include/ChatGPT.h"

ChatGPT::ChatGPT(const std::string& api_key) : api_key_(api_key) {}

std::string ChatGPT::get_prompt(const std::string& input_text) {
    std::string input = "Create a single writing prompt that is random and unique, based on these words of inspiration: ";
    input += input_text;
    std::string endpoint = "engines/text-davinci-003/completions";
    std::string post_data = "{\"prompt\": \"" + input + "\", \"max_tokens\": 50}";
    return perform_request(endpoint, post_data);
}

std::string ChatGPT::get_prompt() {
    std::string input = "Create a single writing prompt that is random and unique:";
    std::string endpoint = "engines/text-davinci-003/completions";
    std::string post_data = "{\"prompt\": \"" + input + "\", \"max_tokens\": 50}";
    return perform_request(endpoint, post_data);
}

size_t ChatGPT::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* response = static_cast<std::string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string get_text_from_json(const std::string& json) {
    std::string text;
    
    size_t text_start = json.find("\"text\":\"") + 12;
    size_t text_end = json.find("\",\"index\":");
    text = json.substr(text_start, text_end - text_start);
    return text;
}


std::string ChatGPT::perform_request(const std::string& endpoint, const std::string& post_data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize cURL");
    }

    std::string url = api_url_ + endpoint;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Content-Type: application/json"));
    headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key_).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data.length());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        throw std::runtime_error("Failed to perform HTTP request: " + std::string(curl_easy_strerror(result)));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    
    return get_text_from_json(response_string);
}
