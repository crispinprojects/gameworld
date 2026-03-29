#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <set>
#include <random>
#include <string>
#include <queue>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <sstream>

// include all header files
#include "spider.h"
#include "vector3.h"
#include "tree.h"
#include "goldpiece.h"
#include "diamondpiece.h"
#include "tower.h"
#include "treasuretower.h"
#include "slime.h"
#include "wasp.h"

#include "synthesizer.h"
#include "audio_data.h"  // audio data

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global constants for game physics and gameplay
const float BULLET_RADIUS = 0.1f;                    // Radius of player bullets
const float PLAYER_PUSH_AWAY_DISTANCE = 2.5f;       // Distance to push player back when hitting tower
const float SLIME_PUSH_AWAY_DISTANCE = 12.0f;       // Distance to push slime away from towers
const float SLIME_TOWER_DISTANCE = 1.0f;            // Minimum distance slimes can get to tower edges
const float JUMP_DISTANCE_RANGE = 6.0f;             // Range of distances from cube side at which player can jump

const float SLIME_CHASE_DISTANCE = 10.0f;            // Distance at which slimes start chasing
const float SPIDER_CHASE_DISTANCE = 20.0f;          // Distance at which spiders start chasing

const float WASP_HIT_RADIUS = 1.4f;                  // Radius for wasp collision detection

// Global variables
float slimeAnimation = 0.0f;
bool showRadar = true;
float radarScale = 0.05f;                           // Scale for radar display
float spiderLegAnimation = 0.0f;                    // Animation counter for spider legs
int frameCount = 0;                                 // Frame counter for FPS calculation
int lastTime = 0;                                   // Last time for FPS calculation
float fps = 0.0f;                                   // Current FPS value

// Standard collision detection for game entities
bool checkCollision(const Vector3& pos1, const Vector3& pos2, float radius1, float radius2) {
    float dist = distance(pos1, pos2);
    return dist < (radius1 + radius2);
}

// Special range-based collision detection for tower jumping (larger radius)
bool checkTowerJumpCollision(const Vector3& playerPos, const Vector3& towerPos, float towerRadius, float jumpRange) {
    float dx = playerPos.x - towerPos.x;
    float dz = playerPos.z - towerPos.z;
    float distance = sqrt(dx*dx + dz*dz);
    return distance < (towerRadius + jumpRange);
}

// Check if point is inside a cube
bool isPointInCube(const Vector3& point, const Vector3& cubePos, float width, float height, float depth) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;
    
    return (point.x >= cubePos.x - halfWidth && point.x <= cubePos.x + halfWidth &&
            point.y >= cubePos.y - halfHeight && point.y <= cubePos.y + halfHeight &&
            point.z >= cubePos.z - halfDepth && point.z <= cubePos.z + halfDepth);
}

// Check if slime head is inside a tower cube
bool isSlimeHeadInTower(const Vector3& slimePos, const Vector3& towerPos, float towerWidth, float towerHeight, float towerDepth, float slimeRadius) {
    // Create a bounding box for the slime head
    float halfWidth = towerWidth / 2.0f;
    float halfHeight = towerHeight / 2.0f;
    float halfDepth = towerDepth / 2.0f;
    
    // Check if slime head position is inside the tower cube
    return (slimePos.x >= towerPos.x - halfWidth + slimeRadius && 
            slimePos.x <= towerPos.x + halfWidth - slimeRadius &&
            slimePos.y >= towerPos.y - halfHeight + slimeRadius && 
            slimePos.y <= towerPos.y + halfHeight - slimeRadius &&
            slimePos.z >= towerPos.z - halfDepth + slimeRadius && 
            slimePos.z <= towerPos.z + halfDepth - slimeRadius);
}

// Check if two slimes are colliding
bool checkSlimeCollision(const Vector3& pos1, const Vector3& pos2, float radius1, float radius2) {
    float dist = distance(pos1, pos2);
    return dist < (radius1 + radius2);
}

//======================================================================
// AUDIO MANAGER
//======================================================================

bool audioEnabled = true; // Toggle for audio on/off

class SimplePhraseBuilder {
public:
    static void buildPhrase(const std::vector<VoiceWord>& words, std::vector<int16_t>& buffer) {
        extern int predicted_sample;
        extern int step_index;
        
        for (VoiceWord word : words) {
            // Reset decoder state
            predicted_sample = 0;
            step_index = 0;
            
            AudioLookup lookup = voice_dictionary[word];
            for (uint32_t i = 0; i < lookup.length; i++) {
                uint8_t byte = lookup.data[i];
                buffer.push_back(decode_adpcm_nibble(byte & 0x0F));
                buffer.push_back(decode_adpcm_nibble((byte >> 4) & 0x0F));
            }
            
            // Add silence between words
            for (int s = 0; s < 1600; s++) {
                buffer.push_back(0);
            }
        }
    }
};

#include <atomic>
#include <thread>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib> // for system()

class GameAudioManager {
private:
    std::atomic<bool> is_playing{false};
    std::atomic<bool> bluetoothEnabled{false}; // New atomic flag

public:
    GameAudioManager() = default;
    ~GameAudioManager() = default;

    void playPhraseAsync(const std::vector<VoiceWord>& words) {
        if (is_playing.load()) {
            return; // Already playing, skip
        }

        std::thread([this, words]() {
            is_playing.store(true);

            std::vector<int16_t> full_audio;
            SimplePhraseBuilder::buildPhrase(words, full_audio);

            std::ofstream file("temp_audio.wav", std::ios::binary);
            if (file.is_open()) {
                write_wav_header(file, static_cast<int>(full_audio.size()), SAMPLE_RATE);
                file.write(reinterpret_cast<const char*>(full_audio.data()),
                           full_audio.size() * sizeof(int16_t));
                file.close();

                // Choose command based on bluetooth flag
                std::string cmd;
                if (bluetoothEnabled.load()) {
                    cmd = "aplay -D bluealsa temp_audio.wav > /dev/null 2>&1";
                } else {
                    cmd = "aplay temp_audio.wav > /dev/null 2>&1";
                }

                system(cmd.c_str());
            }

            is_playing.store(false);
        }).detach();
    }

    void playImmediatePhrase(const std::vector<VoiceWord>& words) {
        std::vector<int16_t> full_audio;
        SimplePhraseBuilder::buildPhrase(words, full_audio);

        std::ofstream file("temp_audio.wav", std::ios::binary);
        if (file.is_open()) {
            write_wav_header(file, static_cast<int>(full_audio.size()), SAMPLE_RATE);
            file.write(reinterpret_cast<const char*>(full_audio.data()),
                       full_audio.size() * sizeof(int16_t));
            file.close();

            // Choose command based on bluetooth flag
            std::string cmd;
            if (bluetoothEnabled.load()) {
                cmd = "aplay -D bluealsa temp_audio.wav > /dev/null 2>&1";
            } else {
                cmd = "aplay temp_audio.wav > /dev/null 2>&1";
            }

            system(cmd.c_str());
        }
    }

