/*
2d GRAPHIC CALCULATOR.
Author: Aryan Agrawal.

USAGE
To run use command->  gcc Main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
or if make present use->  make run

FEATURES:
1. Box on top left can be used to change the color of the graph drawn.
2. Can enter any equation. The equation should be a polynomial in x.
Graph of y = (input polynomial is drawn.)

TDOO: Add functionality to take any equation in terms of x and y.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>


typedef struct {
    float coefficients[100];
    float exponents[100];
    int numTerms;
    Color color;
    char equation[256];
} Equation;

#define MAX_EQUATIONS 10

float scale = 20.0;
const int screenW = 1850;
const int screenL = 1010;
const int graphW = 1670;
const int graphL = 810;
int offsetX = 100;
int offsetY = 100;
Color colors[] = {RED, BLUE, GREEN, PINK, PURPLE, ORANGE, MAROON, DARKBLUE, LIME, DARKPURPLE};
int currentColorIndex = 0;
Vector2 lastMousePos = {0};
bool isDragging = false;
Equation equations[MAX_EQUATIONS];
int numEquations = 0;
char equationInput[256] = "4x - 1";
bool inputActive = false;

// PixelToX and PixelToY function convert the pixel of the screen into points of the graph area.
float PixelToX(int pixel) {
    return (float)(pixel - offsetX - graphW / 2) / scale;
}
float PixelToY(int pixel) {
    return (float)(offsetY + graphL / 2 - pixel) / scale;
}


// XtoPixel and Y to Pixel function convert the points of graph/ equation to pixel value so they can be constructed.
int XtoPixel(float x) {
    return (int)(offsetX + graphW / 2 + x * scale);
}
int YtoPixel(float y) {
    return (int)(offsetY + graphL / 2 - y * scale);
}

// Evaluate polynomial funds the value of polynomial forany given x
float EvaluateEquation(Equation* eq, float x) {
    float result = 0.0;
    for (int i = 0; i < eq->numTerms; i++) {
        result += eq->coefficients[i] * pow(x, eq->exponents[i]);
    }
    return result;
}

// In order for the polynomial function line to stadn out from the axis and gridlines we can create thick lines using a for loop.
void DrawThickLine(int x1, int y1, int x2, int y2, Color color, int thickness) {
    for (int i = -thickness / 2; i <= thickness / 2; i++) {
        DrawLine(x1, y1 + i, x2, y2 + i, color);
    }
}

/*
    Function DrawPolynomial is used to draw the polynomial of the screen.
    TODO: fix the panning functionality using right click to allow visible portion of screen graph to be shown.

    Currenly has several extra variables like visiableXMin, visibleXMax which should be used in the panning functionality.
*/
void DrawEquation(Equation* eq) {
    float visibleXMin = PixelToX(0);
    float visibleXMax = PixelToX(screenW);
    float step = 0.1f / (scale / 20.0f);
    if (step < 0.01f) step = 0.01f;
    if (step > 0.5f) step = 0.5f;
    
    bool isFirstPoint = true;
    int prevPixelX = 0;
    int prevPixelY = 0;
    
    for (float x = visibleXMin; x <= visibleXMax; x += step) {
        float y = EvaluateEquation(eq, x);
        int pixelX = XtoPixel(x);
        int pixelY = YtoPixel(y);
        
        if (y > 1e6 || y < -1e6) continue;
        
        if (!isFirstPoint) {
            if ((pixelX >= offsetX && pixelX <= offsetX + graphW && 
                 pixelY >= offsetY && pixelY <= offsetY + graphL) ||
                (prevPixelX >= offsetX && prevPixelX <= offsetX + graphW && 
                 prevPixelY >= offsetY && prevPixelY <= offsetY + graphL)) {
                DrawThickLine(prevPixelX, prevPixelY, pixelX, pixelY, eq->color, 3);
            }
        }
        
        prevPixelX = pixelX;
        prevPixelY = pixelY;
        isFirstPoint = false;
    }
}

