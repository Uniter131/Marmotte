#include <bangtal>
#include <string>
#include <random>

using namespace bangtal;
using namespace std;

auto mainScene = Scene::create("", "Images/BackGround.png");

enum Type {
    PLAYER = 1,
    MEDICINE,
    ZOMBIE,
    TOXIC_BARREL,
    OXYGEN_TANK
};

class Board {
    int xPos[3] = { 400, 600, 800 };
    int yPos[3] = { 60, 260, 460 };
public:
    static ObjectPtr objects[9];
    string typeIndex = "smmzzztto012345678";

    void GenerateObjects();
    void RandomSetting(int playerTile, int oxygenTankTile);
};

void Board::GenerateObjects() {

    objects[0] = Object::create("Images/ArmedScientist.png", mainScene, xPos[0], yPos[2]);
    objects[1] = Object::create("Images/Medicine.png", mainScene, xPos[1], yPos[2]);
    objects[2] = Object::create("Images/Medicine.png", mainScene, xPos[2], yPos[2]);
    objects[3] = Object::create("Images/Zombie.png", mainScene, xPos[0], yPos[1]);
    objects[4] = Object::create("Images/Zombie.png", mainScene, xPos[1], yPos[1]);
    objects[5] = Object::create("Images/Zombie.png", mainScene, xPos[2], yPos[1]);
    objects[6] = Object::create("Images/ToxicBarrel.png", mainScene, xPos[0], yPos[0]);
    objects[7] = Object::create("Images/ToxicBarrel.png", mainScene, xPos[1], yPos[0]);
    objects[8] = Object::create("Images/OxygenTank.png", mainScene, xPos[2], yPos[0]);

    RandomSetting();
}

void Board::RandomSetting(int playerTile, int oxygenTankTile) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 8);

    char temp;
    int randNum = 0;  int index = 0;
    while (index <= 8) {
        if (randNum != playerTile && randNum != oxygenTankTile) {
            if (index != playerTile && index != oxygenTankTile) {
                objects[index]->locate(mainScene, xPos[randNum % 3], yPos[randNum / 3]);
                objects[randNum]->locate(mainScene, xPos[index % 3], yPos[index / 3]);
                temp = typeIndex[index];
                typeIndex[index] = typeIndex[randNum];
                typeIndex[randNum] = temp;
                index++;
            }
            else {
                index++;
            }
        }
    }
    
}

class PlayerState {

public:
    ObjectPtr oxy, inf, gun;
    ObjectPtr oxy_bar, inf_bar, gun_bar;

    int oxygen = 10;
    int infect = 0;
    int gunDurability = 5;
    int x = 400, y = 460;
    int currentTile = 0;
    bool isArmed = true;

    void GenerateStates();
    void InteractObject(char type);
};

void PlayerState::GenerateStates() {
    inf = Object::create("Images/Infection.png", mainScene, 80, 600);
    oxy = Object::create("Images/OxygenTank.png", mainScene, 80, 500);
    oxy->setScale(0.5f);
    gun = Object::create("Images/LaserGun.png", mainScene, 80, 400);
    gun->setScale(0.5f);

    inf_bar = Object::create("Images/Infect0.png", mainScene, 180, 600);
    oxy_bar = Object::create("Images/Oxygen10.png", mainScene, 180, 500);
    gun_bar = Object::create("Images/Durability5.png", mainScene, 180, 400);
}

void PlayerState::InteractObject(char type) {

    switch (type) {
        case 'm':
            infect = 0;
            gun_bar->setImage("Images/" + infect);
            break;
        case 'z':
            if (!isArmed) {
                infect += 1;
                gun_bar->setImage("Images/" + infect);
            }
            else {
                if (--gunDurability <= 0) {
                    isArmed = false;
                    Board::objects[0]->setImage("Images/Scientist.png");
                }
            }
            break;
        case 't':
            infect += 1;
            gun_bar->setImage("Images/" + infect);
            break;
        case 'o':
            oxygen += 4;
            if (oxygen > 10) {
                oxygen = 10;
                oxy_bar->setImage("Images/" + oxygen);
            }
            break;
        default:
            showMessage("알 수 없는 타입 존재");
    }
}

int main()
{
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    Board board; PlayerState p;
    board.GenerateObjects();
    p.GenerateStates();

    mainScene->setOnKeyboardCallback([&](auto scene, auto keyCode, auto isPressed)->bool {
        if (isPressed != false) {
            switch (keyCode) {
            case KeyCode::KEY_LEFT_ARROW:
                if (p.x > 400) {
                    board.objects[0]->locate(scene, p.x -= 200, p.y);
                    board.objects[stoi(typeIndex[--currentTile + 9])];
                    InteractObject(typeIndex[--currentTile]);
                }
                break;
            case KeyCode::KEY_RIGHT_ARROW:
                if (p.x < 800) {
                    board.objects[0]->locate(scene, p.x += 200, p.y);
                    board.objects[stoi(typeIndex[--currentTile + 9])];
                    InteractObject(typeIndex[--currentTile]);
                }
                break;
            case KeyCode::KEY_UP_ARROW:
                if (p.y < 460) {
                    board.objects[0]->locate(scene, p.x, p.y += 200);
                    board.objects[stoi(typeIndex[--currentTile + 9])];
                    InteractObject(typeIndex[--currentTile]);
                }
                break;
            case KeyCode::KEY_DOWN_ARROW:
                if (p.y > 60) {
                    board.objects[0]->locate(scene, p.x, p.y -= 200);
                    board.objects[stoi(typeIndex[--currentTile + 9])];
                    InteractObject(typeIndex[--currentTile]);
                }
                break;
            }
        }
        return true;
    });
    startGame(mainScene);
    return 0;
}