    bool isAudioPlaying() const {
        return is_playing.load();
    }

    // Public getter/setter for bluetoothEnabled
    void setBluetoothEnabled(bool enabled) {
        bluetoothEnabled.store(enabled);
    }

    bool getBluetoothEnabled() const {
        return bluetoothEnabled.load();
    }
};

// Global audio manager instance
GameAudioManager* g_audio_manager = nullptr;

//======================================================================
void playerGameOpenPhrase() {
    std::vector<VoiceWord> open_phrase = {VOICE_WAKE, VOICE_UP, VOICE_YOU, VOICE_MUST, VOICE_CAPTURE, VOICE_GOLD, VOICE_AND, VOICE_DIAMONDS};
    g_audio_manager->playPhraseAsync(open_phrase);  // Play asynchronously
}

void playerGoNorthPhrase() {
	 if (!audioEnabled) return;
    std::vector<VoiceWord> north_phrase = {VOICE_GO, VOICE_NORTH};
    g_audio_manager->playPhraseAsync(north_phrase);  // Play asynchronously
}

void playerGoldCapturePhrase() {
	 if (!audioEnabled) return;
    std::vector<VoiceWord> gold_phrase = {VOICE_GOLD, VOICE_CAPTURE};
    g_audio_manager->playPhraseAsync(gold_phrase);  // Play asynchronously
}

void playerDiamondCapturePhrase() {
	 if (!audioEnabled) return;
    std::vector<VoiceWord> diamond_phrase = {VOICE_DIAMONDS, VOICE_CAPTURE};
    g_audio_manager->playPhraseAsync(diamond_phrase);  // Play asynchronously
}

void playerTeleportPhrase() {
	 if (!audioEnabled) return;
    std::vector<VoiceWord> phrase = {VOICE_TELEPORT};
    g_audio_manager->playPhraseAsync(phrase);  // Play asynchronously
}

void playerTowerJumpPhrase() {
	 if (!audioEnabled) return;
    std::vector<VoiceWord> phrase = {VOICE_TOWER, VOICE_JUMP};
    g_audio_manager->playPhraseAsync(phrase);  // Play asynchronously
}



//======================================================================

class GameState {
public:
    // Game level configuration
    
    enum Level { EASY, MEDIUM, HARD };
    Level currentLevel;    
    // Configuration variables based on level
    int numberSpiders;
    int numberSlimes;   
    int numberGoldPieces;
    int numberDiamonds;
    int numberPads;
    
    Vector3 playerPosition;
    Vector3 levelStartPosition; // Stores where the player should start for  level
    float playerAngle;    // Radians       
    float playerPitch;    // For aiming up/down
    float playerHeight;
    bool playerAlive;   
    int goldCount=0;
    int collectedDiamonds=0; // Count of collected diamonds
    bool onTower;         // Flag to track if player is on tower
    bool showMap;         // Flag to show game map
    float towerHeight;    // Height of the tower player is currently on
    
    struct Bullet {
        Vector3 position;
        Vector3 direction;
        float speed = 40.0f; // Increased speed for real-time movement
        bool active = true;
        float lifetime = 2.0f;
        std::vector<Vector3> history; // Stores previous positions for the trail
        float playerPitch = 0.0f; // Radians for looking up/down
        
        Bullet(const Vector3& pos, const Vector3& dir) : position(pos), direction(dir) {}
        
        // Now update takes 'dt' (deltaTime)
        void update(float dt) {
            // Save current position for the trail
            history.insert(history.begin(), position);
            if (history.size() > 8) history.pop_back(); // Keep only last 8 frames
            
            // Move: Position = Position + (Direction * Speed * Time)
            position.x += direction.x * speed * dt;
            position.y += direction.y * speed * dt;
            position.z += direction.z * speed * dt;
            
            lifetime -= dt;
            if (lifetime <= 0) active = false;
        }
    };
       
    
    std::vector<Tree> trees;
    std::vector<GoldPiece> goldPieces;
    std::vector<Spider> spiders;   
    std::vector<Bullet> bullets;
    std::vector<Slime> slimes;     
    std::vector<Tower> towers;
    std::vector<TreasureTower> treasuretowers;        
    std::vector<DiamondPiece> diamondPieces;
    
    // Wasps
    std::vector<Wasp> wasps;
    
    // Original starting position for reset
    Vector3 originalPosition;
    float originalAngle;  
    
    
  GameState()
    : currentLevel(EASY),
    playerPosition(0, 0, -70.0f), 
    playerAngle(0.0f), 
    playerPitch(0.0f),
    playerHeight(1.0f), 
    playerAlive(true),
    onTower(false),     // Now initialized BEFORE showMap
    showMap(false),      // Now initialized AFTER onTower
    towerHeight(0.0f) 
    {  
        originalPosition = playerPosition;
        originalAngle = playerAngle;
        setLevel(currentLevel);
        generateLevel();
    }
  
    void setLevel(Level level) {
        currentLevel = level;
        
        switch(level) {
            case EASY:
                levelStartPosition = Vector3(0, 0, -70.0f);
                numberSpiders = 3;
                numberSlimes = 2;   
                numberGoldPieces = 6;
                numberDiamonds = 4;
                numberPads = 3;
                break;
            case MEDIUM:
				levelStartPosition = Vector3(0, 0, -90.0f); // Further back for larger map
                numberSpiders = 5;
                numberSlimes = 4;   
                numberGoldPieces = 10;
                numberDiamonds = 4;
                numberPads = 3;
                break;
            case HARD:
				levelStartPosition = Vector3(50, 0, -90.0f); // Start in a corner
                numberSpiders = 8;
                numberSlimes = 6;   
                numberGoldPieces = 15;
                numberDiamonds = 4;
                numberPads = 3;
                break;
        }
        
        // sync the player to the  start position
        resetToStart();
    }   
    
    void resetToStart() {
        playerPosition = levelStartPosition;
        playerAngle = 0.0f; // Reset to face North
        playerHeight = 1.0f;
        playerPitch=0.0f;
        onTower = false;
		playerAlive = true; 
        onTower = false;        
        collectedDiamonds = 0;
        goldCount = 0; 
        generateLevel(); 
    }   
    
