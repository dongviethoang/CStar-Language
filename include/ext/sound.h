#pragma once

#include <cstdlib>
#include <string>
#include <string_view>
#include <windows.h>
#include <iostream>

inline int call_sound_player(std::string_view sound_file) noexcept {
    std::string command;

    // Open the file
    command = "open \"" + std::string(sound_file) + "\" type mpegvideo alias music";
    mciSendStringA(command.c_str(), NULL, 0, 0);

    // Play it
    mciSendStringA("play music", NULL, 0, 0);

    std::cout << "Playing: " << std::string(sound_file) << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    // Close the device
    mciSendStringA("close music", NULL, 0, 0);
}

inline int call_music_player(std::string_view music_file) noexcept {
    // Play some music, eh?
    std::string cmd = "python \"D:/CStar/sound_play.py\" --music \"";
    cmd += std::string(music_file);
    cmd += "\"";
    return std::system(cmd.c_str());
}

#define play_sound(file) call_sound_player(file)
#define play_music(file) call_music_player(file)