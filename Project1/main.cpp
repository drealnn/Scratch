#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class Timer {
public:
    static sf::Clock* clock;
    static float getElapsedTime() {
        float secs = clock->getElapsedTime().asSeconds();
        //clock->restart();
        return secs;
    }
    static void restart() {
        clock->restart();
    }
};

sf::Clock* Timer::clock = new sf::Clock();

class SnakeBlock : public sf::Drawable {
public:
    SnakeBlock* m_Head;
    sf::RectangleShape m_Base;
    int m_CurDirection;
    int m_Direction;
    int m_Speed;
    std::vector<std::vector<sf::RectangleShape>> m_GridArea;
    sf::Vector2i m_GridIndex;

    SnakeBlock(sf::RectangleShape& base, SnakeBlock* head, std::vector<std::vector<sf::RectangleShape>>& gridArea) : m_Base(base),  m_Head(head), m_GridArea(gridArea) {
        if (head == nullptr) {
            // set at center
            int rows = gridArea.size();
            int cols = gridArea[0].size();
            m_GridIndex = { (int)(cols / 2), (int)(rows / 2) };
            m_Base.setPosition(gridArea[m_GridIndex.y][m_GridIndex.x].getPosition());
        }
        else {
            // set behind based on head direction from grid index
            sf::Vector2f initPos = m_Head->getPosition();
            sf::Vector2i initGrid = m_Head->getCurrentIndex();
            int headDirection = m_Head->getDirection();
            switch (headDirection) {
            case(0):
                initGrid += {0, 1};
                break;
            case(1):
                initGrid -= {0, 1};
                break;
            case(2):
                initGrid -= {1, 0};
                break;
            case(3):
                initGrid += {1, 0};
                break;
            }
            m_GridIndex = initGrid;
            m_Base.setPosition(gridArea[m_GridIndex.y][m_GridIndex.x].getPosition());
        }

        m_Direction = 0;
        m_CurDirection = getDirection();
        m_Speed = m_Base.getSize().x * 8;

    }

    void go(float time) {
        if (m_Head != nullptr) {
            m_Head->go(time);
        }
        if (moveTo(m_CurDirection, time)) {
            m_CurDirection = getDirection();
        }
    }

    bool moveTo(int curDirection, float time) {
        sf::Vector2i destinationIndex = getNextDestinationIndex(curDirection);
        sf::Vector2f newPos = m_Base.getPosition();
        //std::cout << destinationIndex.x << "," << destinationIndex.y << (m_Head != nullptr ? "has head" : "") << std::endl;
        sf::Vector2f destination = m_GridArea[destinationIndex.y][destinationIndex.x].getPosition();
        float distance = m_Speed * time;
        // up
        switch (curDirection) {
            // up
        case(0):
            if (destination.y < m_Base.getPosition().y) {
                newPos -= {0, distance};
            }
            break;
        case(1):
            if (destination.y > m_Base.getPosition().y) {
                newPos += {0, distance};
            }
            break;
            // right
        case(2):
            if (destination.x > m_Base.getPosition().x) {
                newPos += {distance, 0};
            }
            break;
        case(3):
            if (destination.x < m_Base.getPosition().x) {
                newPos -= {distance, 0};
            }
            break;
        }
        bool reachedDestination = newPos == m_Base.getPosition();
        if (reachedDestination) {
            newPos = destination;
            m_GridIndex = destinationIndex;
        }
        m_Base.setPosition(newPos);
        return reachedDestination;
    }

    sf::Vector2i getNextDestinationIndex(int direction) {
        //std::cout << direction << std::endl;
        switch (direction) {
        case(0):
            return { m_GridIndex.x, m_GridIndex.y - 1 };
        case(1):
            return { m_GridIndex.x, m_GridIndex.y + 1 };
        case(2):
            return { m_GridIndex.x + 1, m_GridIndex.y };
        case(3):
            return { m_GridIndex.x - 1, m_GridIndex.y };
        }
    }

