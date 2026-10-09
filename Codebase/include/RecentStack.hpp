#ifndef RECENTSTACK_HPP
#define RECENTSTACK_HPP

#include <string>
#include <vector>
#include <deque>

class RecentStack {
private:
    std::deque<std::string> stack;
    size_t maxSize;

public:
    explicit RecentStack(size_t max = 10) : maxSize(max) {}

    void push(const std::string& siteName) {
        stack.push_front(siteName);
        if (stack.size() > maxSize) {
            stack.pop_back();
        }
    }

    std::vector<std::string> peekTopN(size_t n) const {
        size_t count = std::min(n, stack.size());
        return std::vector<std::string>(stack.begin(), stack.begin() + static_cast<long>(count));
    }

    bool empty() const { return stack.empty(); }
    size_t size() const { return stack.size(); }
};

#endif
