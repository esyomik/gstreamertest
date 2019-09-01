#ifndef RESULT_21_11_20_1_9_19_h
#define RESULT_21_11_20_1_9_19_h

#include <string>


enum RetCode
{
    Success = 0,
    FileNotFound,
    BadFormat,
    Error
};


template<class RetType> class Result
{
public:
    Result(RetType data) : ret_(data), code_(RetCode::Success) { ; }
    Result(RetType data, RetCode retCode) : ret_(data), code_(retCode) { ; }
    Result(RetType data, RetCode retCode, const char* text)
        : ret_(data), code_(retCode), description_(text) { ; }

    RetType getData() const { return ret_; }
    RetCode getCode() const { return code_; }
    bool success() const { return code_ == RetCode::Success; }
    const std::string& description() const { return description_; }

private:
    RetType ret_;
    RetCode code_;
    std::string description_;

};

#endif // RETCODE_21_11_20_1_9_19_h