    sf::Vector2i getCurrentIndex() {
        return m_GridIndex;
    }

    void setDirection(int direction) {
        m_Direction = direction;
    }

    int getDirection() {
        if (m_Head != nullptr) {
            sf::Vector2i curIndex = m_Head->getCurrentIndex();
            //std::cout << "head index: " << curIndex.x << "," << curIndex.y << ", tail index: " << m_GridIndex.x << "," << m_GridIndex.y << std::endl;
            if (curIndex.x > m_GridIndex.x) {
                return 2;
            }
            else if (curIndex.x < m_GridIndex.x) {
                return 3;
            }
            else if (curIndex.y < m_GridIndex.y) {
                return 0;
            }
            else if (curIndex.y > m_GridIndex.y) {
                return 1;
            }
        } else {
            return m_Direction;
        }
    }

    float getSize() {
        return m_Base.getSize().x;
    }
    sf::Vector2f getPosition() {
        return m_Base.getPosition();
    }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(m_Base, states);
        if (m_Head != nullptr) {
            m_Head->draw(target, states);
        }
    }
};

class Snake: public sf::Drawable {
public:
    sf::RectangleShape m_Head;
    int m_CurDirection;
    int m_Direction;
    int m_Speed;
    std::vector<std::vector<sf::RectangleShape>> m_GridArea;
    sf::Vector2i m_GridIndex;

    Snake(sf::RectangleShape& head, std::vector<std::vector<sf::RectangleShape>>& gridArea): m_Head(head),m_GridArea(gridArea) {
        m_CurDirection = 0;
        m_Direction = 0;
        m_Speed = m_Head.getSize().x;

        // get center
        int rows = gridArea.size();
        int cols = gridArea[0].size();
        m_GridIndex = { (int)(cols / 2), (int)(rows / 2) };
        m_Head.setPosition(gridArea[m_GridIndex.y][m_GridIndex.x].getPosition());
    }

    void go() {
        if (moveTo(m_CurDirection)) {
            m_CurDirection = m_Direction;
        }
    }

    bool moveTo(int curDirection) {
        sf::Vector2i destinationIndex = getNextDestinationIndex(curDirection);
        sf::Vector2f newPos = m_Head.getPosition();
        sf::Vector2f destination = m_GridArea[destinationIndex.y][destinationIndex.x].getPosition();
        float distance = m_Speed * Timer::getElapsedTime();
        // up
        switch (curDirection) {
            // up
        case(0):
            if (destination.y < m_Head.getPosition().y) {
                newPos -= {0, distance};
            }
            break;
        case(1):
            if (destination.y > m_Head.getPosition().y) {
                newPos += {0, distance};
            }
            break;
            // right
        case(2):
            if (destination.x > m_Head.getPosition().x) {
                newPos += {distance, 0};
            }
            break;
        case(3):
            if (destination.x < m_Head.getPosition().x) {
                newPos -= {distance, 0};
            }
            break;
        }
        bool reachedDestination = newPos == m_Head.getPosition();
        if (reachedDestination) {
            newPos = destination;
            m_GridIndex = destinationIndex;
        }
        m_Head.setPosition(newPos);
        return reachedDestination;
    }

    sf::Vector2i getNextDestinationIndex(int direction) {
        switch (direction) {
        case(0):
            return { m_GridIndex.x, m_GridIndex.y - 1 };
        case(1):
            return { m_GridIndex.x, m_GridIndex.y + 1 };
        case(2):
            return { m_GridIndex.x + 1, m_GridIndex.y };
        case(3):
            return { m_GridIndex.x - 1, m_GridIndex.y };
        }
    }

    void setDirection(int direction) {
        m_Direction = direction;
    }
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(m_Head, states);
    }
};

class PlayArea : public sf::Drawable {
public:
    sf::Vector2f m_Position;
    sf::RectangleShape m_Base;
    std::vector<std::vector<sf::RectangleShape>> m_GridArea;
    SnakeBlock* m_Snake;

