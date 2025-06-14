# The Last Warden

**A 2D survival game built using Allegro.**  
In a hostile world overrun by enemies, you are the last surviving warden. Build defenses, manage resources, and hold your ground as increasingly powerful enemy waves attack from all directions. Strategically place turrets, walls, and traps to survive as long as possible.

---

## 👨‍💻 Detailed Division of Labor

| Module / Feature Area    | Alex (LouiseDavise) Contributions                                                                                                                                                                                                                                                                           | John (Marty) Contributions                                                                                                                                                                                                             |
|--------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **🧍 Player Mechanics**   | - Implemented player movement using keyboard (WASD & arrows)<br>- Designed camera to follow player within map bounds<br>- Added animation logic for walking/running<br>- Created new heroes: **Spearman**, **Archer**, **Mage**<br>- Developed throwable and retrievable spear system<br>- Fixed bugs in spear return logic and hero weapon aiming | - Refined player animation states<br>- Added knockback and dying animations with lose scene integration<br>- Created weapon base/derived class for spear<br>- Helped debug player/map resolution and movement interaction issues         |
| **🧠 AI & Enemies**       | - Developed wave-spawning logic and file-based wave reading<br>- Implemented enemy pathfinding system<br>- Fixed movement bugs causing enemies to overshoot or freeze                                                                                                                                        | - Fully implemented **GreenSlime** enemy including logic and animations<br>- Designed visual effects for enemy hits and death<br>- Implemented enemy collision handling and attack cooldown logic<br>- Developed enemy variation system |
| **🛠 Build System**       | - Enabled dynamic structure spawning<br>- Implemented SmashBone structure removal logic<br>- Rebuilt wall and turret placement system                                                                                                                                                                      | - Implemented tower/structure logic and inheritance<br>- Built wall HP and damage system<br>- Fixed tile occupancy and building placement issues                                               |
| **🧱 Map & Environment**  | - Designed forest, river, and bridge tilesets<br>- Fixed camera bounds and map scrolling limits<br>- Resolved placement and collision bugs in map layouts                                                                                                                                                   | - Created large 200×200 map draft<br>- Optimized tile culling for performance<br>- Implemented night mode for biome variation                                                                 |
| **🎮 UI / UX**           | - Created Welcome, SelectHero, and SelectCompanion scenes<br>- Implemented UI buttons, hover logic, and scene transitions<br>- Designed companion profile visuals<br>- Created score display and leaderboard UI                                                                                              | - Designed and implemented side panel UI and layout<br>- Added resource and structural menus<br>- Polished structural panel and fade/transition animations                                     |
| **💬 Companion System**  | - Implemented all 3 companion skills: Wisp (swap), Mochi (heal), Zuko (shockwave)<br>- Integrated cooldown visuals into side panel<br>- Developed companion cooldown bar logic and fixed reset bugs<br>- Added movement and profile UI for companions                                                                                                         | - Designed companion vision logic<br>-  Helped resolve UI alignment and positioning bugs                                                       |
| **🧪 Gameplay Systems**  | - Built game end logic (win/lose states)<br>- Created pause/play logic and UI<br>- Developed score calculation and display<br>- Added fade effects for smoother scene transitions                                                                                                                            | - Implemented wave FIFO queue system<br>- Fixed bugs in death animations and game reset<br>- Adjusted balance and timing for resources and systems                                              |
| **🎨 Visuals & VFX**     | - Designed spear throw/return animation<br>- Resized hero and weapon assets<br>- Polished general UI and in-game visual consistency                                                                                                                                                                       | - Created clash, toxic, and hit visual effects<br>- Designed bonfire lighting logic<br>- Added slime dying animations                                                                         |
| **🔊 Audio System**      | - Integrated audio for game scenes and actions<br>- Fixed playback and overlapping audio bugs                                                                                                                                                                                                               | - Helped debug and merge audio transitions                                                                                                               |
| **🧾 Project & GitHub**  | - Set up initial GitHub repo and project structure<br>- Resolved merge conflicts and scene refactors                                                                                                                                                                      | - Merged branches and organized folders<br>- Helped with bugfix commits and transitions                                                                 |


## 🎮 Game Features

### 🧍 Player
- Free movement using **WASD** or arrow keys
- Camera follows the player
- Spawns at the center of the map

### ⚔️ Combat & Weapons
- **Spear** (throwable & retrievable)
- **Ranged weapons** (staffs, bows, guns)
- **Landmines**, **spike walls**, and other damaging structures

### 🧱 Buildables
- **Turrets** that auto-attack within a radius
- **Walls** with limited HP
- **Mud leak** to slow enemies
- **SmashBone** to remove structures

### 👾 Enemies
- Spawn from map edges
- Scale in strength and type over time

### 🌎 World Features
- **Day-night cycle** (reduced visibility at night)
- **Biomes & tiles** (e.g., mud, water)
- **Mini-map** (optional)
- **Flag system** for territory control and income generation

---

## 📅 Development Schedule

| Date        | Goal                                                                 |
|-------------|----------------------------------------------------------------------|
| June 2      | Project setup, basic map, player movement & camera                   |
| June 3–5    | Enemy spawn, AI, death logic, currency system, basic buildables      |
| June 6–9    | Advanced buildables, biomes, scaling difficulty                      |
| June 10     | UI setup (score, health, build menu), optional mini-map              |
| June 11     | Territory/flag system, upgrade mechanics                             |
| June 12–14  | Polishing, SFX, balance testing, bug fixes, README & final packaging |
| June 15     | Project Demo                                                         |
---

## 👨‍💻 Division of Labor

Detailed contributions (features owned, modules implemented) will be documented in the final `README.md` update upon project completion. Each team member will be credited based on actual implementation and work logs.

---

## 🛠 Technologies Used

- **Language**: C/C++
- **Library**: [Allegro 5](https://liballeg.org/)
- **Platform**: Desktop (Windows/Linux)

---

## 🚧 Disclaimer

This is a student project developed as part of a course assignment. Game features and implementation details may change during development based on time constraints, feedback, and feasibility.

---

## 📁 How to Build and Run

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/The-Last-Warden.git
   cd The-Last-Warden

© 2025 Louise Davise & Amartyananda Chang. All rights reserved.
