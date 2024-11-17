# IsoMaker

IsoMaker is a game engine for creating isometric games. It is written in C++ and uses the Raylib for rendering.

Your draft provides a solid foundation! Here's a refined and detailed version that expands on your initial structure:  

---

## Commit Message Standard

All commits contributing to this project must adhere to the following message format, inspired by the [Conventional Commits format](https://www.conventionalcommits.org/en/v1.0.0/):  

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
  - Start with a verb in the present tense (e.g., *"add,"* *"fix,"* *"update"*).
  - Don't capitalize the first letter or end with a period.

### Examples

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

## Team ❤️

| [<img src="https://github.com/Tomi-Tom.png?size=85" width=85><br><sub>Tom BARITEAU-PETER</sub>](https://github.com/Tomi-Tom) | [<img src="https://github.com/AetherStollMetz.png?size=85" width=85><br><sub>Aether Stoll Metz</sub>](https://github.com/AetherStollMetz) | [<img src="https://github.com/steci.png?size=85" width=85><br><sub>Léa Guillemard</sub>](https://github.com/steci) | [<img src="https://github.com/Criticat02.png?size=85" width=85><br><sub>Aless Tosi</sub>](https://github.com/Criticat02)
|:---:| :---: |:------------------------------------------------------------------------------------------------------------------:| :---: | 