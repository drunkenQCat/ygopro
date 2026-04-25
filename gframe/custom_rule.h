#ifndef CUSTOM_RULE_H
#define CUSTOM_RULE_H

#include <cstdint>

namespace ygo {
struct Deck;
struct LFList;
struct CardDataC;

namespace custom_rule {

enum class SidePlacement : uint8_t {
	Side = 0,
	Extra = 1,
	Ignore = 2
};

bool TryHandleLFListLine(LFList& list, const char* linebuf);
uint32_t CheckDeck(const Deck& deck, const LFList& list);
SidePlacement DecideSidePlacement(const CardDataC& card, bool is_packlist);

}
}

#endif // CUSTOM_RULE_H
