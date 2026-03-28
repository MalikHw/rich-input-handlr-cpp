#pragma once

#include <algorithm>
#include <cctype>
#include <future>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace rih {

// Public types

enum class Answer {
    Yes,       ///< Recognized as affirmative
    No,        ///< Recognized as negative
    Uncertain, ///< Could not determine (uncertain_behavior == None, or still
               ///< unclear after re-prompt)
};

enum class UncertainBehavior {
    Random, ///< 50/50 Yes/False (default)
    None,   ///< Return Answer::Uncertain, caller handles it
    Prompt, ///< Re-ask the user once, then Uncertain if still unclear
};

// Word lists

namespace detail {

inline const std::vector<std::string>& yes_words() {
    static const std::vector<std::string> w = {
        // classic
        "y","yes","yeah","yep","yup","yea","aye","ok","okay","okie",
        "sure","alright","alrighty","absolutely","definitely","certainly",
        "of course","indeed","affirmative","positive","confirmed","confirm",
        "gladly","willingly","righto","right","correct","true",
        // slang
        "ye","ya","yah","k","kk","mkay","mhm","mmhm","mm","uh huh",
        "for sure","fosho","fo sho","totally","totes","obv","obviously",
        "duh","bet","fax","facts","fr","fr fr","no cap","real",
        "say less","lets go","let's go","lesgo","letsgo","lets goo",
        "based","valid","w","big w","hard yes",
        // formal
        "yep yep","yeppers","yepperino","yeperoni","yeperooni",
        "yes indeedy","yes indeed","indeedy","indeedy do",
        "indubitably","undoubtedly","unquestionably","without a doubt",
        "without question","by all means","with pleasure",
        "most certainly","most definitely","most assuredly",
        "you bet","you betcha","betcha","you know it",
        "you know that's right","that's right","thats right",
        "roger","roger that","copy","copy that","10-4","10 4",
        "loud and clear","understood","i understand","gotcha",
        "got it","gotchu","on it","i'm on it","im on it",
        "say no more","no need to ask twice",
        "well duh","well yeah","well yes",
        "goes without saying","needless to say",
        "1","+1","👍","✅","✔","☑",
        "yes!!","yes!!!","yeah!!","yeah!!!",
        "omg yes","omg yeah","omg totally","omg absolutely",
        "oh yes","oh yeah","oh absolutely","oh definitely",
        "oh for sure","oh totally","oh most definitely",
        "yesss","yasss","yeah","yep","yup",
        // uwuspeak
        "yws","yw","uwu yes","yesh","yus","yiss","yers",
        "myes","mehyes","oki","okie dokie","okey dokey",
        "ofc","ofc!","obvi","abso-lutely",
        "yeahhh","yeahh","yesss","yessss","yessir","yessirr",
        "owo yes","uwu","owo","yesh pwease","yes pwease",
        "mhm mhm","mhmm","mhmmm","mhmhm",
        "okiee","okiii","okieee","ofc ofc",
        "yuppers","yupperino","yupp","yuppp",
        "yeeee","yeee","yeaaa","yeaaaa",
        // discord/internet
        "pog","poggers","pogchamp","lets fkin go","let's fkin go",
        "slay","periodt","periodt yes","period",
        "understood the assignment","sheesh",
        "lowkey yes","highkey yes","highkey",
        "on god","on god yes",
        // gaming
        "gg","accept","accepted","agree","agreed",
        "approved","approve","very well","as you wish","so be it",
        "it shall be done","consider it done","will do","can do",
        "i'll do it","ill do it","i will","i will do it",
        "i am willing","i am down","i'm down","im down",
        "down","down for it","down for that",
        // typos
        "yse","eys","yas",
        // multilingual
        "si","sí","claro","claro que sí","por supuesto",
        "oui","oui oui","bien sûr",
        "ja","jawohl","genau","natürlich",
        "da","hai","はい",
        "sim","com certeza",
        "ken","כן","نعم","أيوه",
        "evet","io","ναι","haan","हाँ","ne","네",
        // agreements
        "go for it","do it","just do it","make it happen",
        "make it so","engage","proceed","proceed please",
        "i consent","i agree","i concur","concur",
        "that works","works for me","works","sounds good",
        "sounds great","sounds perfect","sounds like a plan",
        "that's fine","thats fine","fine by me","fine",
        "good by me","good with me","i'm good with that",
        "im good with that","i'm cool with that","im cool with that",
        "cool with me","cool","cool cool",
        "no problem","no prob","no probs","np",
        "no objections","no issues","no complaints",
        "yass","yasss","yassss","yas queen","yas",
        "yes please","yes pls","pls yes","please yes",
        "sign me up","im in","i'm in","count me in","in",
        // profanity yes (common combos)
        "fuck yes","fuck yeah","fuck yep","fuck yup","fuck right",
        "hell yes","hell yeah","hell yep","hell yup","hell absolutely",
        "damn yes","damn yeah","damn right","damn straight",
        "shit yes","shit yeah","absolutely fucking yes",
        "oh fuck yes","oh hell yes","oh shit yes",
        "fucking yes","fucking yeah","fucking absolutely","fucking right",
        "fucking totally","fucking sure","fucking correct",
        "hell fucking yes","fuck fucking yes",
    };
    return w;
}

inline const std::vector<std::string>& no_words() {
    static const std::vector<std::string> w = {
        // classic
        "n","no","nope","nah","nay","negative","negatory",
        "never","not","not really","no way","no thanks","no thank you",
        "absolutely not","definitely not","certainly not",
        "declined","decline","reject","rejected","denied","deny",
        "false","incorrect","wrong",
        // slang
        "na","nahhh","nahh","nope nope",
        "no no","nono","nonono",
        "not a chance","no chance","fat chance",
        "pass","hard pass","im good","i'm good","i'm okay","im okay",
        "l","big l","hard no",
        "gtfo","get out","no way jose",
        "nuh uh","nuh-uh","nu uh","unh uh",
        "not today","not today satan",
        "nope nope nope","no no no",
        "under no circumstances","not in a million years",
        "not in your life","not on your life","over my dead body",
        "when pigs fly","when hell freezes over",
        "not gonna happen","ain't gonna happen","aint gonna happen",
        "not happening","never happening","never ever",
        "never ever ever","not now not ever",
        // internet/discord
        "l + ratio","ratio","ratio + no",
        "0","f","-1","👎","❌","✖","🚫",
        "no shot","no shot bruh","cap","that's cap","thats cap",
        "big cap","massive cap","nah fr","nah fr fr",
        "nah bro","nah man","nah homie","nah dawg","nah fam",
        "nope dot com",
        "not even","not even a little","not even close",
        // formal
        "i decline","i refuse","i object","i disagree","disagree",
        "objection","vetoed","veto","blocked","block",
        "not approved","disapproved","disapprove",
        "i will not","i wont","i won't","i shall not",
        "i am not willing","unwilling",
        "count me out","leave me out","leave me out of it",
        "not interested","no interest",
        "thanks but no thanks","thanks but no",
        "nty","no ty","ty no","thanks no",
        // uwuspeak
        "nwo","nuuu","nuu","nuuuu","nooo","noooo","nooooo",
        "nope uwu","no thx","ew no","ew nope","eww no","yikes no",
        "nuu pwease","nuuu pwease","pwease no",
        "no thwanks","no fank you",
        "nahhhh","nahhhhh","nooope",
        "negativo","nop","nope-a-dope",
        "nopenopenope","naw man","naw bro","nawww",
        "negatory ghost rider","that's a negative",
        "thats a no from me","thats a no","that's a no",
        "that's gonna be a no","gonna be a no",
        "i'm gonna have to say no","ima have to say no",
        // disgust
        "ew","eww","ewww","gross no","yuck","yuck no",
        "ugh no","ugh nope","bleh","bleh no",
        // typos
        "on","noo","nou",
        // multilingual
        "non","nein","niet","nee",
        "no gracias","non merci","nein danke",
        "iie","いいえ","hayır","όχι",
        "nahi","नहीं","lo","לא","لا","아니요","아니",
        // misc
        "i'd rather not","i would rather not","i'd rather die",
        "rather not","prefer not","prefer not to",
        "i'll pass","ill pass","passing",
        "skip","skip it","skipping",
        "nope out","opting out","opt out","i opt out",
        "not for me","not my thing","not my jam","not my vibe",
        "doesn't work for me","doesn't work","wont work",
        "won't work","that won't work","that doesn't work",
        "bad idea","terrible idea","awful idea",
        "i don't think so","dont think so","i think not",
        "naw",
        // profanity no (common combos)
        "fuck no","fuck nah","fuck nope","hell no","hell nah",
        "hell nope","damn no","shit no","absolutely fucking not",
        "oh fuck no","oh hell no","oh shit no",
        "fucking no","fucking nah","fucking nope","fucking never",
        "hell fucking no","fuck fucking no","fuck that",
        "fuck this","oh hell nah",
    };
    return w;
}

inline const std::vector<std::string>& uncertain_words() {
    static const std::vector<std::string> w = {
        // classic
        "maybe","perhaps","possibly","probably","might","could be",
        "idk","i don't know","i dont know","dunno","duno","donno",
        "not sure","unsure","uncertain","undecided",
        "depends","it depends","hard to say","hard to tell",
        "who knows","who knows lol",
        // casual
        "eh","ehhh","meh","mehh","kinda","kinda sorta","sorta",
        "sort of","kind of","more or less","ish","maybe ish",
        "whatever","whatevs","w/e","idrc","idc",
        "50/50","flip a coin","coin flip",
        "prolly","probs","prob","maybe maybe",
        "shrug","¯\\_(ツ)_/¯","idk man","idk bro","idk lol",
        "could go either way","either way",
        "i'm not sure","im not sure","not entirely sure",
        "not 100%","not 100 percent",
        "not totally sure","not quite sure",
        "can't say for sure","cant say for sure",
        "could be yes could be no","yes and no",
        "yes but also no","no but also yes",
        // hedging
        "to be honest","tbh","tbh idk","honestly idk",
        "on the fence","sitting on the fence",
        "torn","i'm torn","im torn",
        "conflicted","i'm conflicted","im conflicted",
        "mixed feelings","i have mixed feelings",
        "it's complicated","its complicated","complicated",
        "not black and white","gray area","grey area",
        // probability
        "likely","unlikely","somewhat likely","somewhat unlikely",
        "might be","might not be","may or may not",
        // internet/discord
        "lmao idk","lol idk","idk lmao","idk lol",
        "bruh idk","bro idk","ngl idk","ngl not sure",
        "lowkey unsure","lowkey idk","lowkey maybe",
        "🤷","🤷‍♂️","🤷‍♀️","😐","🫤","😑",
        "ehhhhh","mehhhh","idkkkk",
        "hmm","hmmm","hmmmm","hm","hmmmmmm",
        "well","uhh","uhhhh","umm","ummm","uhhh","uhm",
        "i mean","i mean maybe","i mean idk",
        "i guess","i guess so","i guess maybe",
        "i suppose","i suppose so","suppose so","guess so",
        // uwuspeak
        "maaaybe","maybeee","mebby","mebbe",
        "idkkk","idkkkk","idk idk",
        "perhaps uwu","uwu idk","maybeee uwu","not suwe",
        // profanity uncertain
        "fuck if i know","shit idk","hell if i know",
        "beats the fuck out of me","beats me",
        "fucked if i know","damned if i know","shit if i know",
        "fuck knows","shit knows","hell knows","damn knows",
        "who the fuck knows","who the hell knows","who the shit knows",
        "who the damn knows","how the fuck should i know",
        "how the hell should i know","how the shit should i know",
        "fuck idk","hell idk","damn idk",
        "the fuck do i know","the hell do i know",
        // tbd/pending
        "tbd","to be determined","tba","to be announced",
        "pending","unclear","yet to be determined",
        // sleep on it
        "ask me later","check back later",
        "let me think about it","need to think",
        "let me sleep on it","sleep on it","i'll sleep on it",
        "rain check","let's revisit","revisit",
        "subject to change","undetermined",
        "remains to be seen","we'll see","we shall see",
        "could argue both ways",
        // lean variants
        "leaning yes","leaning no",
        "leaning towards yes","leaning towards no","leaning maybe",
        "somewhere in the middle","middle ground",
        "partially","partly","in part","somewhat",
        "kind of yes","kind of no","sort of yes","sort of no",
        // multilingual
        "peut-être","tal vez","vielleicht",
        "probablemente","wahrscheinlich","forse",
        "kanske","belki","ίσως",
        "शायद","אולי","ربما","아마도",
        "多分","たぶん",
    };
    return w;
}


inline double similarity(std::string_view a, std::string_view b) {
    if (a.empty() && b.empty()) return 1.0;
    if (a.empty() || b.empty()) return 0.0;

    // Find longest common substring
    std::size_t best_a = 0, best_b = 0, best_len = 0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        for (std::size_t j = 0; j < b.size(); ++j) {
            std::size_t len = 0;
            while (i + len < a.size() && j + len < b.size() &&
                   a[i + len] == b[j + len]) {
                ++len;
            }
            if (len > best_len) {
                best_len = len;
                best_a   = i;
                best_b   = j;
            }
        }
    }

