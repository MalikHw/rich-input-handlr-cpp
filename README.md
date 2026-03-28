# rich-input-handlr — C++20 port

> Because nobody types exactly `Y` or `N`.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square)
![Header-only](https://img.shields.io/badge/header--only-yes-brightgreen?style=flat-square)
![Zero dependencies](https://img.shields.io/badge/dependencies-zero-brightgreen?style=flat-square)
![License: GPLv3](https://img.shields.io/badge/license-GPLv3-blue?style=flat-square)
![Platforms](https://img.shields.io/badge/platforms-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey?style=flat-square)

C++20 port of the [Python library](https://github.com/MalikHw/rich-input-handlr).  
Single header. Zero dependencies. Stdlib only.

```cpp
"You sure wanna download Miku songs? (yes/no): fuck yeah"
→ Answer::Yes
```

---

## Install

### Option 1 — Copy the header
```
include/rich_input_handlr/rich_input_handlr.hpp
```
Drop it anywhere on your include path. Done.

### Option 2 — CMake FetchContent
```cmake
include(FetchContent)
FetchContent_Declare(rih
    GIT_REPOSITORY https://github.com/MalikHw/rich-input-handlr-cpp.git
    GIT_TAG        v20240315-143022   # ← use a real tag from Releases
)
FetchContent_MakeAvailable(rih)
target_link_libraries(my_target PRIVATE rih::rich_input_handlr)
```

### Option 3 — vcpkg (overlay port)
```
vcpkg install rich-input-handlr
```

---

## Quick start

```cpp
#include <rich_input_handlr/rich_input_handlr.hpp>

// one-liner (default settings)
auto r = rih::ask("Wanna overwrite this file?");
if (r == rih::Answer::Yes) {
    overwrite();
} else if (r == rih::Answer::No) {
    std::cout << "okay skipping lol\n";
} else {
    std::cout << "couldn't figure out what you meant\n";
}
```

---

## Return type

```cpp
enum class Answer { Yes, Uncertain, No };
```

Convert to `optional<bool>` to match the Python API:
```cpp
auto b = rih::RichInput::to_bool(r);
// b == true      → yes
// b == false     → no
// b == nullopt   → uncertain
```

---

## Full config

```cpp
rih::RichInput handler{rih::Config{
    .uncertain_behavior = rih::UncertainBehavior::Prompt,  // Random | None | Prompt
    .fuzzy_threshold    = 0.82,                            // 0.0–1.0
    .extra_yes          = {"affirmativo", "let it rip"},
    .extra_no           = {"no bueno", "abort"},
    .extra_uncertain    = {"on the fence"},
    .prompt_suffix      = " [y/n]: ",
}};

auto r = handler.ask("Continue?");
```

### `UncertainBehavior` options

| Value | On uncertain / unrecognized input |
|---|---|
| `Random` | 50/50 `Yes` or `No` (default) |
| `None` | returns `Answer::Uncertain` — you deal with it |
| `Prompt` | re-asks once, then `Uncertain` if still unclear |

---

## Async

```cpp
auto fut = handler.ask_async("Proceed?");
// ... do other stuff ...
auto r = fut.get();   // blocks here until user answers
```

```cpp
auto fut = handler.parse_async("hell yeah");
auto r   = fut.get();  // Answer::Yes
```

Both use `std::async(std::launch::async, ...)` — no extra threads to manage.

---

## Parse without prompting

```cpp
rih::RichInput parser;
parser.parse("hell yeah");  // Answer::Yes
parser.parse("nah fam");    // Answer::No
parser.parse("idk man");    // Answer::Uncertain (or random, depends on config)
```

---

## Fuzzy matching

Typos are handled via a Ratcliff/Obershelp similarity implementation
(the same algorithm as Python's `difflib.SequenceMatcher`).  
Default threshold: `0.82`.

```cpp
rih::RichInput h{rih::Config{.fuzzy_threshold = 0.65}}; // more lenient
h.parse("yse");   // → Answer::Yes
h.parse("noo");   // → Answer::No
```

> **Unicode note:** exact matching works correctly for all UTF-8 input
> (multilingual words, emoji, etc.). Fuzzy matching operates at the byte
> level rather than the grapheme level, so it is less accurate for
> non-ASCII inputs. All multilingual entries in the built-in word lists
> are covered by exact matching.

---

## Build from source

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

---

## License

[GPL-3.0](./LICENSE)

### [Youtube Channel](https://www.youtube.com/@MalikHw47) — [Donate](https://MalikHw.github.io/donate) ...or thx thats it
