# Skateboard Workshop: Pong! 🎮

Welcome to the **Skateboard Workshop: Pong!** — a short, self-paced game development workshop where you’ll explore an Entity Component System (ECS)-based game engine and enhance a basic Pong game with polish and interactivity.

## 📌 Overview
This workshop is designed to be asynchronous and should take around **10–15 minutes**. You’ll get hands-on with the Skateboard engine and learn how systems, components, and entities come together in an ECS framework. No prior game development experience is needed, and staff are on hand if you need help.

---

## 🚀 Getting Started

### 🔗 Clone the Starter Code
Download or clone the codebase 


### 🛠 Prerequisites
- **Visual Studio 2022**
- Windows environment

### ▶️ Run the Application
1. Open `Skateboard.sln` in Visual Studio 2022.
2. Press the green "Start" button (or press `F5`) to launch.
3. When started, you'll see two paddles and a ball at rest in the center — press **Space Bar** to launch it.

---

## 🔍 Exploring the Code

### 📁 File: `TutorialScene.cpp`
This file contains the main logic for entity creation and system setup.

- **Entities** represent game objects (paddles, ball, walls).
- **Components** hold data.
- **Systems** define logic (e.g., movement, collisions, effects).

### 🔧 Initialization
- Renderer and Registry set up first.
- Physics World and Camera entities come next.
- Paddle entities created using a boolean to determine left/right.
- Top and bottom wall entities follow.

🔎 Tip: You can hold `Ctrl` and click on any function to view its definition. Use the **Back** arrow in Visual Studio to return.

---

## 🕹 Implementing Basic Movement

- The `OnUpdate()` function is called every frame (~60 FPS).
- Uncomment the `PaddleSystem` line to allow paddle control using:
  - `W` / `S` keys (left paddle)
  - `Up` / `Down` arrows (right paddle)

Press `F5` to run and test the functionality.

---

## ✨ Adding Enhancements

1. **Recharge System**
   - Uncomment it.
   - Press `D` (Right) or `Left Arrow` (Left) to recharge paddles.
   - Fully charged paddles increase ball speed on contact.

2. **Visual Effects**
   - Uncomment `EffectsSystem`.
   - Observe the visual polish.

3. **Ball Animation**
   - Uncomment `BallEffects` for dynamic ball visuals.

4. **Audio Feedback**
   - Uncomment `AudioSystem` to add sound effects.

---

## 🧪 Workshop Tasks

Try these fun enhancements:

1. 🎨 Make the **Left Paddle Green** and **Right Paddle Blue**.
2. 🏐 On pressing `B`, **spawn an additional ball**.
3. 🔊 Play the **"WallHit"** audio clip when the ball hits a wall.
   - Tip: You can add this to `AudioSystem` or create a new `WallSystem`.

---

## 🙋 Need Help?
Don’t hesitate to reach out to a staff member nearby. This is a learning experience — no pressure to finish all tasks!

---

Made with ❤️ by Abertay University’s FDIB Team
