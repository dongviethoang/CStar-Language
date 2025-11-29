import sys
import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "1"

try:
    import pygame
except ModuleNotFoundError:
    cmd = "pip install pygame"
    os.system(cmd)
    import pygame

def play_sound_file(path):
    try:
        sound = pygame.mixer.Sound(path)
        chan = sound.play()
        if chan is None:
            print(f"Failed to play: {path}")
            return 1
        print(f"Playing sound: {path}")
        while chan.get_busy():
            pygame.time.delay(50)
        return 0
    except Exception as e:
        print(f"Error playing sound '{path}': {e}")
        return 1

def play_music_file(path):
    try:
        pygame.mixer.music.load(path)
        pygame.mixer.music.play()
        print(f"Playing music: {path}")
        while pygame.mixer.music.get_busy():
            pygame.time.delay(100)
        return 0
    except Exception as e:
        print(f"Error playing music '{path}': {e}")
        return 1

def main():
    try:
        pygame.mixer.init()
    except pygame.error as e:
        print(f"Error initializing mixer: {e}")
        return 1

    if len(sys.argv) < 2:
        print("Usage: sound_play.py [--music <file>] <file1> [file2 ...]")
        return 1

    rc = 0
    i = 1
    while i < len(sys.argv):
        arg = sys.argv[i]
        if arg == "--music":
            # consume next argument as music filename
            if i + 1 >= len(sys.argv):
                print("Error: --music requires a filename argument")
                rc = 1
                break
            fname = sys.argv[i + 1]
            if not os.path.exists(fname):
                print(f"File not found: {fname}")
                rc = 1
            else:
                rc |= play_music_file(fname)
            i += 2
            continue

        # normal sound file
        fname = arg
        if not os.path.exists(fname):
            print(f"File not found: {fname}")
            rc = 1
            i += 1
            continue

        rc |= play_sound_file(fname)
        i += 1

    try:
        pygame.mixer.quit()
    except Exception:
        pass

    return 1 if rc != 0 else 0

if __name__ == "__main__":
    sys.exit(main())