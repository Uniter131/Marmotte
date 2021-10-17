#include <bangtal>
#include <string>
#include <random>

using namespace bangtal;
using namespace std;

auto mainScene = Scene::create("", "Images/BackGround.png");

enum Type {
    MEDICINE = 12,
    OXYGEN_TANK = 14,
    TOXIC_BARREL = 19,
    ZOMBIE = 25
};

class Board {

public:
    ObjectPtr objects[9];
    ObjectPtr dropItem[2];
    ObjectPtr gun = Object::create("Images/LaserGun.png", mainScene, 0, 0, false);

    int xPos[3] = { 400, 600, 800 };
    int yPos[3] = { 60, 260, 460 };
    int nextOxy[8] = { 8, 6, 7, 5, 0, 2, 3, 1 };
    string typeIndex = "smmzzztto012345678";

    void GenerateObjects();
    void swapObjects(int n1, int n2);
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

    dropItem[0] = Object::create("Images/GunEngine.png", mainScene, 0, 0, false);
    dropItem[1] = Object::create("Images/Wire.png", mainScene, 0, 0, false);

    gun->defineCombination(dropItem[0], dropItem[1]);

    RandomSetting(0, 8);
}

void Board::swapObjects(int n1, int n2) {
    objects[typeIndex[n1 + 9] - '0']->locate(mainScene, xPos[n2 % 3], yPos[2 - n2 / 3]);
    objects[typeIndex[n2 + 9] - '0']->locate(mainScene, xPos[n1 % 3], yPos[2 - n1 / 3]);

    char temp = typeIndex[n1];
    typeIndex[n1] = typeIndex[n2];
    typeIndex[n2] = temp;
    temp = typeIndex[n1 + 9];
    typeIndex[n1 + 9] = typeIndex[n2 + 9];
    typeIndex[n2 + 9] = temp;
}

void Board::RandomSetting(int playerTile, int oxygenTankTile) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 8);

    int randNum = 0;  int index = 0;

    while (index <= 8) {
        randNum = dis(gen);
        if (randNum != playerTile && randNum != oxygenTankTile && randNum != index) {
            if (index != playerTile && index != oxygenTankTile) {
                swapObjects(index, randNum);
                index++;
            }
            else {
                index++;
            }
        }
    }
}

class PlayerState: Board {

public:
    ObjectPtr oxy, inf;
    ObjectPtr oxy_bar, inf_bar, gun_bar;

    int oxygen = 10, infect = 0; 
    int item = 0, dropItemIndex = 1;
    int x = 400, y = 460;
    int currentTile = 0;
    bool isArmed = true;

    void GenerateStates();
    void InteractObject(Board b, int type);
    void getItem(Board b);
};

void PlayerState::GenerateStates() {
    inf = Object::create("Images/Infection.png", mainScene, 80, 600);
    oxy = Object::create("Images/OxygenTank.png", mainScene, 80, 500);
    oxy->setScale(0.5f);

    inf_bar = Object::create("Images/Infect0.png", mainScene, 180, 600);
    oxy_bar = Object::create("Images/Oxygen10.png", mainScene, 180, 500);

}

void PlayerState::getItem(Board b) {
    if (item <= 3) {
        if (dropItemIndex) {
            b.dropItem[--dropItemIndex]->pick();
            item++;
        }
        else {
            b.dropItem[++dropItemIndex]->pick();
            item++;
        }
    }
    else showMessage("최대 소지 가능한 아이템을 초과했습니다.");
}

void PlayerState::InteractObject(Board b, int type) {

    oxy_bar->setImage("Images/Oxygen" + to_string(--oxygen) + ".png");

    switch (type) {
        case MEDICINE:
            infect = 0;
            inf_bar->setImage("Images/Infect" + to_string(infect) + ".png");
            break;
        case ZOMBIE:
            if (!isArmed) {
                infect += 1;
                inf_bar->setImage("Images/Infect" + to_string(infect) + ".png");
            }
            else {
                isArmed = false;
                b.objects[0]->setImage("Images/Scientist.png");
                b.gun->drop();
                b.gun->hide();
            }
            getItem(b);
            break;
        case TOXIC_BARREL:
            infect += 1;
            inf_bar->setImage("Images/Infect" + to_string(infect) + ".png");
            break;
        case OXYGEN_TANK:
            oxygen += 4;
            if (oxygen > 10) {
                oxygen = 10;
            }
            else {
                oxy->locate(mainScene, xPos[b.nextOxy[currentTile] % 3], yPos[2 - b.nextOxy[currentTile] / 3]);
                b.RandomSetting(currentTile, b.nextOxy[currentTile]);
            }
            oxy_bar->setImage("Images/" + to_string(oxygen) + ".png");
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
    board.gun->pick();
    printf("%s\n", board.typeIndex.c_str());

    mainScene->setOnKeyboardCallback([&](auto scene, auto keyCode, auto isPressed)->bool {
        printf("%s\n", board.typeIndex.c_str());
        if (isPressed != false) {
            switch (keyCode) {
            case KeyCode::KEY_LEFT_ARROW:
                if (p.x > board.xPos[0]) {
                    p.x -= 200;
                    p.InteractObject(board, board.typeIndex[--p.currentTile] - 'a');
                    board.swapObjects(p.currentTile, p.currentTile + 1);
                }
                break;
            case KeyCode::KEY_RIGHT_ARROW:
                if (p.x < board.xPos[2]) {
                    p.x += 200;
                    p.InteractObject(board, board.typeIndex[++p.currentTile] - 'a');
                    board.swapObjects(p.currentTile, p.currentTile - 1);
                }
                break;
            case KeyCode::KEY_UP_ARROW:
                if (p.y < board.yPos[2]) {
                    p.y += 200;
                    p.currentTile -= 3;
                    p.InteractObject(board, board.typeIndex[p.currentTile] - 'a');
                    board.swapObjects(p.currentTile, p.currentTile + 3);
                }
                break;
            case KeyCode::KEY_DOWN_ARROW:
                if (p.y > board.yPos[0]) {
                    p.y -= 200;
                    p.currentTile += 3;
                    p.InteractObject(board, board.typeIndex[p.currentTile] - 'a');
                    board.swapObjects(p.currentTile, p.currentTile - 3);
                }
                break;
            }
        }
        return true;
    });

    board.gun->setOnCombineCallback([&](auto objcet)->bool {
        board.objects[0]->setImage("Images/ArmedScientist.png");
        return true;
    });

    startGame(mainScene);
    return 0;
}