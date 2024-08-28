#include <iostream>
#include <string>
#include <array>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "JSONbullShit.hpp"

size_t dataSize = 0;

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
    
    char* data=0;
    

    std::string key = "S14jxYHPDoXhtPwrvYRm9m";

    std::string begin = "{";
    std::string JSON;
    
    JSON += begin;
    
    int lonLat[] = {2,5}; // make float
    std::string position = parse(
                                "\"points\": [{ \"lon\": %i, \"lat\": %i }],", lonLat);
    JSON += position;
    
    // variables, put comma after each variable except last
    std::string variables = " \"variables\": [ ";
    variables += " \"wave.height\", ";
    variables += " \"wave.height.max\" ";
    variables += "],";
    
    JSON += variables;

    // time in ISO 8601 UTC 
//(2024-08-26: Year, month, and day (in YYYY-MM-DD format).
//T: Separator between date and time.
//00:00:00: Hour, minute, and second (in HH:MM:SS format).
//Z: Indicates Coordinated Universal Time (UTC).)
    
    std::string time = "\"time\": {\"from\": ";
    time += "\"2024-08-26T00:00:00Z\"";
    
    JSON += time;
    
    int intervalInHours = 5;
    std::string interval = ",\"interval\": \"";
    interval += std::to_string(intervalInHours);
    interval += "h\"";

    JSON += interval;  // replace /\ and \/ with the parse function
    
    
    int repeats = 4;
    std::string reps = ",\"repeat\":  ";
    reps += std::to_string(repeats);
    
    JSON += reps;
    JSON += interval;
    
    std::string end = "}}\0";
    
    JSON += end;

    if (handle) {
        CURLcode res;
        curl_easy_setopt(handle, CURLOPT_URL, "https://forecast-v2.metoceanapi.com/point/time");
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &CURLWriteFunction);
        
        std::string key_header = "x-api-key: " + key;
        headers = curl_slist_append(headers, key_header.c_str());
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, JSON.c_str());

        res = curl_easy_perform(handle);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(handle);
    }
    
    printf("Page data:\n\n%s\n", data);
    
    char test = data[3];
    
    free(data);
    return 0;
}
