# cs334-game
## Build process
1. `cmake -B build` for a debug build or `cmake -B build -DCMAKE_BUILD_TYPE=Release` to enable compiler optimizations
2. `cmake --build build`
3. Run with `./build/Game <framerate (default 60)>` (or adjust the path for your OS and compiler)
   - Example: `./build/Game 60`

Note that the program can take up to two minutes to start.

## Keybinds
- `W/A/S/D`: move
- `Left Control`: move down
- `Left Shift`: move faster (momentary, not toggle)
- `Escape`: quit the game
- `F`: toggle distance fog
- `G`: toggle screen-space godrays
- `N`: toggle no-clip (disables collisions and faux-gravity)
- `P`: toggle Picture-in-Picture view of occlusion render pass

Water texture [resources/Water.jpg](https://commons.wikimedia.org/wiki/File:ISS006-E-37356_-_View_of_Pacific_Ocean.jpg): public domain NASA image. not protected by copyright
