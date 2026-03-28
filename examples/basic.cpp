#include <rich_input_handlr/rich_input_handlr.hpp>
#include <iostream>

int main() {
    // 1. Quick one-liner (default settings)
    auto r1 = rih::ask("Wanna overwrite this file?");
    switch (r1) {
        case rih::Answer::Yes:       std::cout << "→ Yes!\n";       break;
        case rih::Answer::No:        std::cout << "→ No.\n";        break;
        case rih::Answer::Uncertain: std::cout << "→ Uncertain.\n"; break;
    }

    // 2. Configurable handler
    rih::RichInput handler{rih::Config{
        .uncertain_behavior = rih::UncertainBehavior::Prompt,
        .fuzzy_threshold    = 0.75,
        .extra_yes          = {"affirmativo", "yolo"},
        .extra_no           = {"no bueno"},
        .prompt_suffix      = " [y/n]: ",
    }};

    auto r2 = handler.ask("Continue with the installation?");
    // convert to optional<bool> to match Python's return style
    auto b = rih::RichInput::to_bool(r2);
    if (b.has_value())
        std::cout << "Got: " << (*b ? "true" : "false") << '\n';
    else
        std::cout << "Got: nullopt (uncertain)\n";

    // 3. Parse without prompting

    rih::RichInput parser;
    std::cout << "parse(\"hell yeah\") = "
              << (parser.parse("hell yeah") == rih::Answer::Yes ? "Yes" : "No/Uncertain")
              << '\n';
    std::cout << "parse(\"nah fam\")   = "
              << (parser.parse("nah fam")   == rih::Answer::No  ? "No"  : "Yes/Uncertain")
              << '\n';

    // 4. Async
    auto fut = handler.ask_async("Run async task?");
    auto r3  = fut.get();   // blocks until user answers
    std::cout << "Async result: "
              << (r3 == rih::Answer::Yes ? "Yes" : "No/Uncertain") << '\n';

    return 0;
}