// Draws axis of thr graph on opening the executable.
// TODO: Need to work on visiable portiona and panning parts still.
void DrawGraphAxes() {
    DrawLine(offsetX, offsetY + graphL / 2, offsetX + graphW, offsetY + graphL / 2, BLACK);
    DrawLine(offsetX + graphW / 2, offsetY, offsetX + graphW / 2, offsetY + graphL, BLACK);
    float visibleXMin = PixelToX(0);
    float visibleXMax = PixelToX(screenW);
    float visibleYMin = PixelToY(offsetY + graphL);
    float visibleYMax = PixelToY(offsetY);
    int step = (scale > 50) ? 1 : (scale > 20) ? 5 : (scale > 10) ? 10 : (scale > 5) ? 20 : (scale > 2) ? 40 : 80;
    int startX = (int)floor(visibleXMin / step) * step;
    int endX = (int)ceil(visibleXMax / step) * step;
    
    for (int x = startX; x <= endX; x += step) {
        int pixelX = XtoPixel(x);
        if (pixelX >= offsetX && pixelX <= offsetX + graphW) {
            DrawLine(pixelX, offsetY, pixelX, offsetY + graphL, LIGHTGRAY);
            if (x != 0) {
                DrawText(TextFormat("%d", x), pixelX - 10, offsetY + graphL / 2 + 5, 10, BLACK);
            }
        }
    }
    
    int startY = (int)floor(visibleYMin / step) * step;
    int endY = (int)ceil(visibleYMax / step) * step;
    
    for (int y = startY; y <= endY; y += step) {
        int pixelY = YtoPixel(y);
        if (pixelY >= offsetY && pixelY <= offsetY + graphL) {
            DrawLine(offsetX, pixelY, offsetX + graphW, pixelY, LIGHTGRAY);
            if (y != 0) {
                DrawText(TextFormat("%d", y), offsetX + graphW / 2 + 5, pixelY - 5, 10, BLACK);
            }
        }
    }
    DrawText("0", offsetX + graphW / 2 + 5, offsetY + graphL / 2 + 5, 10, BLACK);
}

/*
MOST IMPORTANT FUNCTION: Parses the input given in the input box to find the degree and coefficient of the polynomial.

TODO: Add features for more advance functions like log, sin, cos...
*/
void ParseEquation(const char* equation, Equation* eq) {
    memset(eq->coefficients, 0, sizeof(eq->coefficients));
    memset(eq->exponents, 0, sizeof(eq->exponents));
    eq->numTerms = 0;
    strcpy(eq->equation, equation);

    char* eq_str = strdup(equation);
    char* token;
    char* rest = eq_str;
    bool isNegative = false;

    while ((token = strtok_r(rest, "+-", &rest))) {
        float coef = isNegative ? -1.0f : 1.0f;
        float exp = 0.0f;
        char* xPos = strchr(token, 'x');

        while (*token == ' ') token++;

        if (xPos) {
            char coefStr[32] = {0};
            strncpy(coefStr, token, xPos - token);

            if (strlen(coefStr) > 0 && strcmp(coefStr, " ") != 0) {
                if (strcmp(coefStr, "-") == 0) coef *= -1.0f;
                else coef *= atof(coefStr);
            }

            char* powerPos = strchr(xPos, '^');
            if (powerPos) {
                exp = atof(powerPos + 1);  // Supports floating-point exponents
            } else {
                exp = 1.0f;
            }
        } else {
            coef *= atof(token);
            exp = 0.0f;
        }

        eq->coefficients[eq->numTerms] = coef;
        eq->exponents[eq->numTerms] = exp;
        eq->numTerms++;
        char* nextChar = rest;
        while (nextChar && *nextChar == ' ') nextChar++;
        isNegative = (nextChar && *nextChar == '-');
        if (isNegative) {
            rest = nextChar + 1;
        }
    }

    free(eq_str);
}