    void teleport() {
        playerPosition = levelStartPosition;
        playerAngle = 0.0f; // Reset to face North
        playerHeight = 1.0f;
        playerPitch=0.0f;
        onTower = false;
		playerAlive = true; 
        onTower = false;  
    }      
   
   
//======================================================================
// Generate game level
//======================================================================

void generateLevel() {    
    // Clear everything
    goldPieces.clear();
    spiders.clear();
    trees.clear();
    bullets.clear();
    towers.clear();
    treasuretowers.clear(); // Ensure this is cleared!
    slimes.clear();
    diamondPieces.clear();      
    wasps.clear();        

    //Create trees (Fixed positions)
    if (currentLevel == EASY) {
        trees.emplace_back(Vector3(15, 0, 15));
        trees.emplace_back(Vector3(-15, 0, 15));
        trees.emplace_back(Vector3(15, 0, -15));
        trees.emplace_back(Vector3(-15, 0, -15));
    } else if (currentLevel == MEDIUM) {
        trees.emplace_back(Vector3(15, 0, 15));
        trees.emplace_back(Vector3(-15, 0, 15));
        trees.emplace_back(Vector3(15, 0, -15));
        trees.emplace_back(Vector3(-15, 0, -15));
        trees.emplace_back(Vector3(30, 0, 30));
        trees.emplace_back(Vector3(-30, 0, 30));
        trees.emplace_back(Vector3(30, 0, -30));
        trees.emplace_back(Vector3(-30, 0, -30));
    } else { // HARD
        trees.emplace_back(Vector3(15, 0, 15));
        trees.emplace_back(Vector3(-15, 0, 15));
        trees.emplace_back(Vector3(15, 0, -15));
        trees.emplace_back(Vector3(-15, 0, -15));
        trees.emplace_back(Vector3(30, 0, 30));
        trees.emplace_back(Vector3(-30, 0, 30));
        trees.emplace_back(Vector3(30, 0, -30));
        trees.emplace_back(Vector3(-30, 0, -30));
        trees.emplace_back(Vector3(45, 0, 45));
        trees.emplace_back(Vector3(-45, 0, 45));
        trees.emplace_back(Vector3(45, 0, -45));
        trees.emplace_back(Vector3(-45, 0, -45));
    }

    // Create towers (Fixed positions)
    if (currentLevel == EASY) {
        towers.emplace_back(Vector3(0, Tower::TOWER_HEIGHT / 2.0f, 0)); // Center tower
        towers.emplace_back(Vector3(30, Tower::TOWER_HEIGHT / 2.0f, 30)); // Diagonal tower
    } else if (currentLevel == MEDIUM) {
        towers.emplace_back(Vector3(-25, Tower::TOWER_HEIGHT / 2.0f, -25)); // Bottom left
        towers.emplace_back(Vector3(25, Tower::TOWER_HEIGHT / 2.0f, -25)); // Bottom right
        towers.emplace_back(Vector3(25, Tower::TOWER_HEIGHT / 2.0f, 25)); // Top right
    } else { // HARD
        towers.emplace_back(Vector3(-30, Tower::TOWER_HEIGHT / 2.0f, -30)); // Bottom left
        towers.emplace_back(Vector3(30, Tower::TOWER_HEIGHT / 2.0f, -30)); // Bottom right
        towers.emplace_back(Vector3(30, Tower::TOWER_HEIGHT / 2.0f, 30)); // Top right
        towers.emplace_back(Vector3(-30, Tower::TOWER_HEIGHT / 2.0f, 30)); // Top left
    }

    // Create treasure towers (Fixed positions)
    if (currentLevel == EASY) {
        treasuretowers.emplace_back(Vector3(10, TreasureTower::TOWER_HEIGHT / 2.0f, 10));
    } else if (currentLevel == MEDIUM) {
        treasuretowers.emplace_back(Vector3(-15, TreasureTower::TOWER_HEIGHT / 2.0f, 15));
        treasuretowers.emplace_back(Vector3(15, TreasureTower::TOWER_HEIGHT / 2.0f, -15));
    } else { // HARD
        treasuretowers.emplace_back(Vector3(-20, TreasureTower::TOWER_HEIGHT / 2.0f, 20));
        treasuretowers.emplace_back(Vector3(20, TreasureTower::TOWER_HEIGHT / 2.0f, -20));
        treasuretowers.emplace_back(Vector3(0, TreasureTower::TOWER_HEIGHT / 2.0f, 0));
    }

    // Diamonds for Standard Towers (Roof)
    for (const auto& tower : towers) {
        Vector3 roofPos(tower.position.x, tower.position.y + (Tower::TOWER_HEIGHT / 2.0f) + 0.5f, tower.position.z);
        DiamondPiece roofDiamond(roofPos);
        roofDiamond.setIsOnTower(true);
        diamondPieces.push_back(roofDiamond);
    }

    // Diamonds for Treasure Towers
    for (const auto& tt : treasuretowers) {
        // Ground level diamond (on the floor)
        DiamondPiece groundDiamond(Vector3(tt.position.x, 1.0f, tt.position.z)); // Inside (Ground) 
        groundDiamond.setIsOnTower(false);
        diamondPieces.push_back(groundDiamond);
        
        // Rooftop diamond (on the roof floor - Height 6.0)	
        DiamondPiece roofDiamond(Vector3(tt.position.x, 6.1f, tt.position.z)); // Rooftop (Roof is 6.0, Diamond is at 6.1)
        roofDiamond.setIsOnTower(true);
        diamondPieces.push_back(roofDiamond);
    }

    // Create gold pieces - Fixed: Check for valid positions to avoid towers
    int createdGold = 0;
    while (createdGold < numberGoldPieces) {
        float x = (rand() % 30) - 15;
        float z = (rand() % 30) - 15;

        // Check if this position is inside any tower or treasure tower
        bool inTower = false;
        
        // Check standard towers
        for (const auto& tower : towers) {
            float halfWidth = Tower::TOWER_WIDTH / 2.0f;
            float halfDepth = Tower::TOWER_DEPTH / 2.0f;

            if (x >= tower.position.x - halfWidth && x <= tower.position.x + halfWidth &&
                z >= tower.position.z - halfDepth && z <= tower.position.z + halfDepth) {
                inTower = true;
                break;
            }
        }

        // Check treasure towers
        for (const auto& tt : treasuretowers) {
            float halfWidth = TreasureTower::TOWER_WIDTH / 2.0f;
            float halfDepth = TreasureTower::TOWER_DEPTH / 2.0f;

            if (x >= tt.position.x - halfWidth && x <= tt.position.x + halfWidth &&
                z >= tt.position.z - halfDepth && z <= tt.position.z + halfDepth) {
                inTower = true;
                break;
            }
        }

        // Only place gold piece if not inside a tower
        if (!inTower) {
            goldPieces.emplace_back(Vector3(x, 0, z));
            createdGold++;
        }
    }  

    
    // Spiders (Safe placement - avoid towers)
    for (int i = 0; i < numberSpiders; ++i) {
        float x, z;
        bool valid = false;
        
        // Keep trying until we find a valid location that's not inside any tower
        while (!valid) {
            x = (rand() % 90) - 45;
            z = (rand() % 90) - 45;
            
            valid = true;
            
            // Check against standard towers
            for (const auto& t : towers) {
                if (abs(x - t.position.x) < (Tower::TOWER_WIDTH/2.0f + 1.0f) && 
                    abs(z - t.position.z) < (Tower::TOWER_DEPTH/2.0f + 1.0f)) {
                    valid = false;
                    break;
                }
            }
            
            // Check against treasure towers
            for (const auto& tt : treasuretowers) {
                if (abs(x - tt.position.x) < (TreasureTower::TOWER_WIDTH/2.0f + 1.0f) && 
                    abs(z - tt.position.z) < (TreasureTower::TOWER_DEPTH/2.0f + 1.0f)) {
                    valid = false;
                    break;
                }
            }
        }
        
        spiders.emplace_back(Vector3(x, 0.0f, z)); //y=0.5 creates low level flying spiders
    }
    
    // Slimes (Safe placement - avoid towers)
    for (int i = 0; i < numberSlimes; ++i) {
        float x, z;
        bool valid = false;
        
        // Keep trying until we find a valid location that's not inside any tower
        while (!valid) {
            x = (rand() % 80) - 40;
            z = (rand() % 80) - 40;
            
            valid = true;
            
            // Check against standard towers
            for (const auto& t : towers) {
                if (abs(x - t.position.x) < (Tower::TOWER_WIDTH/2.0f + 1.0f) && 
                    abs(z - t.position.z) < (Tower::TOWER_DEPTH/2.0f + 1.0f)) {
                    valid = false;
                    break;
                }
            }
            
            // Check against treasure towers
            for (const auto& tt : treasuretowers) {
                if (abs(x - tt.position.x) < (TreasureTower::TOWER_WIDTH/2.0f + 1.0f) && 
                    abs(z - tt.position.z) < (TreasureTower::TOWER_DEPTH/2.0f + 1.0f)) {
                    valid = false;
                    break;
                }
            }
        }
        
        slimes.emplace_back(Vector3(x, 0.0f, z)); //y=0.0 
    }
    
    // Wasps (Hovering logic - on top of all towers)
    for (long unsigned int i = 0; i < towers.size(); ++i) {
        Vector3 waspPos(towers[i].position.x, 
                        towers[i].position.y + (Tower::TOWER_HEIGHT / 2.0f) + 4.0f, 
                        towers[i].position.z);
        wasps.emplace_back(Wasp(waspPos));
    }  
    
    // Wasps on treasure towers
    for (long unsigned int i = 0; i < treasuretowers.size(); ++i) {
        Vector3 waspPos(treasuretowers[i].position.x, 
                        treasuretowers[i].position.y + (TreasureTower::TOWER_HEIGHT / 2.0f) + 4.0f, 
                        treasuretowers[i].position.z);
        wasps.emplace_back(Wasp(waspPos));
    }
}

//======================================================================
    
