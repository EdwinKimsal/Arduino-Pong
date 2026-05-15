// Arduino pins
const int SerialCol = 3;
const int SerialRow = 5;

const int ShiftClk = 6;
const int DisplClk = 9;

const int oneUp = 7;
const int oneDown = 8;

const int twoUp = 10;
const int twoDown = 11;

// Server
int server = 1;


// Length
const int ledLength = 6;

// Seconds Per Frame
const int spf = 1;

// A blank image (row, col)
int img[][6] = { // [row, col]
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0}
};

// Intiate a struct to store paddle movement
struct PaddleMovements {
  int player[2] = {0, 0}; // playerOne, playerTwo
};
PaddleMovements paddleMovements;

// Initiate a struct to store paddle position
struct PaddlePositions {
  int top[2] = {0, ledLength-2}; // playerOne, playerTwo
  int bot[2] = {1, ledLength-1}; // playerOne, playerTwo
};
PaddlePositions paddlePositions;

// Initiate a struct to store ball movement
struct BallMovement {
  int x = 1;
  int y = 1;
};
BallMovement ballMovement;

// Initiate a struct to store ball position
struct BallPosition {
  int x = (ledLength / 2) - 1;
  int y = (ledLength / 2) - 1;
};
BallPosition ballPosition;

// Initiate a variable to keep track if it is a new round
bool newRound = true;

// Ran on start
void setup() {
  // Set the pins
  pinMode(SerialCol, OUTPUT);
  pinMode(SerialRow, OUTPUT);
  pinMode(ShiftClk, OUTPUT);
  pinMode(DisplClk, OUTPUT);
  pinMode(oneUp, INPUT);
  pinMode(oneDown, INPUT);
  pinMode(twoUp, INPUT);
  pinMode(twoDown, INPUT);

  digitalWrite(SerialCol, LOW);
  digitalWrite(SerialRow, LOW);
  digitalWrite(ShiftClk, LOW);
  digitalWrite(DisplClk, LOW);

  // Reset the display
  displImage(img);

  // Update the image to have paddle
  img[0][0] = 1; // Left paddle
  img[1][0] = 1; // Left paddle
  img[4][5] = 1; // Right paddle
  img[5][5] = 1; // Right paddle
}


// Constantly running
void loop() {
  // If new round, start
  if (newRound) {
    // Start the new round
    startNewRound();

    // Check paddle movements
    checkPaddleDirection();

    // Reposition paddles
    updatePaddle(1); // Player One's paddle
    updatePaddle(2); // Player Two's paddle

    // Set the display for seconds per frame
    unsigned startTime = millis();
    while (millis() - startTime < 1000*spf) { displImage(img); }

    // Check paddle movements
    checkPaddleDirection();

    // Reposition paddles
    updatePaddle(1); // Player One's paddle
    updatePaddle(2); // Player Two's paddle
  }

  // Else, continue with this round
  else {
    // Check paddle movements
    checkPaddleDirection();

    // Reposition paddles
    updatePaddle(1); // Player One's paddle
    updatePaddle(2); // Player Two's paddle

    // Check and move ball
    checkBallCel();
    moveBall();

    // Check new round
    checkNewRound();
  }

  // Set the display for seconds per frame
  unsigned startTime = millis();
  while (millis() - startTime < 1000*spf) { displImage(img); }
}


/*
* Function for a new round
*   - params:
*     - none
*   - return:
*     - none
*/
void startNewRound() {
  // Remove old ball
  img[ballPosition.y][ballPosition.x] = 0;

  // Set random postion of new ball
  ballPosition.x = server + (ledLength / 2) - 2;
  ballPosition.y = (ledLength / 2) - 1;

  // Set random direction of the new ball
  ballMovement.x = server==1 ? 1 : -1;
  ballMovement.y = 0;

  // Set ball
  img[ballPosition.y][ballPosition.x] = 1;

  // Set the other person as server
  server = server == 1 ? 2 : 1;

  // No longer a new round
  newRound = false;
}


/*
* Function to check the direction to move the players' paddles
*   - params:
*     - none
*   - return:
*     - none
*/
void checkPaddleDirection() {
  // Read inputs
  int oneUpInput = digitalRead(oneUp);
  int oneDownInput = digitalRead(oneDown);
  int twoUpInput = digitalRead(twoUp);
  int twoDownInput = digitalRead(twoDown);

  // Get movement directions
  if (oneUpInput == HIGH && oneDownInput == LOW){
    paddleMovements.player[0] = 1;
  } else if (oneUpInput == LOW && oneDownInput == HIGH) {
    paddleMovements.player[0] = -1;
  } else {
    paddleMovements.player[0] = 0;
  }

  if (twoUpInput == HIGH && twoDownInput == LOW){
    paddleMovements.player[1] = 1;
  } else if (twoUpInput == LOW && twoDownInput == HIGH) {
    paddleMovements.player[1] = -1;
  } else {
    paddleMovements.player[1] = 0;
  }
}