    PlayArea(sf::RectangleShape& base, int gridCount) : m_Base(base) {
        sf::Color gray(128, 128, 128, 255);
        //sf::RectangleShape* blockGrid = new sf::RectangleShape[gridCount];
        float gridSize = base.getSize().x / gridCount;
        for (int i = 0; i < gridCount; i++) {
            std::vector<sf::RectangleShape> recVec;
            for (int c = 0; c < gridCount; c++) {
                sf::RectangleShape block(sf::Vector2f(gridSize, gridSize));
                block.setFillColor(gray);
                block.setOutlineThickness(1);
                block.setOutlineColor(sf::Color::Black);
                block.setPosition(base.getPosition().x + (c * gridSize), base.getPosition().y + (i * gridSize));
                recVec.push_back(block);
                //recVec[c] = block;
                //window.draw(block);
            }
            m_GridArea.push_back(recVec);
        }
        sf::RectangleShape head({gridSize, gridSize});
        head.setFillColor(sf::Color::Yellow);
        SnakeBlock* m_Head = new SnakeBlock(head, nullptr, m_GridArea);

        sf::RectangleShape head2({ gridSize, gridSize });
        head2.setFillColor(sf::Color::Yellow);
        SnakeBlock* block3 = new SnakeBlock(head2, m_Head, m_GridArea);

        sf::RectangleShape head3({ gridSize, gridSize });
        head3.setFillColor(sf::Color::Yellow);
        m_Snake = new SnakeBlock(head3, block3, m_GridArea);
    }
    
    void onKeyPress(sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case(sf::Keyboard::Up):
                m_Snake->m_Head->m_Head->setDirection(0);
                break;
            case(sf::Keyboard::Down):
                m_Snake->m_Head->m_Head->setDirection(1);
                break;
            case(sf::Keyboard::Left):
                m_Snake->m_Head->m_Head->setDirection(3);
                break;
            case(sf::Keyboard::Right):
                m_Snake->m_Head->m_Head->setDirection(2);
                break;
            case(sf::Keyboard::A):
               /* sf::RectangleShape head2({ gridSize - 1, gridSize - 1 });
                head2.setFillColor(sf::Color::Yellow);*/
                break;
            }
        }
    }
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(m_Base, states);
        for (int i = 0; i < m_GridArea.size(); i++) {
            for (int c = 0; c < m_GridArea[i].size(); c++) {
                target.draw(m_GridArea[i][c]);
            }
        }
        target.draw(*m_Snake, states);
        m_Snake->go(Timer::getElapsedTime());
    }
};



class Scene {
public:
    float m_Width, m_Height;
    //static float m_ElapsedTime;
    sf::RenderWindow* m_Window;
    //sf::RenderWindow *window;
    Scene(float myWidth, float myHeight): m_Width(myWidth), m_Height(myHeight) {
        m_Window = new sf::RenderWindow(sf::VideoMode(m_Width, m_Height), "SFML works!");
        int gridSize = 20;
        //sf::Clock* clock = new sf::Clock();

        float edgeSize = 10;
        float baseSize = 600.0f - (edgeSize * 2);
        //sf::Vector2f((this->m_Width - baseSize) / 2)
        //float posX = (this->m_Width - baseSize) / 2;
        //float posY = (this->m_Height - baseSize) / 2;
        //float bounds[4] = {};
        sf::RectangleShape base(sf::Vector2f(baseSize, baseSize));
        sf::Color gray(128, 128, 128, 255);
        base.setFillColor(gray);
        base.setOutlineThickness(10);
        base.setOutlineColor(sf::Color::White);
        setPositionFromOrigin(base, sf::Vector2f(0, 0), sf::Vector2f(m_Width, m_Height), 1);

        PlayArea playArea(base, gridSize);

        while (m_Window->isOpen())
        {
            Timer::restart();
            sf::Event event;
            while (m_Window->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    m_Window->close();

                playArea.onKeyPress(event);

                /*if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Down)
                        gridSize -= 1;
                    else if (event.key.code == sf::Keyboard::Up) {
                        gridSize += 1;
                    }
                }*/
               
            }
           
            //std::cout << gridSize << std::endl;

            //m_ElapsedTime = clock->getElapsedTime().asSeconds();
            //clock->restart();

            m_Window->clear();
            m_Window->draw(playArea);
            //this->drawGrid(*Scene::m_Window, gridSize);
            m_Window->display();
        }
    }

