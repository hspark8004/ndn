#include <iostream>

#include <stdint.h>
#include <unistd.h>
#include <string.h>

class Content {
    private : 
        // contentLength : size of Data
        // content : data
        uint8_t contentLength;
        unsigned char* content;

    public : 
        Content() {

        }
        void setContentLength(uint8_t length) {
            this->contentLength = length;
        }
        void setContent(char* data) {
            uint8_t length = strlen(data);
            
            if(content == NULL) {
                content = new unsigned char[length];
            } else {
                delete content;
                content = new unsigned char[length];
            }
            memcpy(data, content, length);
            contentLength = length;
        }
};
