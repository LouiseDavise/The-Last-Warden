# 🛡️ The Last Warden

**A 2D survival game built using Allegro.**  
In a hostile world overrun by enemies, you are the last surviving warden. Build defenses, manage resources, and hold your ground as increasingly powerful enemy waves attack from all directions. Strategically place turrets, walls, and traps to survive as long as possible.

---

## 📝 Description

**The Last Warden** is a handcrafted 2D open-world survival game built in C++ using the Allegro 5 graphics library.  
You play as the last protector of a ruined realm, fighting off endless waves of monsters while building your defenses and managing limited resources.

⚔️ **Combine fast-paced combat, real-time building, and companion strategy** in a dynamic battlefield where every second counts. Place turrets, build walls, throw your spear, and survive the night—if you can.

Whether you're fending off slimes or facing elite orcs and vampires, **The Last Warden** challenges you to think, fight, and adapt to survive longer than ever before.

---

### 🧩 Highlights

- 🎮 Smooth WASD-based movement with camera follow  
- 🗡️ Throwable and retrievable spear weapon  
- 🧱 Real-time building: turrets, walls, traps, and utility tools  
- 🧠 Smart enemy with pathfinding and wave escalation  
- 💥 Companion skills with cooldowns (swap, heal, knockback)  
- 🌙 Atmospheric day-night cycle with limited night vision  
- 🔥 Dynamic tile-based world with biomes and build zones

---

## 🎥 Gameplay Video

Watch our full gameplay showcase here:  
📺 **[YouTube - The Last Warden Gameplay Demo](https://youtu.be/nb0SRaiFAUs)**

See the full system in action, including:
- Player and companion skills  
- Wave-based enemy spawns  
- Buildable defenses and walls  
- Night mode with limited visibility  
- Beautiful tile-based world design  
- Polished UI and SFX

---

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
- Types include: `GreenSlime`, `ToxicSlime`, `LavaSlime`, `Orc`, `HighOrc`, `Vampire`

### 🧠 Companion System
- Companion support with unique skills:
  - **Wisp**: Teleport-swap with player  
  - **Mochi**: Healing field  
  - **Zuko**: Shockwave knockback  
- Cooldown bars and UI indicators

### 🌎 World Features
- **Day-night cycle** (reduced visibility at night)  
- **Biomes & tiles** (e.g., mud, water)  
- **Mini-map** (optional)  
- **Flag system** for territory control and income

---

## 🛠 Technologies Used

- **Language**: C/C++  
- **Library**: [Allegro 5](https://liballeg.org/)  
- **Platform**: Desktop (Windows/Linux)

---

## 📁 How to Build and Run

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/The-Last-Warden.git
   cd The-Last-Warden