    void moveForward(float d)  { playerPosition.x += sin(playerAngle) * d; playerPosition.z += cos(playerAngle) * d; }
    void moveBackward(float d) { playerPosition.x -= sin(playerAngle) * d; playerPosition.z -= cos(playerAngle) * d; }
    void turnLeft(float a)     { playerAngle += a; }
    void turnRight(float a)    { playerAngle -= a; }
    
    void rotate(float angle) {
        playerAngle += angle;
        // Normalize angle to keep it between 0 and 2*PI
        if (playerAngle < 0) playerAngle += 2.0f * M_PI;
        if (playerAngle > 2.0f * M_PI) playerAngle -= 2.0f * M_PI;
    }
    
    
    void updateBullets(float dt) {
        for (auto& b : bullets) if (b.active) b.update(dt);
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b){ return !b.active; }), bullets.end());      
    }
    
    Vector3 randomPosition() const {
        // uniformly in a square [-10,10] x [-10,10] on the XZ plane
        float x = (rand() / (float)RAND_MAX) * 20.0f - 10.0f;
        float z = (rand() / (float)RAND_MAX) * 20.0f - 10.0f;
        return Vector3(x, 0.0f, z);
    }

    void spawnSpider() {
        // Spawn spider at valid position outside towers
        float x, z;
        bool valid = false;
        
        while (!valid) {
            x = (rand() % 90) - 45;
            z = (rand() % 90) - 45;
            
            valid = true;
            
            // Check against standard towers
            for (const auto& t : towers) {
                if (abs(x - t.position.x) < (Tower::TOWER_WIDTH/2.0f + 1.0f) && 
                    abs(z - t.position.z) < (Tower::TOWER_DEPTH/2.0f + 1.0f)) {
                    valid = false;
                    break;
                }
            }
            
            // Check against treasure towers
            for (const auto& tt : treasuretowers) {
                if (abs(x - tt.position.x) < (TreasureTower::TOWER_WIDTH/2.0f + 1.0f) && 
                    abs(z - tt.position.z) < (TreasureTower::TOWER_DEPTH/2.0f + 1.0f)) {
                    valid = false;
                    break;
                }
            }
        }
        
        spiders.emplace_back(Vector3(x, 0.0f, z));
    }    
    
    Vector3 randomPosition() {
        return Vector3((rand() % 30) - 15, 0, (rand() % 30) - 15);
    }  
    
};

GameState gameState;


//======================================================================
void updateSpiderAI() {
    if (!gameState.playerAlive) return;

    for (auto& s : gameState.spiders) {
        if (!s.alive) continue;

        // Calculate 2D distance to player
        float dx = gameState.playerPosition.x - s.position.x;
        float dz = gameState.playerPosition.z - s.position.z;
        float dist = sqrt(dx * dx + dz * dz);

        // Update chasing state
        s.chasing = (dist < SPIDER_CHASE_DISTANCE);

        if (s.chasing && dist > 0.5f) {
            // Move toward player
            s.position.x += (dx / dist) * s.speed * 0.5f;
            s.position.z += (dz / dist) * s.speed * 0.5f;

            // Face the player
            s.angle = atan2(dx, dz) * 180.0f / M_PI;
        } else {
            // Idle behavior: slowly wander forward in current direction
            float rad = s.angle * M_PI / 180.0f;
            s.position.x += sin(rad) * s.speed * 0.1f;
            s.position.z += cos(rad) * s.speed * 0.1f;

            // Randomly turn slightly to simulate wandering
            if (rand() % 100 < 5) {
                s.angle += (rand() % 30 - 15);
            }
        }

        // Collision with player
        if (dist < 1.2f && abs(gameState.playerPosition.y - 1.5f) < 1.0f) {
            gameState.playerAlive = false;
        }
    }
}

void updateSlimeAI() {
    for (auto& s : gameState.slimes) {
        if (!s.alive) continue;

        // Slimes stay on the ground (Y=0)
        s.position.y = 0.0f; 

        float dx = gameState.playerPosition.x - s.position.x;
        float dz = gameState.playerPosition.z - s.position.z;
        float dist = sqrt(dx*dx + dz*dz);

        if (dist < SLIME_CHASE_DISTANCE && dist > 0.5f) {
            s.position.x += (dx / dist) * s.speed;
            s.position.z += (dz / dist) * s.speed;
        }

        // Only kill player if player is actually near the ground
        if (dist < 1.5f && gameState.playerPosition.y < 3.0f) {
            gameState.playerAlive = false;
        }
    }
}