    if (best_len == 0) return 0.0;

    double score = 2.0 * static_cast<double>(best_len) /
                   static_cast<double>(a.size() + b.size());

    // Recurse on the non-matching parts
    if (best_a > 0 && best_b > 0)
        score += similarity(a.substr(0, best_a), b.substr(0, best_b)) *
                 static_cast<double>(best_a + best_b) /
                 static_cast<double>(a.size() + b.size());

    std::size_t ra = best_a + best_len, rb = best_b + best_len;
    if (ra < a.size() && rb < b.size())
        score += similarity(a.substr(ra), b.substr(rb)) *
                 static_cast<double>((a.size() - ra) + (b.size() - rb)) /
                 static_cast<double>(a.size() + b.size());

    return score;
}

inline std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

inline std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

} // namespace detail

// Config structure

struct Config {
    UncertainBehavior uncertain_behavior = UncertainBehavior::Random;
    double            fuzzy_threshold    = 0.82;
    std::vector<std::string> extra_yes        = {};
    std::vector<std::string> extra_no         = {};
    std::vector<std::string> extra_uncertain   = {};
    std::string       prompt_suffix      = " (yes/no): ";
};

// RichInput — main class

class RichInput {
public:
    explicit RichInput(Config cfg = {})
        : cfg_(std::move(cfg))
        , rng_(std::random_device{}())
    {
        // Build lookup vectors (base + extras, lowercased)
        auto build = [](const std::vector<std::string>& base,
                        const std::vector<std::string>& extra)
        {
            std::vector<std::string> out;
            out.reserve(base.size() + extra.size());
            for (auto& w : base)  out.push_back(detail::to_lower(w));
            for (auto& w : extra) out.push_back(detail::to_lower(w));
            return out;
        };

        yes_list_       = build(detail::yes_words(),       cfg_.extra_yes);
        no_list_        = build(detail::no_words(),        cfg_.extra_no);
        uncertain_list_ = build(detail::uncertain_words(), cfg_.extra_uncertain);

        // Fast exact-match sets
        yes_set_.insert(yes_list_.begin(), yes_list_.end());
        no_set_.insert(no_list_.begin(),   no_list_.end());
        unc_set_.insert(uncertain_list_.begin(), uncertain_list_.end());
    }

