#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>


void printCardSFML(sf::RenderWindow& window, const Card& card, float x, float y) {
    sf::RectangleShape cardShape(sf::Vector2f(60, 80));
    cardShape.setFillColor(sf::Color::White);
    cardShape.setOutlineThickness(2);
    cardShape.setOutlineColor(sf::Color::Black);
    cardShape.setPosition(x, y);
    window.draw(cardShape);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text valueText;
    valueText.setFont(font);
    valueText.setString(std::to_string(card.value));
    valueText.setCharacterSize(30);
    valueText.setFillColor(sf::Color::Black);
    valueText.setPosition(x + 5, y + 5);
    window.draw(valueText);

    sf::Text suitText;
    suitText.setFont(font);
    suitText.setString(card.suit);
    suitText.setCharacterSize(30);
    suitText.setFillColor(sf::Color::Black);
    suitText.setPosition(x + 35, y + 55);
    window.draw(suitText);
}

void printHandsSFML(sf::RenderWindow& window, const std::vector<std::vector<Card>>& hands, float startX, float startY, float spacing) {
    float x = startX;
    float y = startY;
    for (const auto& hand : hands) {
        printCardSFML(window, hand[0], x, y);
        printCardSFML(window, hand[1], x + 70, y);
        y += spacing;
    }
}


enum class Rank {
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH
};

struct Card {
    int value;
    char suit;
};

bool compareCards(const Card& a, const Card& b) {
    return a.value < b.value;
}

bool isFlush(const std::vector<Card>& hand) {
    char suit = hand[0].suit;
    for (const auto& card : hand) {
        if (card.suit != suit)
            return false;
    }
    return true;
}

bool isStraight(const std::vector<Card>& hand) {
    for (int i = 1; i < hand.size(); ++i) {
        if (hand[i].value != hand[i - 1].value + 1)
            return false;
    }
    return true;
}

Rank determineRank(const std::vector<Card>& hand) {
    std::vector<Card> sortedHand = hand;
    std::sort(sortedHand.begin(), sortedHand.end(), compareCards);

    if (isFlush(hand) && isStraight(hand)) {
        if (sortedHand[0].value == 10)
            return Rank::ROYAL_FLUSH;
        else
            return Rank::STRAIGHT_FLUSH;
    } else if (isFlush(hand)) {
        return Rank::FLUSH;
    } else if (isStraight(hand)) {
        return Rank::STRAIGHT;
    }

    return Rank::HIGH_CARD;
}

std::vector<std::vector<Card>> generateHands(const std::vector<Card>& tableCards, const std::vector<Card>& specificHand) {
    std::vector<Card> remainingDeck;

    for (int value = 2; value <= 14; ++value) {
        for (char suit : {'S', 'H', 'D', 'C'}) {
            bool cardAvailable = true;
            for (const Card& card : specificHand) {
                if (card.value == value && card.suit == suit) {
                    cardAvailable = false;
                    break;
                }
            }
            for (const Card& card : tableCards) {
                if (card.value == value && card.suit == suit) {
                    cardAvailable = false;
                    break;
                }
            }
            if (cardAvailable) {
                remainingDeck.push_back({value, suit});
            }
        }
    }

    std::vector<std::vector<Card>> beatingHands;

    // Generate hands including specific hand and table cards
    for (const Card& card1 : specificHand) {
        for (const Card& card2 : tableCards) {
            std::vector<Card> hand = {card1, card2};
            if (determineRank(hand) > determineRank(specificHand)) {
                beatingHands.push_back(hand);
            }
        }
    }

    // Generate hands including table cards only
    for (size_t i = 0; i < tableCards.size(); ++i) {
        for (size_t j = i + 1; j < tableCards.size(); ++j) {
            std::vector<Card> hand{tableCards[i], tableCards[j]};
            if (determineRank(hand) > determineRank(specificHand)) {
                beatingHands.push_back(hand);
            }
        }
    }

    return beatingHands;
}


int main() {

    sf::RenderWindow window(sf::VideoMode(800, 600), "Poker Hands");

    std::vector<Card> specificHand(2);
    
    std::cout << "Enter the value and suit of your first card (e.g., 10 S for 10 of Spades): ";
    std::cin >> specificHand[0].value >> specificHand[0].suit;
    if (specificHand[0].value == 1 || specificHand[0].value == 14) {
        specificHand[0].value = 14;
    }
    std::cout << "Enter the value and suit of your second card: ";
    std::cin >> specificHand[1].value >> specificHand[1].suit;
    if (specificHand[1].value == 1 || specificHand[1].value == 14) {
        specificHand[1].value = 14;
    }

    std::vector<Card> tableCards(3);
    std::cout << "Enter the 3 cards on the table and their respective suits:" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Card " << i + 1 << ": ";
        std::cin >> tableCards[i].value >> tableCards[i].suit;
        if (tableCards[i].value == 1 || tableCards[i].value == 14) {
            tableCards[i].value = 14;
        }
    }

    std::vector<std::vector<Card>> beatingHands = generateHands(tableCards, specificHand);
    std::cout << "Hands that beat your hand after the flop:" << std::endl;
    for (const auto& hand : beatingHands) {
        std::cout << "Hand: " << hand[0].value << hand[0].suit << ", " << hand[1].value << hand[1].suit << std::endl;
    }

    std::cout << "Enter the turn card value and suit: ";
    Card turnCard;
    std::cin >> turnCard.value >> turnCard.suit;
    if (turnCard.value == 1 || turnCard.value == 14) {
        turnCard.value = 14;
    }
    tableCards.push_back(turnCard);

    beatingHands = generateHands(tableCards, specificHand);
    std::cout << "Hands that beat your hand after the turn:" << std::endl;
    for (const auto& hand : beatingHands) {
        std::cout << "Hand: " << hand[0].value << hand[0].suit << ", " << hand[1].value << hand[1].suit << std::endl;
    }

    std::cout << "Enter the river card value and suit: ";
    Card riverCard;
    std::cin >> riverCard.value >> riverCard.suit;
    if (riverCard.value == 1 || riverCard.value == 14) {
        riverCard.value = 14;
    }
    tableCards.push_back(riverCard);

    beatingHands = generateHands(tableCards, specificHand);
    std::cout << "Hands that beat your hand after the river:" << std::endl;
    for (const auto& hand : beatingHands) {
        std::cout << "Hand: " << hand[0].value << hand[0].suit << ", " << hand[1].value << hand[1].suit << std::endl;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Green);

        printCardSFML(window, specificHand[0], 50, 200);
        printCardSFML(window, specificHand[1], 130, 200);

        
        window.display();
    }

    return 0;
}