int main() {
    InitWindow(screenW, screenL, "Multi-Equation Graphing Software");
    SetTargetFPS(60);

    Rectangle colorBox = {10, 10, 50, 50};
    Rectangle inputBox = {70, 10, 200, 50};
    Rectangle addButton = {280, 10, 150, 50};

    while (!WindowShouldClose()) {
        /*
        Can change the scale of the graph using scroll.
        */
        float scroll = GetMouseWheelMove();
        if (scroll != 0) {
            scale += scroll * 2.0f;
            if (scale < 1.0f) scale = 1.0f;
            if (scale > 100.0f) scale = 100.0f;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            
            if (CheckCollisionPointRec(mousePos, colorBox)) {
                currentColorIndex = (currentColorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
            } else if (CheckCollisionPointRec(mousePos, inputBox)) {
                inputActive = true;
            } else if (CheckCollisionPointRec(mousePos, addButton) && strlen(equationInput) > 0) {
                if (numEquations < MAX_EQUATIONS) {
                    /*
                        Check if the color changing or the text box is clicked or not.
                    */
                    ParseEquation(equationInput, &equations[numEquations]);
                    equations[numEquations].color = colors[currentColorIndex];
                    numEquations++;
                    memset(equationInput, 0, sizeof(equationInput));
                }
                inputActive = false;
            } else {
                inputActive = false;
            }
        }

        if (inputActive) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32 && key <= 125) && strlen(equationInput) < 255) {
                    char keyStr[] = {(char)key, '\0'};
                    strcat(equationInput, keyStr);
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && strlen(equationInput) > 0) {
                equationInput[strlen(equationInput) - 1] = '\0';
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);
        
        DrawGraphAxes();
        
        // Draw all equations
        for (int i = 0; i < numEquations; i++) {
            DrawEquation(&equations[i]);
        }

         /*
        Label the points close the polynomical graph.
        Has accuracy upto (2.0f) -> Can lead to certain thiings like the value of y will be same for x ranging in +-2.0f.

        TODO: Can you fix it such that the labelling is shown when is closed to a point but the correct value of X and Y shown
        (without saving all the points used in evaluate polynomials.)
        */
        Vector2 mousePos = GetMousePosition();
        float mouseXGraph = PixelToX(mousePos.x);
        float mouseYGraph = PixelToY(mousePos.y);
        float tolerance = 2.0f;
        for (int i = 0; i < numEquations; i++) {
            float evaluatedY = EvaluateEquation(&equations[i], mouseXGraph);
            if (fabs(mouseYGraph - evaluatedY) < tolerance) {
                DrawText(TextFormat("(%0.1f, %0.1f)", mouseXGraph, evaluatedY), 
                        mousePos.x + 10, mousePos.y - 20 - (i * 25), 20, equations[i].color);
            }
        }

        // Draw UI elements
        DrawRectangleRec(colorBox, colors[currentColorIndex]);
        DrawRectangleLines(colorBox.x, colorBox.y, colorBox.width, colorBox.height, BLACK);
        
        DrawRectangleRec(inputBox, LIGHTGRAY);
        DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, BLACK);
        DrawText(equationInput, inputBox.x + 5, inputBox.y + 15, 20, BLACK);

        DrawRectangleRec(addButton, DARKGRAY);
        DrawRectangleLines(addButton.x, addButton.y, addButton.width, addButton.height, BLACK);
        DrawText("Add Graph", addButton.x + 5, addButton.y + 15, 20, WHITE);
        for (int i = 0; i < numEquations; i++) {
            DrawText(equations[i].equation, 10, 70 + i * 30, 20, equations[i].color);
        }
        DrawText("Mouse wheel to zoom", 10, screenL - 30, 20, BLACK);
        DrawText("Click color box to change color", 10, screenL - 60, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
