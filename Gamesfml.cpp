#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <cstdlib>
#include "player.cpp"
#include "rollDice.cpp"
#include "snakesandladders.cpp"

using namespace std;

enum class State { EnterNames, Playing, GameOver };

string toStd(const sf::String& s) {
    auto u = s.toUtf8();
    return string(reinterpret_cast<const char*>(u.data()), u.size());
}

sf::Vector2f tokenPos(int pos, int idx) {
    if (pos == 0) {
        return {20.f + idx * 40.f, 812.f};
    }
    int i = pos - 1;
    int row = i / 10;
    int col = i % 10;
    if (row % 2 != 0) col = 9 - col;

    float cx = col * 100.f + 50.f;
    float cy = 800.f - (row + 1) * 80.f + 40.f;
    float offX = (idx == 0) ? -12.f : 12.f;
    float offY = (idx == 0) ? -10.f : 10.f;
    return {cx + offX - 15.f, cy + offY - 15.f};
}

class Game {
private:
    sf::RenderWindow window{sf::VideoMode({1000, 860}), "Snakes and Ladders"};
    sf::Font font;

    player p1{"Player 1"};
    player p2{"Player 2"};
    snakesandladders board;
    rolldice dice;

    State state = State::EnterNames;
    sf::String input1, input2;
    sf::String name1 = "Player 1";
    sf::String name2 = "Player 2";
    sf::String message;
    sf::String winnerName;
    int activeField = 0;
    int turn = 0;
    string lastRoll = "-";
    int lastRollValue = 0;

    sf::Texture boardTexture;
    sf::Sprite boardSprite{boardTexture};
    sf::CircleShape token1{15.f};
    sf::CircleShape token2{15.f};

public:
    Game() {
        window.setFramerateLimit(60);

        if (!font.openFromFile("Roboto_Condensed-Black.ttf")) {
             
            cout << "Error, Could not load Roboto_Condensed-Black.ttf " << endl;
            exit(1);

        }

        if (!boardTexture.loadFromFile("images.jpeg")) {

            cout << "Error, Could not load images.jpeg " << endl;

        } else {

            boardSprite.setTexture(boardTexture, true);

        }

        token1.setFillColor(sf::Color::Yellow);
        token2.setFillColor(sf::Color(46, 134, 193));

    }

    void startGame() {

        name1 = input1.isEmpty() ? sf::String("Player 1") : input1;
        name2 = input2.isEmpty() ? sf::String("Player 2") : input2;
        p1 = player(toStd(name1));
        p2 = player(toStd(name2));
        turn = 0;
        lastRoll = "-";
        lastRollValue = 0;
        message = sf::String("It is ") + name1 + sf::String("'s turn");
        state = State::Playing;

    }

    void resetGame() {

        state = State::EnterNames;
        input1.clear();
        input2.clear();
        activeField = 0;
        lastRoll = "-";
        lastRollValue = 0;
        message.clear();
        winnerName.clear();

    }

    void playTurn() {

        player& cur = (turn == 0) ? p1 : p2;
        int roll = dice.rolldic();
        lastRoll = to_string(roll);
        lastRollValue = roll;
        cur.moveby(roll);

        if (cur.getposition() > 100) {

            cur.moveby(-(cur.getposition() - 100));

        }

        int pos = cur.getposition();

        if (board.ladders.count(pos)) {

            message = sf::String("Ladder! Climbing to " + to_string(board.ladders.at(pos)));
            cur.moveby(board.ladders.at(pos) - pos);
            pos = cur.getposition();

        } else if (board.snakes.count(pos)) {

            message = sf::String("Snake! Sliding to " + to_string(board.snakes.at(pos)));
            cur.moveby(board.snakes.at(pos) - pos);
            pos = cur.getposition();

        }

        if (pos == 100) {

            winnerName = (turn == 0) ? name1 : name2;
            state = State::GameOver;
            return;

        }

        turn = 1 - turn;
        sf::String nextName = (turn == 0) ? name1 : name2;
        message = sf::String("It is ") + nextName + sf::String("'s turn");

    }

    void processEvents() {

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }

