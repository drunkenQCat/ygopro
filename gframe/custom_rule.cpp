#include "custom_rule.h"
#include "deck_manager.h"
#include "network.h"
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <unordered_set>

namespace ygo {
namespace custom_rule {

static std::vector<uint32_t> ParseUInt32List(const char* s) {
	std::vector<uint32_t> out;
	if (!s)
		return out;
	auto pos = s;
	while (*pos && *pos != '\r' && *pos != '\n') {
		while (*pos == ' ')
			++pos;
		if (!*pos || *pos == '\r' || *pos == '\n')
			break;
		errno = 0;
		char* endptr = nullptr;
		auto value = std::strtoul(pos, &endptr, 10);
		if (endptr == pos)
			break;
		pos = endptr;
		if (errno || value > UINT32_MAX)
			continue;
		out.push_back(static_cast<uint32_t>(value));
	}
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	return out;
}

bool TryHandleLFListLine(LFList& list, const char* linebuf) {
	if (!linebuf || linebuf[0] != '$')
		return false;
	auto codes = ParseUInt32List(linebuf + 1);
	if (codes.size() <= 1)
		return true;
	list.mutualExclusiveGroups.push_back(std::move(codes));
	for (const auto code : list.mutualExclusiveGroups.back()) {
		const uint32_t v = static_cast<uint32_t>(code);
		list.hash = list.hash ^ ((v << 20) | (v >> 12));
	}
	return true;
}

uint32_t CheckDeck(const Deck& deck, const LFList& list) {
	if (list.mutualExclusiveGroups.empty())
		return 0;
	std::unordered_set<uint32_t> deckCodes;
	deckCodes.reserve(deck.main.size() + deck.extra.size() + deck.side.size());
	for (auto card : deck.main)
		deckCodes.insert(card->get_duel_code());
	for (auto card : deck.extra)
		deckCodes.insert(card->get_duel_code());
	for (auto card : deck.side)
		deckCodes.insert(card->get_duel_code());
	for (const auto& group : list.mutualExclusiveGroups) {
		int found = 0;
		uint32_t first = 0;
		for (const auto& code : group) {
			if (deckCodes.find(code) != deckCodes.end()) {
				++found;
				if (!first)
					first = code;
				if (found > 1)
					return (DECKERROR_LFLIST << 28) | first;
			}
		}
	}
	return 0;
}

SidePlacement DecideSidePlacement(const CardDataC& card, bool is_packlist) {
	if (is_packlist)
		return SidePlacement::Side;
	if (card.type & TYPES_EXTRA_DECK)
		return SidePlacement::Extra;
	return SidePlacement::Side;
}

}
}
