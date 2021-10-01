#ifndef CONFIG_H
#define CONFIG_H

#define INITIAL_GAME_SIZE 50
#define STRING_SIZE 3000
#define COUNT_SIZE 7

#define DATABASE_DIRECTORY "/revision/database"
#define SCORE_PATH "/revision/score.txt"
#define LOG_PATH "/revision/slcard_log.txt"

#define WINDOW_TITLE "slcard"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_PADDING 15
#define FONT_SIZE 32

/* Linux user should define their own path to ttf file. */
/* #define FONT_PATH "/usr/share/fonts/ttf/PingFang.ttc" */

/* For macOS, using PingFang for out-of-the-box Chinese support. */
#define FONT_PATH "/System/Library/Fonts/PingFang.ttc"

#endif /* CONFIG_H */