//======================================================================

void drawLevelComplete() {
    // Switch to 2D Projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Darken background slightly
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(WINDOW_WIDTH, 0);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
        glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Draw Text
    glColor3f(1.0f, 1.0f, 0.0f); // Gold
    const char* message = "LEVEL COMPLETE!";
    const char* subMessage = "All Diamonds Collected. Press 'X' to Restart.";
    
    // Position text in middle (roughly)
    glRasterPos2f(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 20);
    for (int i = 0; message[i] != '\0'; i++) 
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);

    glRasterPos2f(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 20);
    for (int i = 0; subMessage[i] != '\0'; i++) 
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, subMessage[i]);

    // Restore 3D
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

//======================================================================

void renderHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);     
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // 2. Draw Black Banner at TOP (Y=0 to Y=100)
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(WINDOW_WIDTH, 0);
        glVertex2f(WINDOW_WIDTH, 100);
        glVertex2f(0, 100);
    glEnd();

    // 3. Draw Text (Adjusted Y positions)
    glColor3f(1.0f, 1.0f, 1.0f); 
    char buf[256];    
    sprintf(buf, "GOLD: %d/%d  |  DIAMONDS: %d/%d  |  FPS: %.1f", 
            gameState.goldCount, gameState.numberGoldPieces,
            gameState.collectedDiamonds, gameState.numberDiamonds, fps);    
    glRasterPos2f(20, 40); // Moved to top
    for (int i = 0; buf[i]; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buf[i]);

    // Show level properly with all three levels
    const char* levelStr;
    switch(gameState.currentLevel) {
        case GameState::EASY:   levelStr = "EASY"; break;
        case GameState::MEDIUM: levelStr = "MEDIUM"; break;
        case GameState::HARD:   levelStr = "HARD"; break;
        default:                levelStr = "UNKNOWN"; break;
    }
    
    sprintf(buf, "LEVEL: %s  |  PLAYER: %s |  BLUETOOTH: %s", 
    levelStr, 
    (gameState.playerAlive ? "ALIVE" : "DEAD"),
    (g_audio_manager->getBluetoothEnabled() ? "ON" : "OFF") 
    );
    glRasterPos2f(20, 70); // Moved to top
    for (int i = 0; buf[i]; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buf[i]);  
     
    bool levelComplete = (gameState.goldCount >= gameState.numberGoldPieces && 
                          gameState.collectedDiamonds >= gameState.numberDiamonds);    
    if (levelComplete) drawLevelComplete();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawWeapon() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0); 
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    float centerX = WINDOW_WIDTH / 2.0f;
    float bottomY = WINDOW_HEIGHT;

    // Draw Crosshair
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(centerX - 10, WINDOW_HEIGHT/2); glVertex2f(centerX + 10, WINDOW_HEIGHT/2);
        glVertex2f(centerX, WINDOW_HEIGHT/2 - 10); glVertex2f(centerX, WINDOW_HEIGHT/2 + 10);
    glEnd();
        
    float aimOffset = gameState.playerPitch * 40.0f; // Reduced sensitivity
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(centerX - 20, bottomY);
        glVertex2f(centerX + 20, bottomY);
        glVertex2f(centerX + 15, bottomY - 80 + aimOffset); // 80 is much shorter than 150
        glVertex2f(centerX - 15, bottomY - 80 + aimOffset);
    glEnd();

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}
//======================================================================

void renderMap() {
    if (!gameState.showMap) return;

    // Switch to 2D Overlay
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float mapSize = 350.0f; 
    float margin = 20.0f;
    float xOff = margin;
    float yOff = margin;

    //  Map Background & Border
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(xOff, yOff); glVertex2f(xOff + mapSize, yOff);
        glVertex2f(xOff + mapSize, yOff + mapSize); glVertex2f(xOff, yOff + mapSize);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(xOff, yOff); glVertex2f(xOff + mapSize, yOff);
        glVertex2f(xOff + mapSize, yOff + mapSize); glVertex2f(xOff, yOff + mapSize);
    glEnd();

    // North Indicator
    glRasterPos2f(xOff + (mapSize / 2.0f) - 5.0f, yOff + mapSize + 5.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'N');

    auto toMapX = [&](float x) { return xOff + (x + 100.0f) / 200.0f * mapSize; };
    auto toMapY = [&](float z) { return yOff + (z + 100.0f) / 200.0f * mapSize; };
    
    
    // PLAYER & DIRECTION STICK
    float px = toMapX(gameState.playerPosition.x);
    float py = toMapY(gameState.playerPosition.z);
    float pSize = 8.0f;
    float lineLen = pSize * 2.5f;
    float hx = px - sin(gameState.playerAngle) * lineLen; 
    float hy = py + cos(gameState.playerAngle) * lineLen; 

    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(3.0f);
    glBegin(GL_LINES); glVertex2f(px, py); glVertex2f(hx, hy); glEnd();

    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(px - pSize, py - pSize); glVertex2f(px + pSize, py - pSize);
        glVertex2f(px + pSize, py + pSize); glVertex2f(px - pSize, py + pSize);
    glEnd();

    // DRAW TOWERS AS RECTANGLES 
    float tSize = 10.0f; // Visual size of the tower on the map
    glColor3f(0.6f, 0.6f, 0.6f); 
    for (const auto& t : gameState.towers) {
        float tx = toMapX(t.position.x);
        float ty = toMapY(t.position.z);
        glBegin(GL_QUADS);
            glVertex2f(tx - tSize, ty - tSize); glVertex2f(tx + tSize, ty - tSize);
            glVertex2f(tx + tSize, ty + tSize); glVertex2f(tx - tSize, ty + tSize);
        glEnd();
    }
    // Treasure Towers (Darker Red blocks)
    glColor3f(0.4f, 0.0f, 0.0f);
    for (const auto& tt : gameState.treasuretowers) {
        float tx = toMapX(tt.position.x);
        float ty = toMapY(tt.position.z);
        glBegin(GL_QUADS);
            glVertex2f(tx - tSize, ty - tSize); glVertex2f(tx + tSize, ty - tSize);
            glVertex2f(tx + tSize, ty + tSize); glVertex2f(tx - tSize, ty + tSize);
        glEnd();
    }

    // DRAW ITEMS 
    glPointSize(10.0f); 
    glBegin(GL_POINTS);
        glColor3f(0.9f, 0.7f, 0.0f); // Gold
        for (const auto& g : gameState.goldPieces) glVertex2f(toMapX(g.position.x), toMapY(g.position.z));
        
        glColor3f(0.0f, 0.9f, 1.0f); // Diamonds
        for (const auto& d : gameState.diamondPieces) glVertex2f(toMapX(d.position.x), toMapY(d.position.z));
    glEnd();

    // DRAW ENEMIES
    glPointSize(8.0f);
    glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f); // Spiders, Slimes, Wasps
        for (const auto& s : gameState.spiders) if (s.alive) glVertex2f(toMapX(s.position.x), toMapY(s.position.z));
        for (const auto& sl : gameState.slimes) if (sl.alive) glVertex2f(toMapX(sl.position.x), toMapY(sl.position.z));
        for (const auto& w : gameState.wasps) if (w.alive) glVertex2f(toMapX(w.position.x), toMapY(w.position.z));
    glEnd();
   

    // LEGEND (Outside map)
    float legX = xOff + mapSize + 10.0f; 
    float currentY = yOff + 120.0f; 
    auto drawLegendItem = [&](const char* text, float r, float g, float b, float& yPos) {
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
            glVertex2f(legX, yPos); glVertex2f(legX + 12, yPos);
            glVertex2f(legX + 12, yPos + 12); glVertex2f(legX, yPos + 12);
        glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
        glRasterPos2f(legX + 18, yPos + 1);
        for (const char* c = text; *c; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
        yPos -= 20.0f;
    };

    drawLegendItem("PLAYER", 0.0f, 0.8f, 0.0f, currentY);
    drawLegendItem("TOWER", 0.6f, 0.6f, 0.6f, currentY);
    drawLegendItem("TREASURE", 0.4f, 0.0f, 0.0f, currentY);
    drawLegendItem("GOLD", 0.9f, 0.7f, 0.0f, currentY);
    drawLegendItem("DIAMOND", 0.0f, 0.9f, 1.0f, currentY);
    drawLegendItem("ENEMIES", 1.0f, 0.0f, 0.0f, currentY);

    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
}



