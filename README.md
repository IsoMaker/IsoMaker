# IsoMaker

IsoMaker is a game engine for creating isometric games. It is written in C++ and uses the Raylib for rendering.

---

## C++ Coding Style

The foundation of the coding style applied to this project is a C++ adapatation of Epitech's C coding style, here are the main rules:

### Naming conventions

- All **subdirectories** and **files** under the `/src` and `/includes` directories should have one word names when possible. These, as well as **classes**, should follow the [UpperCamelCase naming convention](https://www.techtarget.com/whatis/definition/CamelCase), which dictates every compound word in a name is capitalized.
- **Interfaces** and **abstract classes** must have names starting with a capital I and A respectively (e.g., `IHandler`, `AHandler`).
- **Namespaces**, **functions** and **variables** should follow the [lowerCamelCase naming convention](https://www.techtarget.com/whatis/definition/CamelCase), which dictates the first compound word should start with a lowercase, and every compound word after that should be capitalized (e.g., `input::KeyboardHandler::startLoop(bool status)`).


### Project folder architecture

- `.hpp` files containing **interfaces**, **abstract class implementations**, **project specific variable types** and other such things should be found in `/includes/<subdirectory>` (e.g., `IHandler.hpp`, `AHandler.hpp` and `Types.hpp` in `/includes/Input`).
- `.cpp` source files required for building the project should be found in `/src/<subdirectory>`, along with their respective `.hpp` files. (e.g., `Keyboard.cpp` and `Keyboard.hpp` in `/src/Input`).

### Files

- All files should start with the standard Epitech header provided by the official Epitech header extension on VSCode.
- Contents of `.hpp` files should be preceded by `#pragma once`.
- All files should end with `\n` (newline).

### Classes

- A class' **access specifiers** should be written in the following order: `public`, `protected`, `private`.
- **Function prototypes** should be declared before **variables**.

---

## Commit Message Standard

All commits contributing to this project must be accompanied by a commit message that adheres to the following format, based on the [Conventional Commits standard](https://www.conventionalcommits.org/en/v1.0.0/):  

```
<type>(<scope>): <description>
```

### Structure

- **`type`**: Describes the purpose of the commit. Use one of the following predefined types:
  - **feat**: A new feature.
  - **fix**: A bug fix.
  - **refacto**: Code changes that neither fix a bug nor add a feature.
  - **doc**: Documentation changes (e.g., README updates).
  - **style**: Code style changes (e.g., formatting, missing semicolons) that do not affect functionality.
  - **test**: Adding or updating tests.
  - **build**: Maintenance tasks (e.g., build scripts, dependency updates).

- **`scope`**: Identifies the specific area of the codebase affected. Keep it concise, such as:
  - A module name (e.g., `graphics`, `input handling`).
  - A component name (e.g., `button`, `keyboard handler`).
  - Use `*` for a global change affecting multiple areas.

- **`description`**: A brief, imperative summary of the change.
  - Start with a verb in the present tense (e.g., `add`, `fix`, `update`).
  - Don't capitalize the first letter or end with a period.

#### Examples

1. Adding a new feature:
   ```
   feat(input handling): add support for joypad controls
   ```

2. Fixing a bug:
   ```
   fix(graphics): resolve crash on object rendering
   ```

3. Updating documentation:
   ```
   doc(readme): improve commit convention examples
   ```

4. Refactoring code:
   ```
   refacto(engine): simplify event loop logic
   ```

5. Updating tests:
   ```
   test(graphics): add tests for sprite rendering
   ```

### Why Use This Format?

- **Clarity**: Makes it easier to understand what a commit does at a glance.
- **Automation**: Supports tools for generating changelogs and release notes.
- **Consistency**: Encourages contributors to document changes uniformly.

---

## How to add a feature and Pull Request 101

#### 1. Access the repository's [Github Project](https://github.com/AetherStollMetz/IsoMaker/projects?query=is%3Aopen)

#### 2. Add an **item** to the list

#### 3 **Name it** and add a description

#### 4. Click on the inexplicably small **Create a branch**

#### 5. You may want to make the source branch different from main, otherwise leave everything as is and **create branch**

#### 6. Check out locally and code your feature (make sure to respect [the commit message standard](#commit-message-standard) when pushing onto the branch)

---

## Team ❤️

| [<img src="https://github.com/Tomi-Tom.png?size=85" width=85><br><sub>Tom BARITEAU-PETER</sub>](https://github.com/Tomi-Tom) | [<img src="https://github.com/AetherStollMetz.png?size=85" width=85><br><sub>Aether Stoll Metz</sub>](https://github.com/AetherStollMetz) | [<img src="https://github.com/steci.png?size=85" width=85><br><sub>Léa Guillemard</sub>](https://github.com/steci) | [<img src="https://github.com/Criticat02.png?size=85" width=85><br><sub>Aless Tosi</sub>](https://github.com/Criticat02)
|:---:| :---: |:------------------------------------------------------------------------------------------------------------------:| :---: | 