#ifndef LOG_HPP
#define LOG_HPP

class Log {

    public:
        bool On;
        bool writeResult;
        Log();
        ~Log();
        void setLogPath(const char *logPath);
        void write(const char *format, ...);
    private:
        char *__logPath;
        size_t len;
};

#endif