//#include <SFML/Graphics.hpp>
//
//int main() {
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Window",
//        sf::Style::Titlebar | sf::Style::Close);
//    sf::Font arial;
//    arial.loadFromFile("Candy Beans.otf");
//    sf::Text t;
//    t.setFillColor(sf::Color::White);
//    t.setFont(arial);
//    std::string s = "This is text that you type: ";
//    t.setString(s);
//
//    while (window.isOpen()) {
//        sf::Event event;
//
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//            if (event.type == sf::Event::TextEntered) {
//                if (event.text.unicode < 128) {
//                    if (event.text.unicode == 8 && !s.empty())
//                    {
//                        s.pop_back();
//                    }else
//                    s += static_cast<char>(event.text.unicode);
//                }
//            }
//        }
//        t.setString(s);
//        window.clear(sf::Color::Black);
//        window.draw(t);
//        window.display();
//    }
//}