/*
* Function to update a player's paddle
*   - params:
*     - playerInt, int representing player (1 or 2)
*   - return:
*     - none
*/
void updatePaddle(int playerInt) {
  int idx = playerInt-1; // Subtract one for index
  int moveDir = paddleMovements.player[idx]; // Direction player is moving

  if (moveDir == 0) return; // No move

  // Calculate the potential new positions
  int newTop = paddlePositions.top[idx] - moveDir;
  int newBot = paddlePositions.bot[idx] - moveDir;

  // If the new position is valid, reposition
  if (newTop >= 0 && newBot <= ledLength-1) {
    int col = idx == 0 ? 0 : ledLength - 1;

    // Reset new paddle
    img[paddlePositions.top[idx]][col] = 0;
    img[paddlePositions.bot[idx]][col] = 0;

    paddlePositions.top[idx] = newTop; // Move top
    paddlePositions.bot[idx] = newBot; // Move bottom
    
    // Set new paddle
    img[paddlePositions.top[idx]][col] = 1;
    img[paddlePositions.bot[idx]][col] = 1;
  }
}


/* Function to move the ball before displaying
*   - params:
*     - none
*   - return:
*     - none
*/
void moveBall() {
  // Remove old ball
  img[ballPosition.y][ballPosition.x] = 0;

  // Update ball
  ballPosition.y = ballPosition.y - ballMovement.y;
  ballPosition.x = ballPosition.x + ballMovement.x;

  // Add new ball
  img[ballPosition.y][ballPosition.x] = 1;
}


/* Function to check for a new round
*   - params:
*     - none
*   - return:
*     - none
*/
void checkNewRound() {
  // Get player int
  int playerInt = ballPosition.x == 1 ? 1 : ballPosition.x == ledLength-2 ? 2 : 0;
  int idx = playerInt-1;

  // On a player's side
  if (playerInt != 0) {
    // Find where, and if, the ball hit
    bool hitTop = paddlePositions.top[idx] == ballPosition.y;
    bool hitBot = paddlePositions.bot[idx] == ballPosition.y;
    newRound = !hitTop && !hitBot; // Ball did not hit

    // Check paddle hit
    if (!newRound) {
      // x-direction
      ballMovement.x = -1*ballMovement.x;

      // y-direction
      bool posY = (ballMovement.y == 1 && hitTop) || (ballMovement.y == 0 && hitTop);
      bool straightY = (ballMovement.y == 1 && hitBot) || (ballMovement.y == -1 && hitTop);
      // No need to calculate negY due to else

      // Apply movement
      if (posY) { ballMovement.y = 1; }
      else if (straightY) { ballMovement.y = 0; }
      else { ballMovement.y = -1; }
    }
  }
}


/* Function to check the balls next movement
*   - params:
*     - none
*   - return:
*     - none
*/
void checkBallCel() {
  // y-direction
  // Check if the ball hit top or bottom of screen
  if (ballPosition.y == 0 || ballPosition.y == ledLength - 1) {
    ballMovement.y = ballMovement.y * -1;
  }
}


/* Function to display an image
*   - params:
*     - matrixDispl, a 2D array of integers of which lights are ON (1) or OFF (0)
*   - return:
*     - none
*/
void displImage(int matrixDispl[][ledLength]) {
  // Iterate through each column
  for (int col=0; col<ledLength; col++) {

    // Iterate through each row
    for (int row=0; row<ledLength; row++) {
      // Set the row shifter as appropriate
      // LED is ON
      if (matrixDispl[ledLength-row-1][col] == 1) { // Read the matrix bottom to top due to shifter
        digitalWrite(SerialRow, LOW);
      
      // LED is OFF
      } else {
        digitalWrite(SerialRow, HIGH);
      }

      // Set the col shifter as appropriate
      // Column is ON
      if (col == ledLength-row-1) {
        digitalWrite(SerialCol, HIGH);
      }

      // Column is OFF
      else {
        digitalWrite(SerialCol, LOW);
      }

      // Update shifter
      digitalWrite(ShiftClk, HIGH);

      // Reset pins
      digitalWrite(ShiftClk, LOW);
      digitalWrite(SerialCol, LOW);
      digitalWrite(SerialRow, LOW);
    }

    // Update Display
    digitalWrite(DisplClk, HIGH);
    digitalWrite(DisplClk, LOW);
  }
}