    // parse() — classify a pre-collected string

    [[nodiscard]] Answer parse(std::string_view raw) {
        auto classified = classify(raw);
        if (classified.has_value()) return *classified;
        return resolve_uncertain(std::string(raw));
    }

    // ask() — prompt, read stdin, classify
    
    [[nodiscard]] Answer ask(std::string_view question,
                             std::ostream&    out = std::cout,
                             std::istream&    in  = std::cin)
    {
        out << question << cfg_.prompt_suffix;
        out.flush();
        std::string raw;
        std::getline(in, raw);
        auto classified = classify(raw);
        if (classified.has_value()) return *classified;
        return resolve_uncertain(std::string(question), &out, &in);
    }

    // ask_async() — non-blocking; reads stdin in a thread-pool thread


    [[nodiscard]] std::future<Answer> ask_async(std::string question) {
        return std::async(std::launch::async, [this, q = std::move(question)]() {
            return ask(q);
        });
    }

    // parse_async()

    [[nodiscard]] std::future<Answer> parse_async(std::string raw) {
        return std::async(std::launch::async, [this, r = std::move(raw)]() {
            return parse(r);
        });
    }

    // Convenience: convert Answer to optional<bool>

    static std::optional<bool> to_bool(Answer a) {
        switch (a) {
            case Answer::Yes:       return true;
            case Answer::No:        return false;
            case Answer::Uncertain: return std::nullopt;
        }
        return std::nullopt;
    }

private:
    Config cfg_;
    std::mt19937 rng_;

