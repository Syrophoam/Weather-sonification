#include <iostream>
#include <string>
#include <array>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <format>
#include <map>

#include <curl/curl.h>

#include <fstream>
#include <nlohmann/json.hpp>

#define dayInSeconds 86400;

// if have time make informative promo using imac

size_t dataSize = 0;
using json = nlohmann::json;

// really confusing function i copied off the internet
size_t CURLWriteFunction(void *ptr, size_t size, size_t nmemb, void* userData){
    
    char** stringToWrite = (char**)userData;
    const char* input = (const char*)ptr;
    if(nmemb==0) return 0;
    if (!*stringToWrite) {
        *stringToWrite = static_cast<char*>(malloc(nmemb+1));
    }
    else
        *stringToWrite = static_cast<char*>(realloc(*stringToWrite, size+nmemb+1));
        
    memcpy(*stringToWrite+dataSize, input, nmemb);
    dataSize += nmemb;
    (*stringToWrite)[dataSize] = '\0';
    return nmemb;
}

int main() {
    CURL *handle = curl_easy_init();
    struct curl_slist *headers = NULL;
    
    json jsonReq = {};
    std::map pos = std::map<std::string,float>{
        {"lon",174.7842} , {"lat",-37.7935}
    };
    
    json::array_t positionArray = {pos};
    jsonReq["points"] = positionArray;
    
    json::array_t variables = {"wave.height","air.visibility"}; // variable list
    jsonReq["variables"] = variables;
    
    const auto now = std::chrono::system_clock::now();
    
    std::string currentTime = std::format("{:%FT%H:%M:00Z}", now);
    
    std::map time = std::map<std::string, std::string>{
        {"from",currentTime}, {"interval","1h"}
    };
    
    jsonReq["time"] = time;
    
    int repeats = 0;
    std::cout << "repeats" << std::endl;
    std::cin >> repeats;
    jsonReq["time"]["repeat"] = repeats; // repeats
    
    std::string jsonRequestString = jsonReq.dump();
    
    // where the response from the api will go
    char* data=0;
    
    // my api key
    std::string key = "S14jxYHPDoXhtPwrvYRm9m";

    if (handle) {
        CURLcode res;
        // get CURL to look at metservice
        curl_easy_setopt(handle, CURLOPT_URL, "https://forecast-v2.metoceanapi.com/point/time");
        // giving CURL the foncusing function
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &CURLWriteFunction);

        //timeval now;
        //int time = gettimeofday(&now, NULL);
        
        std::string key_header = "x-api-key: " + key;
        headers = curl_slist_append(headers, key_header.c_str());
        // telling CURL to use JSON
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        // sending the top two things to metservice
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, jsonRequestString.c_str());

        // this actually does everything above
        res = curl_easy_perform(handle);
        
        // gives the CPU and memory usage back to the computer
        curl_slist_free_all(headers);
        curl_easy_cleanup(handle);
    }
    // prints the weather data to terminal
    printf("Page data:\n\n%s\n", data);
    
    json jsonResponse = json::parse(data);
    
    json testParse = jsonResponse["variables"];
    json waveHeight = testParse["wave.height"];
    json waveHeightData = waveHeight["data"];
    
    waveHeightData[0];
    
    
    
    
    // gives memory back
    free(data);
    
    return 0;
}