//======================================================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Calculate the effective angle 
    float effectiveAngle = gameState.playerAngle;    

    //  LookAt calculations
    float lookX = gameState.playerPosition.x + sin(effectiveAngle) * cos(gameState.playerPitch);
    float lookY = (gameState.playerHeight + 0.5f) + sin(gameState.playerPitch);
    float lookZ = gameState.playerPosition.z + cos(gameState.playerAngle) * cos(gameState.playerPitch); 
    // lookZ should use effectiveAngle too
    lookZ = gameState.playerPosition.z + cos(effectiveAngle) * cos(gameState.playerPitch);

    gluLookAt(gameState.playerPosition.x, gameState.playerHeight + 0.5f, gameState.playerPosition.z,
              lookX, lookY, lookZ,
              0.0, 1.0, 0.0);
              
     //ground plane size
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.8f, 0.5f);  // Green ground
    glVertex3f(-200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, 200.0f);
    glVertex3f(-200.0f, 0.0f, 200.0f);
    glEnd();   
        
    // Draw all other objects (trees, towers, pads, etc.) - they should be at y=0 or slightly above
    for (const auto& tree : gameState.trees)  tree.draw();     
    for (const auto& tower : gameState.towers)  tower.draw();
    for (const auto& gold : gameState.goldPieces) gold.draw();   
    for (const auto& diamond : gameState.diamondPieces) diamond.draw();
    // draw treasure towers
    for (const auto& tt : gameState.treasuretowers) tt.draw();
       
    
    for (const auto& spider : gameState.spiders) {
        float legAnim = spiderLegAnimation;
        if (!spider.alive) legAnim = 0.0f;
        spider.draw(legAnim);
    }
    
    // Bullets
    for (const auto& b : gameState.bullets) if (b.active) {
        glPushMatrix();
        glTranslatef(b.position.x, b.position.y, b.position.z);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidSphere(BULLET_RADIUS, 8, 8);
        glPopMatrix();
    }
    
     //Draw slimes    
    for (const auto& slime : gameState.slimes) {
        float slimeAnim = slimeAnimation;
        if (!slime.alive) slimeAnim = 0.0f;
        slime.draw(slimeAnim);
    } 
    
    // Draw wasps
    for (const auto& wasp : gameState.wasps) {
        float waspAnim = slimeAnimation; // Reuse slime animation for simplicity
        if (!wasp.alive) waspAnim = 0.0f;
        wasp.draw(waspAnim);
    }
    
    // Draw 2D weapon overlay (3D weapon with sight)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();    
    
    drawWeapon(); //Gemini draw weapon function
    
    
    //Bullets
    glDisable(GL_LIGHTING); // Trails look better without lighting
    for (const auto& b : gameState.bullets) {
        if (!b.active) continue;

        // Draw the Head
        glPushMatrix();
        glTranslatef(b.position.x, b.position.y, b.position.z);
        glColor3f(1.0f, 1.0f, 0.0f); 
        glutSolidSphere(BULLET_RADIUS, 8, 8);
        glPopMatrix();

        // Draw the Trail (The Interpolation)
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < b.history.size(); ++i) {
            // Fade the trail based on history index
            float alpha = 1.0f - ((float)i / b.history.size());
            glColor4f(1.0f, 0.5f, 0.0f, alpha); 
            glVertex3f(b.history[i].x, b.history[i].y, b.history[i].z);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);      
 
    renderHUD();    
    
    if (gameState.showMap) {
        renderMap();
    }
    glutSwapBuffers();
    
   
              
}