    void drawGrid(sf::RenderWindow &window, int gridCount) {
        float edgeSize = 10;
        float baseSize = 600.0f - (edgeSize * 2);
        //sf::Vector2f((this->m_Width - baseSize) / 2)
        //float posX = (this->m_Width - baseSize) / 2;
        //float posY = (this->m_Height - baseSize) / 2;
        //float bounds[4] = {};
        sf::RectangleShape base(sf::Vector2f(baseSize, baseSize));
        sf::Color gray(128, 128, 128, 255);
        base.setFillColor(gray);
        base.setOutlineThickness(10);
        base.setOutlineColor(sf::Color::White);
        setPositionFromOrigin(base, sf::Vector2f(0, 0), sf::Vector2f(m_Width, m_Height), 1);

        //base.setPosition(posX, posY);
        
        //window.draw(base);

        //float gridCount = 10;
        std::vector<std::vector<sf::RectangleShape>> m_GridArea;

        sf::RectangleShape* blockGrid = new sf::RectangleShape[gridCount];
        float gridSize = baseSize / gridCount;
        for (int i = 0; i < gridCount; i++) {
            std::vector<sf::RectangleShape> recVec;
            for (int c = 0; c < gridCount; c++) {
                sf::RectangleShape block(sf::Vector2f(gridSize, gridSize));
                block.setFillColor(gray);
                block.setOutlineThickness(1);
                block.setOutlineColor(sf::Color::Black);
                block.setPosition(base.getPosition().x + (c * gridSize), base.getPosition().y + (i * gridSize));
                recVec.push_back(block);
                //window.draw(block);
            }
            m_GridArea.push_back(recVec);
        }



    }

    void setPositionFromOrigin(sf::RectangleShape& square, sf::Vector2f origin, sf::Vector2f size = sf::Vector2f(0, 0), int alignment = 0) {
        sf::Vector2f newPos = square.getPosition() + origin;
        if (alignment == 1) {
            newPos += sf::Vector2f((size.x - square.getSize().x) / 2, (size.y - square.getSize().y) / 2);
        }
        square.setPosition(newPos);
    }
};

//float Scene::m_ElapsedTime = 0;

class PositionedSquare: public sf::Drawable {
public:
    sf::RectangleShape m_Square;
    sf::Vector2f m_Origin;
    int m_Alignment;
    
    PositionedSquare(sf::RectangleShape& square, sf::Vector2f origin, int alignment = 0) : m_Square(square), m_Origin(origin), m_Alignment(alignment) {
    

    }
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
         target.draw(m_Square, states);
     }
protected:

};

int main() {
    /*sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    sf::CircleShape shape(50);
    shape.setFillColor(sf::Color::Green);
    float posx = 0,posy=275;
    sf::VertexArray lines(sf::Lines, 4);

    // Define the lines (each pair of vertices will create a line)
    lines[0].position = sf::Vector2f(100, 100);
    lines[0].color = sf::Color::Red;
    lines[1].position = sf::Vector2f(700, 100);
    lines[1].color = sf::Color::Red;

    lines[2].position = sf::Vector2f(100, 500);
    lines[2].color = sf::Color::Blue;
    lines[3].position = sf::Vector2f(700, 500);
    lines[3].color = sf::Color::Blue;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            // Check for key presses
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                else if (event.key.code == sf::Keyboard::A)
                    std::cout << "A key pressed!" << std::endl;
            }
        }
        posx += 0.1;
        shape.setPosition(posx, posy);

        window.clear();
        window.draw(lines);
        window.display();
    }*/

    Scene scene(800, 600);

    return 0;
}