    std::vector<std::string> yes_list_, no_list_, uncertain_list_;
    std::unordered_set<std::string> yes_set_, no_set_, unc_set_;

    // Returns nullopt when uncertain/unrecognized
    [[nodiscard]] std::optional<Answer> classify(std::string_view raw) {
        std::string text = detail::trim(detail::to_lower(std::string(raw)));

        // Exact match (fast path)
        if (yes_set_.count(text)) return Answer::Yes;
        if (no_set_.count(text))  return Answer::No;
        if (unc_set_.count(text)) return std::nullopt; // uncertain

        // Fuzzy match — find best candidate across all lists
        double best_score  = 0.0;
        std::optional<Answer> best_result;

        auto check = [&](const std::vector<std::string>& list, Answer ans) {
            for (const auto& word : list) {
                double s = detail::similarity(text, word);
                if (s > best_score) {
                    best_score  = s;
                    best_result = ans;
                }
            }
        };

        check(yes_list_,       Answer::Yes);
        check(no_list_,        Answer::No);
        check(uncertain_list_, Answer::Uncertain);

        // Uncertain list fuzzy match → treat as uncertain (nullopt)
        if (best_score >= cfg_.fuzzy_threshold) {
            if (best_result == Answer::Uncertain) return std::nullopt;
            return best_result;
        }

        // Completely unrecognized → uncertain
        return std::nullopt;
    }

    [[nodiscard]] Answer resolve_uncertain(const std::string& context,
                                           std::ostream* out = nullptr,
                                           std::istream* in  = nullptr)
    {
        switch (cfg_.uncertain_behavior) {
            case UncertainBehavior::Random: {
                std::uniform_int_distribution<int> dist(0, 1);
                return dist(rng_) ? Answer::Yes : Answer::No;
            }
            case UncertainBehavior::None:
                return Answer::Uncertain;

            case UncertainBehavior::Prompt: {
                auto& o = out ? *out : std::cout;
                auto& i = in  ? *in  : std::cin;
                o << "Couldn't understand that. Please answer yes or no.\n";
                o << context << cfg_.prompt_suffix;
                o.flush();
                std::string raw2;
                std::getline(i, raw2);
                auto result = classify(raw2);
                if (!result.has_value()) {
                    o << "Still couldn't understand. Treating as no answer.\n";
                    return Answer::Uncertain;
                }
                return *result;
            }
        }
        return Answer::Uncertain;
    }
};

// Module-level convenience

inline Answer ask(std::string_view question) {
    static RichInput default_handler;
    return default_handler.ask(question);
}

} // namespace rih