//======================================================================
void keyboard(unsigned char key, int, int) {
    // Reset 
    if (key == 'x' || key == 'X') {
        //std::cout << "Reset\n";
        gameState.resetToStart();
        return;
    }
    
    // Reset to original position
    if (key == 'o' || key == 'O') {
        //std::cout << "Teleport -Reset to original position\n";
        gameState.teleport();
        playerTeleportPhrase();
        return;
    }
     
    
    if (!gameState.playerAlive) return;
    
    switch (key) {
        case 'w': case 'W': gameState.moveForward(0.5f); break;
        case 's': case 'S': gameState.moveBackward(0.5f); break;
        case 'a': case 'A': gameState.turnLeft(0.05f); break;        
        case 'd': case 'D': gameState.turnRight(0.05f); break; 
        case 'c': case 'C': std::cout << "Reset puzzle" << std::endl; break; 
              
        
        case 'r': case 'R': 
        gameState.rotate(M_PI / 2.0f); // Rotate 90 degrees clockwise
        break;
        
        		
        case 'l': case 'L': 
        // Cycle through game levels
        if (gameState.currentLevel == GameState::EASY) {
            gameState.setLevel(GameState::MEDIUM);
            //std::cout << "Switched to MEDIUM level" << std::endl;
        } else if (gameState.currentLevel == GameState::MEDIUM) {
            gameState.setLevel(GameState::HARD);
            //std::cout << "Switched to HARD level" << std::endl;
        } else {
            gameState.setLevel(GameState::EASY);
            //std::cout << "Switched to EASY level" << std::endl;
        }        
        break;
        
        
        
        case 'm': case 'M':     
        gameState.showMap=!gameState.showMap;
        //std::cout << "Game Map: " << (gameState.showMap ? "ON" : "OFF") << std::endl;
        break;
        
        case 'u': case 'U': 
        gameState.playerPitch += 0.05f; 
        if (gameState.playerPitch > 1.4f) gameState.playerPitch = 1.4f; // Clamp slightly before 90 deg
        break;
        case 'i': case 'I': 
        gameState.playerPitch -= 0.05f; 
        if (gameState.playerPitch < -1.4f) gameState.playerPitch = -1.4f; // Clamp slightly before -90 deg
        break;
         
       
        case ' ': { // Space to shoot	            
            float fireAngle = gameState.playerAngle;
            float dx = sin(fireAngle) * cos(gameState.playerPitch);
            float dy = sin(gameState.playerPitch);
            float dz = cos(fireAngle) * cos(gameState.playerPitch);	
              
            Vector3 bulletDir(dx, dy, dz);
            Vector3 startPos = gameState.playerPosition;
            startPos.y += 0.5f; // Fire from chest height        
            gameState.bullets.emplace_back(startPos, bulletDir);
        } break; 
                
        case 't': case 'T':
        // Jump to top of nearest tower
        for (const auto& tower : gameState.towers) {
            if (checkTowerJumpCollision(gameState.playerPosition, tower.position, Tower::TOWER_WIDTH/2, JUMP_DISTANCE_RANGE)) {
                gameState.onTower = true;
                gameState.playerHeight = tower.position.y + Tower::TOWER_HEIGHT/2 + 1.0f;
                playerTowerJumpPhrase();
                return; // Exit once jump is successful
            }
        }
        for (const auto& tt : gameState.treasuretowers) {
            if (checkTowerJumpCollision(gameState.playerPosition, tt.position, TreasureTower::TOWER_WIDTH/2, JUMP_DISTANCE_RANGE)) {
                gameState.onTower = true;
                gameState.playerHeight = tt.position.y + TreasureTower::TOWER_HEIGHT/2 + 1.0f;
                playerTowerJumpPhrase();
                return;
            }
        }
        break; 
                
        case 'g': case 'G':
        // Jump down from tower
        if (gameState.onTower) {
            gameState.onTower = false;
            // Reset to ground level
            gameState.playerHeight = 1.5f;
        }
        break;  
        
        //Audio
        case 'b': case 'B':
            g_audio_manager->setBluetoothEnabled(!g_audio_manager->getBluetoothEnabled());
            //std::cout << "Bluetooth enabled: " 
                      //<< (g_audio_manager->getBluetoothEnabled() ? "ON" : "OFF") 
                      //<< std::endl;
            break;
        
        case 'z': case 'Z': 
        audioEnabled = !audioEnabled;
        //std::cout << "Audio " << (audioEnabled ? "ON" : "OFF") << std::endl;
        break;
        
        case 27: exit(0); break;
    }
    glutPostRedisplay();
}


//======================================================================
void specialKeys(int key, int, int) {
    if (!gameState.playerAlive) return;
    switch (key) {
        case GLUT_KEY_UP:    gameState.moveForward(0.25f); break;
        case GLUT_KEY_DOWN:  gameState.moveBackward(0.25f); break;
        case GLUT_KEY_LEFT:  gameState.turnLeft(0.025f); break;
        case GLUT_KEY_RIGHT: gameState.turnRight(0.025f); break;
    }
    glutPostRedisplay();
}


// FPS Calculation Helper
void handleFPSCalculation() {
    frameCount++;
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000) {
        fps = frameCount * 1000.0f / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }
}

// Projectile Collision Helper
void handleBulletCollisions() {
    for (auto it = gameState.bullets.begin(); it != gameState.bullets.end(); ) {
        bool bulletHit = false;

        // Check Spiders
        for (auto& s : gameState.spiders) {
            if (s.alive && checkCollision(it->position, s.position, BULLET_RADIUS, Spider::SPIDER_RADIUS)) {
                s.hitCount++;
                bulletHit = true;
                if (s.hitCount >= 4) {
                    s.alive = false;
                    gameState.spawnSpider(); // Spawn new spider
                }
                break;
            }
        }

        // Check Slimes (only if bullet didn't hit spider)
        if (!bulletHit) {
            for (auto& slime : gameState.slimes) {
                if (slime.alive && checkCollision(it->position, slime.position, BULLET_RADIUS, Slime::SLIME_HEAD_RADIUS)) {
                    slime.hitCount++;
                    bulletHit = true;
                    if (slime.hitCount >= 4) {
                        slime.alive = false;
                        // Spawn new slime at valid position
                        gameState.slimes.emplace_back(gameState.randomPosition());
                    }
                    break;
                }
            }
        }

        // Check Wasps
        if (!bulletHit) {
            for (auto& wasp : gameState.wasps) {
                if (wasp.alive && checkCollision(it->position, wasp.position, BULLET_RADIUS, WASP_HIT_RADIUS)) {
                    wasp.hitCount++;
                    bulletHit = true;
                    if (wasp.hitCount >= 3) {
                        wasp.alive = false;
                        // Respawn wasp on a random tower
                        if (!gameState.towers.empty()) {
                            int r = rand() % gameState.towers.size();
                            Vector3 p = gameState.towers[r].position;
                            gameState.wasps.emplace_back(Vector3(p.x, p.y + 5.0f, p.z));
                        }
                    }
                    break;
                }
            }
        }

        if (bulletHit || !it->active) it = gameState.bullets.erase(it);
        else ++it;
    }
}

void checkTowerCollision(Vector3& playerPos, const Vector3& towerPos, float w, float d) {
    // Add a small margin (0.5f) so the player doesn't clip through the wall
    float hw = w / 2.0f + 0.5f; 
    float hd = d / 2.0f + 0.5f;

    // Check if player is within the tower's X and Z bounds
    if (playerPos.x > towerPos.x - hw && playerPos.x < towerPos.x + hw &&
        playerPos.z > towerPos.z - hd && playerPos.z < towerPos.z + hd) {
        
        // Calculate distance to each side to find the closest exit point
        float dxLeft   = abs(playerPos.x - (towerPos.x - hw));
        float dxRight  = abs(playerPos.x - (towerPos.x + hw));
        float dzBack   = abs(playerPos.z - (towerPos.z - hd));
        float dzFront  = abs(playerPos.z - (towerPos.z + hd));

        float minDist = std::min({dxLeft, dxRight, dzBack, dzFront});

        // Push the player to the nearest edge
        if (minDist == dxLeft)       playerPos.x = towerPos.x - hw;
        else if (minDist == dxRight) playerPos.x = towerPos.x + hw;
        else if (minDist == dzBack)  playerPos.z = towerPos.z - hd;
        else if (minDist == dzFront) playerPos.z = towerPos.z + hd;
    }
}


