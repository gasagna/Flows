#pragma once

#include <string>
#include <fstream>
#include <vector>

// use filesystem
// namespace fs = std::filesystem;

namespace Flows {

// ////////////////////////////////////////////////////////////////
// // Disk STORAGE
// template <typename Y>
// class DiskStorage {
// private:
//     std::filesystem::path _casedir;

// public:
//     DiskStorage(std::filesystem::path casedir)
//         : _casedir(casedir) {
//             if (!fs::exists(_casedir))
//                 throw std::invalid_argument("invalid directory");
//         }

//     template <typename X>
//     void push_back(double t, X&& x) {
//         // construct filename 
//         std::string filename(16, '0');
//         sprintf(&filename[0], "%14.8f", t);

//         // construct full name
//         std::filesystem::path fullname = _casedir / path(filename);

//         // objects of type x should obey this interface
//         write(fullname, x);
//     }
// };

////////////////////////////////////////////////////////////////
// RAM STORAGE
template <typename Y>
class RAMStorage {
private:
    std::vector<double> _ts;
    std::vector<Y>      _xs;

public:
    template <typename X>
    void push_back(double t, X&& x) {
        _ts.push_back(t);
        _xs.push_back(x);
    }

    auto& times() { return _ts; }
    auto& samples() { return _xs; }
};
}