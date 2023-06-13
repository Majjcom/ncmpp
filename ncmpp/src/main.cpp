#include "ncmlib/ncmdump.h"
#include <filesystem>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

std::mutex logMtx;
template <typename... Args> void log(Args &&...args) {
    std::lock_guard<std::mutex> _a(logMtx);
    ((std::cout << args), ...);
    std::cout << endl;
}

class thread_pool {

    std::vector<std::thread> threads;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
    bool stop = false;

public:
    thread_pool() {
        auto n = std::thread::hardware_concurrency();
        if (n == 0) {
            n = 2;
        }
        for (auto i = 0u; i < n; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) {
                            return;
                        }
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~thread_pool() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for (auto &i : threads) {
            i.join();
        }
    }

    template <typename F, typename... Args> auto enqueue(F &&f, Args &&...args) {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto ret = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.emplace([task] { (*task)(); });
        }
        cv.notify_one();
        return ret;
    }
};

std::unordered_set<std::string> unlocked_files;

int main(int argc, char *argv[]) {
    ::system("chcp>nul 2>nul 65001");

    if (!filesystem::exists("unlock")) {
        filesystem::create_directory("unlock");
    } else {
        for (auto &i : filesystem::directory_iterator("./unlock")) {
            if (i.is_directory()) {
                continue;
            }
            unlocked_files.emplace(i.path().stem().u8string());
        }
    }

    {
        thread_pool pool;
        for (auto &i : filesystem::directory_iterator(".")) {
            if (i.is_directory()) {
                continue;
            }
            if (i.path().extension() != ".ncm") {
                continue;
            }
            pool.enqueue(
                    [](const filesystem::path &path) {
                        if (unlocked_files.find(path.stem().u8string()) ==
                                unlocked_files.end()) {
                            ncm::ncmDump(path.u8string(), "unlock");
                            log("Unlocked: ", path.u8string());
                        } else {
                            log("Skipped: ", path.u8string());
                        }
                    },
                    i.path());
        }
    }

    log("\nFinished.");

    system("pause");
    return 0;
}
