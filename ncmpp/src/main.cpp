#include "ncmlib/ncmdump.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "cmdline.h"
#include "pool.h"

using namespace std;

std::mutex logMtx;
template <typename... Args> void log(Args &&...args) {
    std::lock_guard<std::mutex> _a(logMtx);
    ((std::cout << args), ...);
    std::cout << endl;
}

std::unordered_set<std::string> unlocked_files;
int totalPieces = 0;

int main(int argc, char* argv[])
{
    cmdline::parser cmd;
    cmd.set_program_name("ncmpp");
    cmd.add<unsigned int>("threads", 't',
        "Max count of unlock threads.",
        false, std::thread::hardware_concurrency()
    );
    cmd.add("showtime", 's',
        "Shows how long it took to unlock everything."
    );

    bool success = cmd.parse(argc, argv);
    if (!success)
    {
        return 1;
    }

    unsigned int c_thread = cmd.get<unsigned int>("threads");
    bool s_time = cmd.exist("showtime");

    log("Start with ", c_thread," threads.\n");

    ::system("chcp>nul 2>nul 65001");

    if (!filesystem::exists("unlock"))
    {
        filesystem::create_directory("unlock");
    }
    else
    {
        for (auto& i : filesystem::directory_iterator("./unlock"))
        {
            if (i.is_directory())
            {
                continue;
            }
            unlocked_files.emplace(i.path().stem().u8string());
        }
    }

    auto start = std::chrono::steady_clock::now();

    {
        thread_pool pool(c_thread);
        for (auto& i : filesystem::directory_iterator("."))
        {
            if (i.is_directory() ||
                i.path().extension() != ".ncm"
            )  {
                continue;
            }
            pool.enqueue(
                [](const filesystem::path& path)
                {
                    if (unlocked_files.find(path.stem().u8string()) ==
                        unlocked_files.end()) {
                        ncm::ncmDump(path.u8string(), "unlock");
                        log("\033[36mUnlocked:\t", path.filename().u8string(), "\033[0m");
                        ++totalPieces;
                    }
                    else
                    {
                        log("\033[33mSkipped:\t", path.filename().u8string(), "\033[0m");
                    }
                },
                i.path());
        }
    }

    auto end = std::chrono::steady_clock::now();
    log("\n\033[32mFinished.\033[0m");
    log("Unlocked ", totalPieces, " pieces of music.");

    if (s_time)
    {
        log("Time elapsed: ",
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count() / 1000.0,
            "s");
    }

    ::system("pause");
    return 0;
}
