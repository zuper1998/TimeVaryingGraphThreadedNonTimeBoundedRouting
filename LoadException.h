//
// Created by Narcano on 6/10/2022.
//

#ifndef TVGGRAPHCALC_LOADEXCEPTION_H
#define TVGGRAPHCALC_LOADEXCEPTION_H

#include <string>
#include <utility>


class LoadException: public std::exception {
private:
    std::string message_;
public:
    explicit LoadException(std::string  message);
    [[nodiscard]] const char* what() const noexcept override {
        return message_.c_str();
    }
};



#endif //TVGGRAPHCALC_LOADEXCEPTION_H
