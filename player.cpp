#include "player.hpp"
#include "pi.hpp"

#include <SFML/Graphics.hpp>

#include <cmath>
#include <iostream>
#include <stdexcept>

sf::Texture loadTextureFromFile(const std::string& filename) {
    sf::Texture rv;
    if(!rv.loadFromFile(filename)) {
        throw std::runtime_error("ERROR: Could not loadTextureFromFile \"" + filename + '"');
    }
    return rv;
}

Player::Player(sf::RenderWindow& window) :
    windowptr(&window),
    window_width(window.getSize().x),
    window_height(window.getSize().y),
    normalTexture(loadTextureFromFile("content/normal.png")),
    firedTexture(loadTextureFromFile("content/fired.png")),
    bulletMgr(window, 40) //40 bullets tops

{
    player.setTexture(normalTexture);
    player.setScale(.2, .2);

    //auto [left, top, pwidth, pheight] = player.getGlobalBounds();
    auto [left, top, pwidth, pheight] = player.getLocalBounds();
    player.setOrigin(pwidth / 2, pheight / 2);

    auto[wwidth, wheight] = window.getSize();
    player.setPosition(wwidth / 2, wheight / 2);
}

float Player::getAngle() const
{
    return player.getRotation();
}

const sf::Vector2f& Player::getPosition() const {
    return player.getPosition();
}

const sf::Vector2f& Player::getVelocity() const {
    return velocity;
}

bool Player::handleEvent([[maybe_unused]] const sf::Event& ev) {
    return false;
}


void Player::Move(duration time)
{

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        bulletMgr.fireBullet(getPosition(), getVelocity(), getAngle());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player.rotate(-60 * time);
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player.rotate(60 * time);
    }

    // Accelerate on button press
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        player.setTexture(firedTexture);
        auto angle = player.getRotation() * M_PI / 180;
        velocity += sf::Vector2f(std::cos(angle), std::sin(angle)) * (time * 15);
    }
    else
    {
        player.setTexture(normalTexture);
    }
    // Move in direction player is pointed
    player.move(velocity * time);

    screenWrapping();

    bulletMgr.Move(time);
}

void Player::screenWrapping()
{
    // If player goes beyond x bounds set position to opposite site of screen
    if (player.getPosition().x > window_width)
    {
        player.setPosition(0, player.getPosition().y);
    }
    else if (player.getPosition().x <= 0)
    {
        player.setPosition(window_width, player.getPosition().y);
    }

    // If player goes beyond y bounds set position to opposite site of screen
    if (player.getPosition().y >= window_height)
    {
        player.setPosition(player.getPosition().x, 0);
    }
    else if (player.getPosition().y <= 0)
    {
        player.setPosition(player.getPosition().x, window_height);
    }
}

void Player::Draw()
{
    windowptr->draw(player);
    bulletMgr.Draw();
}
