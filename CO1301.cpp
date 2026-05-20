// Assessment_CO1301.cpp: A program using the TL-Engine

#include <TL-Engine.h> // TL-Engine include file and namespace
#include <cmath> // For math functions like atan2 and sqrt
#include <vector> // For dynamic arrays

using namespace tle;

// Configuration constants
const float FLOOR_Y = 0.0f;
const float SKYBOX_Y = -900.0f;
const float CAMERA_Y = 25.0f;
const float CAMERA_Z = -65.0f;
const float CAMERA_ROTATION_X = 10.0f;
const int NUM_BARRIERS = 10;
const float BARRIER_SPACING = 10.0f;
const float BARRIER_LEFT_X = -70.0f;
const float BARRIER_RIGHT_X = 60.0f;
const float WALL_Z = 120.0f;
const int NUM_BLOCKS = 10;
const float BLOCK_WIDTH = 10.0f;
const float BLOCK_GAP = 4.0f;
const float MARBLE_RADIUS = 4.0f / 2;
const float MARBLE_INITIAL_Y = 2.0f;
const float MARBLE_SPEED = 50.0f;
const float OUT_OF_BOUNDS_Z = 200.0f;
const float OUT_OF_BOUNDS_X = 100.0f;

int main()
{
    // Create a 3D engine (using TLX engine here) and open a window for it
    I3DEngine* myEngine = New3DEngine(kTLX);
    myEngine->StartWindowed();

    // Add default folder for meshes and other media
    myEngine->AddMediaFolder(".\\Media");

    /**** Set up your scene here ****/
    IMesh* floorMesh = myEngine->LoadMesh("Floor.x");
    IMesh* barrierMesh = myEngine->LoadMesh("Barrier.x");
    IMesh* skyboxMesh = myEngine->LoadMesh("Skybox_Hell.x");
    IMesh* blockMesh = myEngine->LoadMesh("Block.x");
    IMesh* marbleMesh = myEngine->LoadMesh("Marble.x");
    IMesh* arrowMesh = myEngine->LoadMesh("Arrow.x");

    // Create models
    IModel* floorModel = floorMesh->CreateModel();
    IModel* skyboxModel = skyboxMesh->CreateModel(0, SKYBOX_Y, 0);

    // Create barriers
    for (int i = 0; i < NUM_BARRIERS; ++i)
    {
        float zPosition = i * BARRIER_SPACING;
        barrierMesh->CreateModel(BARRIER_LEFT_X, FLOOR_Y, zPosition);
        barrierMesh->CreateModel(BARRIER_RIGHT_X, FLOOR_Y, zPosition);
    }

    // Create camera
    ICamera* camera = myEngine->CreateCamera(kManual);
    camera->SetPosition(0, CAMERA_Y, CAMERA_Z);
    camera->RotateX(CAMERA_ROTATION_X);

    // Create wall of blocks
    float wallStartX = -(NUM_BLOCKS * (BLOCK_WIDTH + BLOCK_GAP) - BLOCK_GAP) / 2;
    std::vector<IModel*> blockModels(NUM_BLOCKS);
    std::vector<bool> blockHit(NUM_BLOCKS, false);

    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        float xPosition = wallStartX + i * (BLOCK_WIDTH + BLOCK_GAP);
        blockModels[i] = blockMesh->CreateModel(xPosition, BLOCK_WIDTH / 2, WALL_Z);
    }

    // Create marble and dummy node
    IModel* marbleModel = marbleMesh->CreateModel(0, MARBLE_INITIAL_Y, 0);
    IModel* dummyNode = marbleMesh->CreateModel(0, MARBLE_INITIAL_Y, 0);

    // Create arrow
    IModel* arrowModel = arrowMesh->CreateModel(0, 0, -10);
    arrowModel->AttachToParent(dummyNode);
    arrowModel->SetSkin("lava_rock.jpg"); // Fully transparent texture

    // Targeting system variables
    int currentTargetIndex = 0;
    bool marbleLaunched = false;
    float launchDirectionX = 0.0f, launchDirectionZ = 0.0f;

    // Reset game function
    auto resetGame = [&]() {
        marbleModel->SetPosition(0, MARBLE_INITIAL_Y, 0);
        marbleLaunched = false;
        for (int i = 0; i < NUM_BLOCKS; i++)
        {
            blockModels[i]->SetSkin("Default.x");
            blockHit[i] = false;
        }
        currentTargetIndex = 0;
        dummyNode->ResetOrientation();
        };

    // The main game loop, repeat until engine is stopped
    while (myEngine->IsRunning())
    {
        //Draw the scene
        myEngine->DrawScene();

        /**** Update your scene each frame here ****/
        // Handle user input
        if (!marbleLaunched)
        {
            if (myEngine->KeyHit(Key_Left))
            {
                currentTargetIndex = (currentTargetIndex - 1 + NUM_BLOCKS) % NUM_BLOCKS;
            }
            if (myEngine->KeyHit(Key_Right))
            {
                currentTargetIndex = (currentTargetIndex + 1) % NUM_BLOCKS;
            }
            if (myEngine->KeyHit(Key_Space))
            {
                marbleLaunched = true;
                float dx = blockModels[currentTargetIndex]->GetX() - marbleModel->GetX();
                float dz = blockModels[currentTargetIndex]->GetZ() - marbleModel->GetZ();
                float magnitude = sqrt(dx * dx + dz * dz);
                launchDirectionX = dx / magnitude;
                launchDirectionZ = dz / magnitude;
            }

            // Rotate dummy node to aim at selected target
            float dx = blockModels[currentTargetIndex]->GetX() - marbleModel->GetX();
            float dz = blockModels[currentTargetIndex]->GetZ() - marbleModel->GetZ();
            float angle = atan2(dx, dz) * 180.0f / 3.141592653589793f;
            dummyNode->ResetOrientation();
            dummyNode->RotateY(angle);
        }

        // Move marble if launched
        if (marbleLaunched)
        {
            float moveStep = MARBLE_SPEED * myEngine->Timer();
            marbleModel->Move(moveStep * launchDirectionX, 0, moveStep * launchDirectionZ);

            // Check for collisions with blocks
            for (int i = 0; i < NUM_BLOCKS; i++)
            {
                if (!blockHit[i])
                {
                    float dx = marbleModel->GetX() - blockModels[i]->GetX();
                    float dz = marbleModel->GetZ() - blockModels[i]->GetZ();
                    float distance = sqrt(dx * dx + dz * dz);

                    if (distance < (MARBLE_RADIUS + BLOCK_WIDTH / 2))
                    {
                        blockModels[i]->SetSkin("tiles_red.jpg");
                        blockHit[i] = true;
                        marbleModel->SetPosition(0, MARBLE_INITIAL_Y, 0);
                        marbleLaunched = false;
                        break;
                    }
                }
            }

            // Reset marble if out of bounds
            if (marbleModel->GetZ() > OUT_OF_BOUNDS_Z || abs(marbleModel->GetX()) > OUT_OF_BOUNDS_X)
            {
                marbleModel->SetPosition(0, MARBLE_INITIAL_Y, 0);
                marbleLaunched = false;
            }
        }

        // Reset game if R is pressed
        if (myEngine->KeyHit(Key_R))
        {
            resetGame();
        }

        // Highlight selected target
        for (int i = 0; i < NUM_BLOCKS; i++)
        {
            if (i == currentTargetIndex)
            {
                blockModels[i]->SetSkin("Highlight.x");
            }
            else if (!blockHit[i])
            {
                blockModels[i]->SetSkin("Default.x");
            }
        }
    }

    // Delete the #D engine now we are finished with it
    myEngine->Delete();
    return 0;
}
