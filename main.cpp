#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>

using namespace sf;
using namespace std;

int DijkstraVisitedNode= 0 ;
int A_starVisitedNode= 0 ;
int IsDijkstra=0;
unsigned int windowWidth=950;
unsigned int windowHeight=950;

constexpr unsigned int numBlockX{ 25};
constexpr unsigned int numBlockY{ 25 };

float blockWidth{ windowWidth / numBlockX };
float blockHeight{ windowHeight / numBlockY };

float outlineThickness{ -2.f };

int horisontalAndVerticalCost{ 10 };
int diagonalCost{ 14 };

class Coordinate2D
{
public:
    Coordinate2D() {};
    Coordinate2D(int x, int y) : m_x(x), m_y(y) {};

    int x() const { return m_x; }
    int y() const { return m_y; }

private:
    int m_x;
    int m_y;
};

inline bool operator==(const Coordinate2D& lhs, const Coordinate2D& rhs) {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

inline Coordinate2D operator+(Coordinate2D& lhs, const Coordinate2D& rhs) {
    return Coordinate2D(lhs.x() + rhs.x(), lhs.y() + rhs.y());
}

struct equalFunc
{
    bool operator()(const Coordinate2D& lhs, const Coordinate2D& rhs) const
    {
        return lhs.x() == rhs.x() && lhs.y() == rhs.y();
    }
};

struct hashFunc
{
    size_t operator()(const Coordinate2D& coordinate) const
    {
        size_t h1 = std::hash<int>()(coordinate.x());
        size_t h2 = std::hash<int>()(coordinate.y());

        return h1 ^ (h2 << 1);
    }
};

struct Node
{
    int G = 0;
    int H = 0;
    Coordinate2D m_coordinates;
    Node* m_parent;
    int F() const { return G + H; };

    Node(const Coordinate2D& coordinates, Node* parent = nullptr) : m_coordinates(coordinates), m_parent(parent) {};
};

inline bool operator<(const Node& lhs, const Node& rhs)
{
    return make_pair(lhs.m_coordinates.x(), lhs.m_coordinates.y()) < make_pair(rhs.m_coordinates.x(), rhs.m_coordinates.y());
}

struct Block
{
    RectangleShape m_shape;
    Block() {};
    Block(const Block& other) : Block(static_cast<int>(other.m_shape.getPosition().x / blockWidth), static_cast<int>(other.m_shape.getPosition().y / blockHeight), other.m_shape.getFillColor(), other.m_shape.getOutlineColor()) {};

    Block(Color fillColor, Color outlineColor)
    {

        m_shape.setSize({ blockWidth, blockHeight });
        m_shape.setFillColor(fillColor);
        m_shape.setOutlineColor(outlineColor);
        m_shape.setOutlineThickness(outlineThickness);
    }

    Block(int x, int y, Color fillColor, Color outlineColor ) : Block(fillColor, outlineColor)
    {
        m_shape.setPosition(x * blockWidth, y * blockHeight);
        m_shape.setOrigin(0.f, 0.f);
    }
};

using BlockMap = unordered_map<Coordinate2D, Block, hashFunc, equalFunc>;
using BlockPair = pair<Coordinate2D, Block>;

BlockMap gridBlocks;
BlockMap wallBlocks;
BlockMap pathBlocks;
BlockPair sourceBlock;
BlockPair targetBlock;

set<Node*> openSet;
set<Node*> closedSet;
Node* current;
vector<Coordinate2D> path;
bool running = false;
bool isTargetFound = false;

const vector<Coordinate2D> directions = { { 0, 1 },{ 1, 0 },{ 0, -1 },{ -1, 0 },{ -1, -1 },{ 1, 1 },{ -1, 1 },{ 1, -1 } };
sf::View calcView(const sf::Vector2u &windowsize, const sf::Vector2u &designedsize)
{
    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    float screenwidth = windowsize.x / static_cast<float>(designedsize.x);
    float screenheight = windowsize.y / static_cast<float>(designedsize.y);

    if (screenwidth > screenheight)
    {
        viewport.width = screenheight / screenwidth;
        viewport.left = (1.f - viewport.width) / 2.f;
    }
    else if (screenwidth < screenheight)
    {
        viewport.height = screenwidth / screenheight;
        viewport.top = (1.f - viewport.height) / 2.f;
    }

    sf::View view(sf::FloatRect(0.f, 0.f, static_cast<float>(designedsize.x), static_cast<float>(designedsize.y)));
    view.setViewport(viewport);

    return view;
}

void createGrid()
{
    for (int x = 0; x < numBlockX; ++x)
    {
        for (int y = 0; y < numBlockY; ++y)
        {
            //Assign each block with its property
            //And insert it in gridBlock
            Block block({ x, y, Color::White, Color{ 128, 128, 128 } });
            gridBlocks.insert({ Coordinate2D(x, y), block });
        }
    }
}

bool traversable(const Coordinate2D coordinate)
{
    if (wallBlocks.count(coordinate)
        || coordinate.x() < 0
        || coordinate.x() >= numBlockX
        || coordinate.y() < 0
        || coordinate.y() >= numBlockY)
    {
        return false;
    }

    return true;
}

int manhattan(const Coordinate2D& source, const Coordinate2D& target)
{
    const int deltaX = abs(source.x() - target.x());
    const int deltaY = abs(source.y() - target.y());

    return 10 * static_cast<int>(deltaX + deltaY);
}

void explorePath()
{
    if (IsDijkstra) DijkstraVisitedNode ++;
    else A_starVisitedNode ++;
    if (!openSet.empty())
    {
        current = *openSet.begin();

        // Find node in open with lowest f
        for (Node* node : openSet)
        {
            if (node->F() <= current->F())
                current = node;
        }
       //Insert it in the closedSet i,e, visited node
        closedSet.insert(current);
        openSet.erase(find(openSet.begin(), openSet.end(), current));

        // Path found
        if (current->m_coordinates == targetBlock.first)
            isTargetFound = true;

        // Explore neighbours
        for (size_t i = 0; i < directions.size(); ++i)
        {
            Coordinate2D neighbourPos{ current->m_coordinates + directions[i] };
             //lambda funtion
            auto itClosed = find_if(closedSet.begin(), closedSet.end(), [&neighbourPos](const Node* node)
            {
                return node->m_coordinates == neighbourPos;
            });
        //itClosed != closedSet.end() is false if the neighbour node is already visited
            if (!traversable(neighbourPos) || itClosed != closedSet.end())
                continue;
          //Goto to next neighbour

            //Diagonal direction come after index 3
            int G = current->G + ((i < 4) ? horisontalAndVerticalCost : diagonalCost);

            auto itOpen = find_if(openSet.begin(), openSet.end(), [&neighbourPos](const Node* node)
            {
                return node->m_coordinates == neighbourPos;
            });
            //when the below condition is true new neighbour is encountered
            if (itOpen == openSet.end())
            {
                Node* neighbour = new Node(neighbourPos, current);
                neighbour->G = G;
                if (IsDijkstra) neighbour->H = 0 ;
                else neighbour->H = manhattan(neighbour->m_coordinates, targetBlock.first);
                openSet.insert(neighbour);
            }
            //else it was already in the list to be visited , it is far form the source than the current node => current is the parent of that node
            else if (G < (*itOpen)->G)
            {
                (*itOpen)->m_parent = current;
                (*itOpen)->G = G;
            }
        }
    }

    // if target is found trace back the path to the source
    if (isTargetFound)
    {
        while (current != nullptr)
        {
            path.push_back(current->m_coordinates);
            current = current->m_parent;
        }
    }
}

void clearMemory()
{
    for (auto it = openSet.begin(); it != openSet.end();) {
        delete *it;
        it = openSet.erase(it);
    }

    for (auto it = closedSet.begin(); it != closedSet.end();) {
        delete *it;
        it = closedSet.erase(it);
    }
}

void handleInput(RenderWindow& window, const sf::Vector2u designedsize)
{
    Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
        case Event::Closed:
        {
            window.close();
            break;
        }
        case Event::KeyPressed:
        {
            if (event.key.code == Keyboard::R)
            {
                running = true;
            }
            else if (event.key.code == Keyboard::C)
            {
                clearMemory();

                wallBlocks.clear();
                pathBlocks.clear();
                sourceBlock = BlockPair();
                targetBlock = BlockPair();
                openSet.clear();
                closedSet.clear();
                current = nullptr;
                path.clear();
                running = false;
                isTargetFound = false;
                if(IsDijkstra) DijkstraVisitedNode = 0 ;
                else A_starVisitedNode =0;
            }

            else if (event.key.code == Keyboard::Escape)
            {
                window.close();
            }
            else if (event.key.code == Keyboard::S || event.key.code == Keyboard::T)
            {
                Vector2i mousePos = Mouse::getPosition(window);
                const int x = static_cast<int>(mousePos.x / blockWidth);
                const int y = static_cast<int>(mousePos.y / blockHeight);
                Coordinate2D coord(Coordinate2D(x, y));
               auto it = gridBlocks.find(coord);
               //Check if "it" is in the gridBlock and if the mouse coordinate is inside the window
                if (it != gridBlocks.end() && it->second.m_shape.getGlobalBounds().contains(window.mapPixelToCoords(mousePos)))
                {
                    if (event.key.code == Keyboard::S)
                    {
                        Block block({ x, y, Color{252, 36, 3}, Color::Black });
                        sourceBlock = { Coordinate2D(x, y), block };
                        openSet.clear();
                        openSet.insert(new Node(Coordinate2D(x, y)));
                    }
                    else if (event.key.code == Keyboard::T)
                    {
                        Block block({ x, y, Color{153, 255, 51}, Color::Black});
                        targetBlock = { Coordinate2D(x, y), block };
                    }
                }
            }
            break;
        }
        case Event::Resized:
        {
            window.setView(calcView(sf::Vector2u(event.size.width, event.size.height), designedsize));
            break;
        }
        default:
            break;
        }
    }
   // Toggling between the two algorithm i.e. A* (star) And Dijkstra
     if (event.key.code == Keyboard::D)
            {
                clearMemory();
                pathBlocks.clear();
                openSet.clear();
                openSet.insert(new Node(sourceBlock.first));
                closedSet.clear();
                current = nullptr;
                path.clear();
                running = false;
                isTargetFound = false;
                DijkstraVisitedNode = 0 ;
                IsDijkstra =1;
            }

    if (event.key.code == Keyboard::Z)
            {
                clearMemory();
                pathBlocks.clear();
                openSet.clear();
                openSet.insert(new Node(sourceBlock.first));
                closedSet.clear();
                current = nullptr;
                path.clear();
                running = false;
                isTargetFound = false;
                A_starVisitedNode = 0 ;
                IsDijkstra =0;
            }

    // Creating Wall
    if (Keyboard::isKeyPressed(Keyboard::Key::W))
    {
        Vector2i mousePos = Mouse::getPosition(window);
        const int x = static_cast<int>(mousePos.x / blockWidth);
        const int y = static_cast<int>(mousePos.y / blockHeight);
        Coordinate2D coord(Coordinate2D(x, y));
        auto it = gridBlocks.find(coord);
        if (it != gridBlocks.end() && it->second.m_shape.getGlobalBounds().contains(window.mapPixelToCoords(mousePos)))
        {
            Block block({ x, y, Color{183,50,57}, Color{ 128, 128, 128 } });
            wallBlocks.insert({ Coordinate2D(x, y), block });
        }

    }
   // Removing Wall
    if (Keyboard::isKeyPressed(Keyboard::Key::A))
    {
        Vector2i mousePos = Mouse::getPosition(window);
        const int x = static_cast<int>(mousePos.x / blockWidth);
        const int y = static_cast<int>(mousePos.y / blockHeight);
        Coordinate2D coord(Coordinate2D(x, y));
        auto it1 = wallBlocks.find(coord);
        if (it1 != wallBlocks.end() && it1->second.m_shape.getGlobalBounds().contains(window.mapPixelToCoords(mousePos)))
        {
            Block block({ x, y, Color::White, Color{ 128, 128, 128 } });
            wallBlocks.erase(it1);
        }

    }



}
//Draw once the computation is done
void drawScene(RenderWindow& window)
{
    for (const auto& block : gridBlocks)
    {
        window.draw(block.second.m_shape);
    }

    for (const auto& block : wallBlocks)
    {
        window.draw(block.second.m_shape);
    }

    for (const Node* node : closedSet)
    {
        Block block({ node->m_coordinates.x(), node->m_coordinates.y(), Color{51, 153, 255}, Color::Black });
        window.draw(block.m_shape);
    }

    if (current)
    {
        Block block{ current->m_coordinates.x(), current->m_coordinates.y(), Color{46,46,184}, Color::Black };
        window.draw(block.m_shape);
    }

    for (const Coordinate2D& coordinate : path)
    {
        Block block{ coordinate.x(), coordinate.y(), Color{46,46,184}, Color::Black };
        window.draw(block.m_shape);
    }

    window.draw(sourceBlock.second.m_shape);
    window.draw(targetBlock.second.m_shape);
}

int main()
{
    const sf::Vector2u designedsize(950, 950);

    RenderWindow window({ windowWidth, windowHeight }, "A* VS Dijkstra");
    window.setView(calcView(window.getSize(), designedsize));
    window.setFramerateLimit(60);

    Clock clock;

    createGrid();

    // Window Loop
    while (window.isOpen())
    {

        window.clear();

        handleInput(window, designedsize);

        if (running && !isTargetFound)
        {
            // Delay for Visualization
            Time time = clock.getElapsedTime();
            bool timePassed = time.asMilliseconds() > 100.f;
            if (timePassed)
            {
                explorePath();
                clock.restart();
            }
        }

        drawScene(window);

        window.display();
    }

    clearMemory();


   cout<< "A* (star)  Visited Node : " << A_starVisitedNode<<endl;
   cout<< "Dijkstra Visited Node : "<< DijkstraVisitedNode<<endl;
    return 0;
}
