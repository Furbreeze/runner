# Runner
Just playing around with raylib as a gui for apps instead of games. Ended up being a simple GUI for running scripts. Could see uses for simplifying build processes, stream setups, etc.

## Requirements
- gcc
- [raylib](https://github.com/raysan5/raylib)


## Building
1. Place your copy of libraylib.a in the lib folder. 
2. Set the DIR and SCRIPTS_DIR values in the main.c file. 
3. Run the build.sh script in the project's root directory.

## Configuring
Scripts are placed in the scripts folder, defined in main.c. Buttons are dynamically populated by the values in config.ini. The format is below.

```
<button_text> <script_file_name>
<button_text> <script_file_name>
```
