# Git Commit Message Guidelines

## Format
```
<type>[optional scope]: <description>
[optional body]
[optional footer(s)]
```

## Types
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Formatting changes (no code change)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding/modifying tests
- `chore`: Build process/tooling changes
- `revert`: Reverting a previous commit

## Guidelines
- Use imperative mood ("add" not "added")
- First letter lowercase
- Keep subject under 50 characters
- Wrap body at 72 characters
- Body explains "what" and "why" (not "how")
- Footer:
  - Close issues with `Fixes #123`
  - Mark breaking changes with `BREAKING CHANGE: description`

## Examples

```
fix(build): resolve compilation error

Fixes undefined reference error during linking phase caused by
incorrect static variable initialization order in the utils module.

Fixes #1
```