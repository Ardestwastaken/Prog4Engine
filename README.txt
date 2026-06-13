Engine: Minigin (SDL3-based 2D engine)
Game: Q*bert clone

-- Architecture --

The engine uses a GameObject/Component model. GameObjects are owned by Scenes via unique_ptr and form a parent/child hierarchy for world-space transform propagation. Components override Update, FixedUpdate, and Render as needed.

Scene management is centralised in a SceneManager singleton. LoadScene defers the actual swap to the next frame so nothing is destroyed mid-Update. An optional freeze delay keeps the current scene rendering for N seconds before the swap happens. A pending add/remove queue on Scene means Add() and Remove() are safe to call from inside Update without invalidating iterators.

Input uses a Command pattern. Keyboard keys and XInput controller buttons are bound to Command objects via BindKeyboardCommand / BindControllerCommand, each returning an int ID for targeted unbinding. UnbindAllCommands is called on every scene load to clear stale pointers.

Events use an Observer pattern with compile-time hashed string IDs (EventId via sdbm_hash). Subjects hold raw IObserver pointers — no ownership. The IsUnloading flag on SceneManager prevents observer callbacks from firing into already-destroyed subjects during teardown.

Sound runs on a background thread in SDLSoundSystem. The main thread enqueues sound events; the audio thread dequeues and plays them. This avoids blocking the game loop. A SetMuted flag stops enqueuing without stopping the thread. A LoggingSoundSystem decorator wraps it in debug builds.

Textures are cached by path in ResourceManager. TextureAtlasComponent slices a sprite sheet into a col/row grid and draws one frame at a time. SDL_SCALEMODE_NEAREST is set on pixel art textures to prevent blurring on upscale.

-- Game --

The grid stores per-tile state integers. ColorMode::Normal advances tiles up to a requiredState cap. ColorMode::AlternatingAfterFirst lets tiles toggle between state 1 and 2 after the first step, used in level 3. The grid owns its own celebration state machine: on completion it flashes all tiles and fires a callback when done, during which ColorTile is a no-op and Coily is frozen.

The player and Coily both use state machines. The player has Idle, Jumping, Landing, and Dead states. Input buffering stores one directional input with a 0.2s TTL during Jumping or Landing and consumes it immediately on entering Idle, giving the movement a responsive feel. Coily has an egg state that randomly bounces to the bottom row, a snake state that greedily chases the player by Manhattan distance (only valid tiles, never voluntarily steps off the grid), and a player-controlled state used in Versus mode.

Lives and score are stored in a GameSession singleton so they persist across scene transitions within the same run and reset when returning to the main menu.

The HUD and GameOver are observer components. HUDComponent spawns heart GameObjects into the scene dynamically and removes them from the bottom up as lives are lost. GameOverComponent activates on zero lives, pauses the scene manager, and rebinds input to return-to-menu only.

Pause works by setting a paused flag on SceneManager which skips Update and FixedUpdate but keeps Render running. The Escape key and controller Y button are swapped between QuitGame and PauseToMenu while paused by tracking their binding IDs and rebinding on each toggle.

Three game modes share the same level pipeline. Solo: one player, AI Coily. Co-op: two players spawning at opposite bottom corners, Coily targets P1, either player completing all tiles advances the round. Versus: P1 is Q*bert, P2 directly controls Coily via keyboard or controller.