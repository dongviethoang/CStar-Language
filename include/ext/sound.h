#pragma once

#include <cstdlib>
#include <string>
#include <string_view>
#include <windows.h>

inline int call_sound_player(std::string_view sound_file) noexcept {
    string command;

    // Open the file
    command = "open \"" + file + "\" type mpegvideo alias music";
    mciSendStringA(command.c_str(), NULL, 0, 0);

    // Play it
    mciSendStringA("play music", NULL, 0, 0);

    cout << "Playing: " << file << endl;
    cout << "Press Enter to exit..." << endl;
    cin.get();

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