            if (state == State::EnterNames) {
                if (const auto* text = event->getIf<sf::Event::TextEntered>()) {
                    char32_t c = text->unicode;
                    bool printable = c >= 32 && c != 127 && !(c >= 0x80 && c <= 0x9F);
                    if (printable) {
                        sf::String& field = (activeField == 0) ? input1 : input2;
                        if (field.getSize() < 12) {
                            field += sf::String(c);
                        }
                    }
                }
                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Backspace) {
                        sf::String& field = (activeField == 0) ? input1 : input2;
                        if (field.getSize() > 0) field.erase(field.getSize() - 1);
                    } else if (key->code == sf::Keyboard::Key::Tab) {
                        activeField = 1 - activeField;
                    } else if (key->code == sf::Keyboard::Key::Enter) {
                        startGame();
                    }
                }
            }

            else if (state == State::Playing) {
                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Space) {
                        playTurn();
                    }
                }
            }

            else if (state == State::GameOver) {
                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::R) {
                        resetGame();
                    }
                }
            }
        }
    }

    void update() {
        token1.setPosition(tokenPos(p1.getposition(), 0));
        token2.setPosition(tokenPos(p2.getposition(), 1));

    }

    sf::Text makeText(const sf::String& s, unsigned size, sf::Vector2f pos, sf::Color color) {
        sf::Text t(font, s, size);
        t.setPosition(pos);
        t.setFillColor(color);
        return t;

    }

    sf::Text makeCenteredText(const sf::String& s, unsigned size, float y, sf::Color color) {

        sf::Text t(font, s, size);
        float w = t.getLocalBounds().size.x;
        t.setPosition({500.f - w / 2.f, y});
        t.setFillColor(color);
        return t;

    }

    void drawDice(float x, float y, int value) {

        sf::RectangleShape box({48.f, 48.f});
        box.setPosition({x, y});
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(255, 212, 0));
        window.draw(box);

        static const vector<vector<int>> pips = {
            {}, {4}, {0, 8}, {0, 4, 8}, {0, 2, 6, 8}, {0, 2, 4, 6, 8}, {0, 2, 3, 5, 6, 8}
        };
        if (value < 1 || value > 6) return;

        for (int idx : pips[value]) {
            sf::CircleShape dot(5.f);
            dot.setPosition({x + 8.f + (idx % 3) * 13.f, y + 8.f + (idx / 3) * 13.f});
            dot.setFillColor(sf::Color(20, 20, 20));
            window.draw(dot);
        }

    }

    void drawNames() {

        window.draw(makeCenteredText("Snakes and Ladders", 48, 185.f, sf::Color::White));

        window.draw(makeCenteredText("Player 1", 22, 325.f, sf::Color(127, 166, 209)));
        sf::RectangleShape box1({400.f, 60.f});
        box1.setPosition({300.f, 355.f});
        box1.setFillColor(sf::Color(20, 44, 80));
        box1.setOutlineThickness(3.f);
        box1.setOutlineColor(activeField == 0 ? sf::Color(255, 212, 0) : sf::Color(42, 79, 122));
        window.draw(box1);
        sf::String shown1 = input1 + (activeField == 0 ? sf::String("_") : sf::String(""));
        window.draw(makeCenteredText(shown1, 28, 367.f, sf::Color::White));

        window.draw(makeCenteredText("Player 2", 22, 445.f, sf::Color(127, 166, 209)));
        sf::RectangleShape box2({400.f, 60.f});
        box2.setPosition({300.f, 475.f});
        box2.setFillColor(sf::Color(20, 44, 80));
        box2.setOutlineThickness(3.f);
        box2.setOutlineColor(activeField == 1 ? sf::Color(255, 212, 0) : sf::Color(42, 79, 122));
        window.draw(box2);
        sf::String shown2 = input2 + (activeField == 1 ? sf::String("_") : sf::String(""));
        window.draw(makeCenteredText(shown2, 28, 487.f, sf::Color::White));

        window.draw(makeCenteredText("Tab: switch    Enter: start    Backspace: delete", 18, 595.f, sf::Color(127, 166, 209)));


    }

    void drawHud() {


        sf::RectangleShape bar({1000.f, 60.f});
        bar.setPosition({0.f, 800.f});
        bar.setFillColor(sf::Color(15, 36, 64));
        window.draw(bar);

        drawDice(820.f, 806.f, lastRollValue);

        if (state == State::GameOver) {
            window.draw(makeText(winnerName + sf::String(" wins! Press R to play again"), 22, {150.f, 818.f}, sf::Color(255, 212, 0)));
            return;
        }

        sf::String turnName = (turn == 0) ? name1 : name2;
        window.draw(makeText(sf::String("Turn: ") + turnName, 20, {150.f, 808.f}, sf::Color::White));
        window.draw(makeText(sf::String("Last roll: " + lastRoll), 20, {450.f, 808.f}, sf::Color::White));
        window.draw(makeText(message, 18, {150.f, 834.f}, sf::Color(127, 166, 209)));


    }

    void render() {


        window.clear(sf::Color(7, 17, 31));

        if (state == State::EnterNames) {
            drawNames();
        } else {
            window.draw(boardSprite);
            window.draw(token1);
            window.draw(token2);
            drawHud();
        }

        window.display();


    }

    void run() {

        while (window.isOpen()) {
            processEvents();
            update();
            render();
        }
    }
};

int main() {

    Game myGame;
    myGame.run();
    return 0;

}