void checkTreasureTowerCollision(Vector3& p, const Vector3& t, float w, float d) {
    float hw = w / 2.0f;
    float hd = d / 2.0f;
    float margin = 0.5f;

    // Is the player currently inside the tower footprint?
    bool isInside = (p.x > t.x - hw && p.x < t.x + hw && p.z > t.z - hd && p.z < t.z + hd);

    if (isInside) {
        // PUSH BACK INSIDE: If they hit the back or sides from the inside
        if (p.x < t.x - hw + margin) p.x = t.x - hw + margin; // Left wall
        if (p.x > t.x + hw - margin) p.x = t.x + hw - margin; // Right wall
        if (p.z < t.z - hd + margin) p.z = t.z - hd + margin; // Back wall
        //  No check for +Z (Front) allows them to walk out the door
    } else {
        // PUSH AWAY: Standard AABB collision for the 3 solid sides
        if (p.x > t.x - hw - margin && p.x < t.x + hw + margin &&
            p.z > t.z - hd - margin && p.z < t.z + hd + margin) {
            
            // Only block if NOT entering through the front door area
            if (p.z < t.z + hd) { 
                float dxL = abs(p.x - (t.x - hw - margin));
                float dxR = abs(p.x - (t.x + hw + margin));
                float dzB = abs(p.z - (t.z - hd - margin));
                float minDist = std::min({dxL, dxR, dzB});

                if (minDist == dxL) p.x = t.x - hw - margin;
                else if (minDist == dxR) p.x = t.x + hw + margin;
                else p.z = t.z - hd - margin;
            }
        }
    }
}

//======================================================================
// handle player collisions with tower detection
//======================================================================

void handlePlayerCollisions() {
   
    // check player killed
    for (const auto& spider : gameState.spiders) 
        if (spider.alive && checkCollision(gameState.playerPosition, spider.position, 0.5f, Spider::SPIDER_RADIUS) &&  !gameState.onTower) 
            gameState.playerAlive = false;
            
     for (const auto& slime : gameState.slimes) 
        if (slime.alive && checkCollision(gameState.playerPosition, slime.position, 0.5f, Slime::SLIME_HEAD_RADIUS) &&  !gameState.onTower) 
            gameState.playerAlive = false;
    
  
     // Use a 2D distance check for the wasps (ignoring height)     
	for (const auto& wasp : gameState.wasps) {
	if (wasp.alive) {
	// Calculate 2D distance (X and Z only)
	float dx = wasp.position.x - gameState.playerPosition.x;
	float dz = wasp.position.z - gameState.playerPosition.z;
	float dist2D = sqrt(dx*dx + dz*dz);
	
	if (dist2D < WASP_HIT_RADIUS) {
	gameState.playerAlive = false;
	}
	}
	}

    // Solid Tower Collisions (Only if we aren't standing ON them)
    if (!gameState.onTower) {
        for (const auto& t : gameState.towers) {
            checkTowerCollision(gameState.playerPosition, t.position, Tower::TOWER_WIDTH, Tower::TOWER_DEPTH);
        }
        for (const auto& tt : gameState.treasuretowers) {
            checkTreasureTowerCollision(gameState.playerPosition, tt.position, TreasureTower::TOWER_WIDTH, TreasureTower::TOWER_DEPTH);
        }
    }

    //Item Collection (Gold & Diamonds)
    // Using 2D distance for items on the ground to avoid Y-axis misses
    for (auto it = gameState.goldPieces.begin(); it != gameState.goldPieces.end(); ) {
        float dist2D = sqrt(pow(it->position.x - gameState.playerPosition.x, 2) + 
                            pow(it->position.z - gameState.playerPosition.z, 2));
        if (dist2D < 1.2f) {
            gameState.goldCount++;
            it = gameState.goldPieces.erase(it);            
            playerGoldCapturePhrase();            
        } else ++it;
    }
    
    // diamond collection with isOnTower logic
    for (auto it = gameState.diamondPieces.begin(); it != gameState.diamondPieces.end(); ) {
        float dist2D = sqrt(pow(it->position.x - gameState.playerPosition.x, 2) + 
                            pow(it->position.z - gameState.playerPosition.z, 2));
        if (dist2D < 1.2f) {
            // Only collect if player state matches diamond state
            if (gameState.onTower == it->getIsOnTower()) {
                gameState.collectedDiamonds++;
                it = gameState.diamondPieces.erase(it);
                playerDiamondCapturePhrase();
            } else {
                ++it;
            }
        } else ++it;
    }
}

//=====================================================================
void handleEnvironmentPhysics() {
    // Gravity and falling physics
    if (!gameState.onTower && gameState.playerPosition.y > 1.5f) {
        gameState.playerPosition.y -= 0.15f; // Falling speed
        
        // Ground collision
        if (gameState.playerPosition.y < 1.5f) {
            gameState.playerPosition.y = 1.5f;
        }
    }

    // Ensure player doesn't fall below ground level
    if (gameState.playerPosition.y < 1.5f) {
        gameState.playerPosition.y = 1.5f;
    }
}

//======================================================================


// Audio Trigger Helper
void handleAudioWarnings() {
    if (!audioEnabled || !gameState.playerAlive) return;

    bool danger = false;
    float warnDist = 6.0f;

    for (auto& s : gameState.spiders) if (s.alive && distance(s.position, gameState.playerPosition) < warnDist) danger = true;
    for (auto& sl : gameState.slimes) if (sl.alive && distance(sl.position, gameState.playerPosition) < warnDist) danger = true;
    for (auto& w : gameState.wasps) if (w.alive && distance(w.position, gameState.playerPosition) < warnDist) danger = true;

    static bool dangerPlayed = false;
    if (danger && !dangerPlayed) {
        g_audio_manager->playPhraseAsync({VOICE_DANGER, VOICE_CREATURE, VOICE_APPROACHING});
        dangerPlayed = true;
    } else if (!danger) dangerPlayed = false;
}

//======================================================================

void timer(int) {
    
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Convert milliseconds to seconds
    lastTime = currentTime;
    
    
    if (gameState.playerAlive) {
        spiderLegAnimation += 0.1f;
        slimeAnimation += 0.1f;
        
        handleFPSCalculation();
        
        updateSpiderAI();
        updateSlimeAI();
        for (auto& wasp : gameState.wasps) if (wasp.alive) wasp.update();

        gameState.updateBullets(deltaTime);  
        
        handleBulletCollisions();
        handlePlayerCollisions();
        handleEnvironmentPhysics();
        handleAudioWarnings();  
    }
 
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // Aim for ~60fps
}

//======================================================================
void init() {
    glClearColor(0.52f, 0.8f, 0.98f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float light_pos[]    = {10.0f, 10.0f, 10.0f, 1.0f};
    float light_amb[]    = {0.2f, 0.2f, 0.2f, 1.0f};
    float light_diff[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    float light_spec[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

//======================================================================
int main(int argc, char** argv) {
    // Initialize audio manager
    g_audio_manager = new GameAudioManager();
    playerGameOpenPhrase();
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("GameWorld");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    
    // Cleanup
    delete g_audio_manager;
    return 0;
}
