/**
 * @file graphics_editor.c
 * @brief A menu-driven 2D Graphics Editor using standard C library.
 * 
 * This program allows users to draw, modify, and delete 2D geometric shapes 
 * (Lines, Rectangles, Triangles, and Circles) on a 50x50 character-based canvas.
 * It uses Bresenham's algorithms for discrete rasterization on a grid.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CANVAS_SIZE 50
#define MAX_SHAPES 100

/* --- Data Structures --- */

/**
 * @brief Enum representing supported shape types.
 */
typedef enum {
    SHAPE_RECTANGLE = 1,
    SHAPE_LINE,
    SHAPE_TRIANGLE,
    SHAPE_CIRCLE
} ShapeType;

/**
 * @brief Structure for a Rectangle defined by two opposite corners.
 */
typedef struct {
    int x1, y1;
    int x2, y2;
} Rectangle;

/**
 * @brief Structure for a Line defined by two endpoints.
 */
typedef struct {
    int x1, y1;
    int x2, y2;
} Line;

/**
 * @brief Structure for a Triangle defined by three vertices.
 */
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} Triangle;

/**
 * @brief Structure for a Circle defined by center point and radius.
 */
typedef struct {
    int cx, cy;
    int radius;
} Circle;

/**
 * @brief Composite structure representing a shape in the editor.
 */
typedef struct {
    int id;             ///< Unique identifier for the object
    ShapeType type;     ///< Type of shape
    union {
        Rectangle rect;
        Line line;
        Triangle tri;
        Circle circle;
    } data;
} Shape;

/* --- Function Prototypes --- */

void initializeCanvas(char canvas[CANVAS_SIZE][CANVAS_SIZE]);
void displayCanvas(const char canvas[CANVAS_SIZE][CANVAS_SIZE]);
void drawLine(char canvas[CANVAS_SIZE][CANVAS_SIZE], int x1, int y1, int x2, int y2);
void drawRectangle(char canvas[CANVAS_SIZE][CANVAS_SIZE], int x1, int y1, int x2, int y2);
void drawTriangle(char canvas[CANVAS_SIZE][CANVAS_SIZE], int x1, int y1, int x2, int y2, int x3, int y3);
void drawCircle(char canvas[CANVAS_SIZE][CANVAS_SIZE], int cx, int cy, int radius);

int deleteObject(Shape shapes[], int *shape_count, int id, char canvas[CANVAS_SIZE][CANVAS_SIZE]);
int modifyObject(Shape shapes[], int shape_count, int id, char canvas[CANVAS_SIZE][CANVAS_SIZE]);
void redrawCanvas(char canvas[CANVAS_SIZE][CANVAS_SIZE], const Shape shapes[], int shape_count);

int getIntegerInput(const char *prompt, int min, int max);
void displayObjects(const Shape shapes[], int shape_count);

/* --- Function Implementations --- */

/**
 * @brief Initializes the drawing canvas by filling it with underscores.
 * @param canvas The 2D character array representing the canvas.
 */
void initializeCanvas(char canvas[CANVAS_SIZE][CANVAS_SIZE]) {
    for (int y = 0; y < CANVAS_SIZE; y++) {
        for (int x = 0; x < CANVAS_SIZE; x++) {
            canvas[y][x] = '_';
        }
    }
}

/**
 * @brief Renders the canvas to standard output with row and column coordinate guides.
 * @param canvas The 2D character array representing the canvas.
 */
void displayCanvas(const char canvas[CANVAS_SIZE][CANVAS_SIZE]) {
    printf("\n");
    // Print column tens digits
    printf("     ");
    for (int x = 0; x < CANVAS_SIZE; x++) {
        printf("%d ", x / 10);
    }
    printf("\n");

    // Print column units digits
    printf("     ");
    for (int x = 0; x < CANVAS_SIZE; x++) {
        printf("%d ", x % 10);
    }
    printf("\n");

    // Print top border (3 spaces, '+', 101 dashes, '+')
    printf("   +");
    for (int i = 0; i < 101; i++) {
        printf("-");
    }
    printf("+\n");

    // Print rows with row index prefix and pixel values
    for (int y = 0; y < CANVAS_SIZE; y++) {
        printf("%02d | ", y);
        for (int x = 0; x < CANVAS_SIZE; x++) {
            printf("%c ", canvas[y][x]);
        }
        printf("|\n");
    }

    // Print bottom border
    printf("   +");
    for (int i = 0; i < 101; i++) {
        printf("-");
    }
    printf("+\n\n");
}

/**
 * @brief Draws a single pixel on the canvas if it lies within the valid bounds.
 * @param canvas The 2D character array.
 * @param x Horizontal coordinate.
 * @param y Vertical coordinate.
 */
static void drawPixel(char canvas[CANVAS_SIZE][CANVAS_SIZE], int x, int y) {
    if (x >= 0 && x < CANVAS_SIZE && y >= 0 && y < CANVAS_SIZE) {
        canvas[y][x] = '*';
    }
}

/**
 * @brief Draws a straight line using Bresenham's Line Algorithm.
 * @param canvas The 2D character array.
 * @param x1 Start X coordinate.
 * @param y1 Start Y coordinate.
 * @param x2 End X coordinate.
 * @param y2 End Y coordinate.
 */
void drawLine(char canvas[CANVAS_SIZE][CANVAS_SIZE], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        drawPixel(canvas, x1, y1);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * @brief Draws a rectangle outline by drawing its four boundary lines.
 * @param canvas The 2D character array.
 * @param x1 X coordinate of first corner.
 * @param y1 Y coordinate of first corner.
 * @param x2 X coordinate of opposite corner.
 * @param y2 Y coordinate of opposite corner.
 */
void drawRectangle(char canvas[CANVAS_SIZE][CANVAS_SIZE], int x1, int y1, int x2, int y2) {
    // Top boundary
    drawLine(canvas, x1, y1, x2, y1);
    // Bottom boundary
    drawLine(canvas, x1, y2, x2, y2);
    // Left boundary
    drawLine(canvas, x1, y1, x1, y2);
    // Right boundary
    drawLine(canvas, x2, y1, x2, y2);
}

/**
 * @brief Draws a triangle by drawing lines connecting three vertices.
 * @param canvas The 2D character array.
 * @param x1 X coordinate of vertex 1.
 * @param y1 Y coordinate of vertex 1.
 * @param x2 X coordinate of vertex 2.
 * @param y2 Y coordinate of vertex 2.
 * @param x3 X coordinate of vertex 3.
 * @param y3 Y coordinate of vertex 3.
 */
void drawTriangle(char canvas[CANVAS_SIZE][CANVAS_SIZE], int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(canvas, x1, y1, x2, y2);
    drawLine(canvas, x2, y2, x3, y3);
    drawLine(canvas, x3, y3, x1, y1);
}

/**
 * @brief Draws a circle outline using the Midpoint Circle Algorithm.
 * @param canvas The 2D character array.
 * @param cx Center X coordinate.
 * @param cy Center Y coordinate.
 * @param radius Radius of the circle.
 */
void drawCircle(char canvas[CANVAS_SIZE][CANVAS_SIZE], int cx, int cy, int radius) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        // Render 8-way symmetric points
        drawPixel(canvas, cx + x, cy + y);
        drawPixel(canvas, cx - x, cy + y);
        drawPixel(canvas, cx + x, cy - y);
        drawPixel(canvas, cx - x, cy - y);
        drawPixel(canvas, cx + y, cy + x);
        drawPixel(canvas, cx - y, cy + x);
        drawPixel(canvas, cx + y, cy - x);
        drawPixel(canvas, cx - y, cy - x);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/**
 * @brief Rebuilds the canvas from scratch by clearing it and redrawing all shapes in the list.
 * @param canvas The 2D character array.
 * @param shapes Array of shape structures.
 * @param shape_count Total count of active shapes in the array.
 */
void redrawCanvas(char canvas[CANVAS_SIZE][CANVAS_SIZE], const Shape shapes[], int shape_count) {
    initializeCanvas(canvas);
    for (int i = 0; i < shape_count; i++) {
        switch (shapes[i].type) {
            case SHAPE_RECTANGLE:
                drawRectangle(canvas, shapes[i].data.rect.x1, shapes[i].data.rect.y1, 
                              shapes[i].data.rect.x2, shapes[i].data.rect.y2);
                break;
            case SHAPE_LINE:
                drawLine(canvas, shapes[i].data.line.x1, shapes[i].data.line.y1, 
                         shapes[i].data.line.x2, shapes[i].data.line.y2);
                break;
            case SHAPE_TRIANGLE:
                drawTriangle(canvas, shapes[i].data.tri.x1, shapes[i].data.tri.y1, 
                             shapes[i].data.tri.x2, shapes[i].data.tri.y2, 
                             shapes[i].data.tri.x3, shapes[i].data.tri.y3);
                break;
            case SHAPE_CIRCLE:
                drawCircle(canvas, shapes[i].data.circle.cx, shapes[i].data.circle.cy, 
                           shapes[i].data.circle.radius);
                break;
        }
    }
}

/**
 * @brief Deletes a shape from the list by ID and updates the canvas.
 * @param shapes Array of shape structures.
 * @param shape_count Pointer to the current count of shapes.
 * @param id Unique ID of the shape to delete.
 * @param canvas The 2D character array.
 * @return 1 on success, 0 if shape ID was not found.
 */
int deleteObject(Shape shapes[], int *shape_count, int id, char canvas[CANVAS_SIZE][CANVAS_SIZE]) {
    for (int i = 0; i < *shape_count; i++) {
        if (shapes[i].id == id) {
            // Shift remaining elements down to keep list packed
            for (int j = i; j < *shape_count - 1; j++) {
                shapes[j] = shapes[j + 1];
            }
            (*shape_count)--;
            redrawCanvas(canvas, shapes, *shape_count);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Modifies a shape's coordinates in-place and redraws the canvas.
 * @param shapes Array of shape structures.
 * @param shape_count Current count of shapes.
 * @param id Unique ID of the shape to modify.
 * @param canvas The 2D character array.
 * @return 1 on success, 0 if shape ID was not found.
 */
int modifyObject(Shape shapes[], int shape_count, int id, char canvas[CANVAS_SIZE][CANVAS_SIZE]) {
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            printf("\n--- Modifying Object (ID %d: ", id);
            switch (shapes[i].type) {
                case SHAPE_RECTANGLE:
                    printf("Rectangle) ---\n");
                    shapes[i].data.rect.x1 = getIntegerInput("Enter new X1 (0-49): ", 0, 49);
                    shapes[i].data.rect.y1 = getIntegerInput("Enter new Y1 (0-49): ", 0, 49);
                    shapes[i].data.rect.x2 = getIntegerInput("Enter new X2 (0-49): ", 0, 49);
                    shapes[i].data.rect.y2 = getIntegerInput("Enter new Y2 (0-49): ", 0, 49);
                    break;

                case SHAPE_LINE:
                    printf("Line) ---\n");
                    shapes[i].data.line.x1 = getIntegerInput("Enter new X1 (0-49): ", 0, 49);
                    shapes[i].data.line.y1 = getIntegerInput("Enter new Y1 (0-49): ", 0, 49);
                    shapes[i].data.line.x2 = getIntegerInput("Enter new X2 (0-49): ", 0, 49);
                    shapes[i].data.line.y2 = getIntegerInput("Enter new Y2 (0-49): ", 0, 49);
                    break;

                case SHAPE_TRIANGLE:
                    printf("Triangle) ---\n");
                    shapes[i].data.tri.x1 = getIntegerInput("Enter new X1 (0-49): ", 0, 49);
                    shapes[i].data.tri.y1 = getIntegerInput("Enter new Y1 (0-49): ", 0, 49);
                    shapes[i].data.tri.x2 = getIntegerInput("Enter new X2 (0-49): ", 0, 49);
                    shapes[i].data.tri.y2 = getIntegerInput("Enter new Y2 (0-49): ", 0, 49);
                    shapes[i].data.tri.x3 = getIntegerInput("Enter new X3 (0-49): ", 0, 49);
                    shapes[i].data.tri.y3 = getIntegerInput("Enter new Y3 (0-49): ", 0, 49);
                    break;

                case SHAPE_CIRCLE:
                    printf("Circle) ---\n");
                    shapes[i].data.circle.cx = getIntegerInput("Enter new Center X (0-49): ", 0, 49);
                    shapes[i].data.circle.cy = getIntegerInput("Enter new Center Y (0-49): ", 0, 49);
                    shapes[i].data.circle.radius = getIntegerInput("Enter new Radius (1-49): ", 1, 49);
                    break;
            }
            redrawCanvas(canvas, shapes, shape_count);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Prints the list of active shapes with their properties to stdout.
 * @param shapes Array of shape structures.
 * @param shape_count Current count of shapes.
 */
void displayObjects(const Shape shapes[], int shape_count) {
    printf("\n=================== ACTIVE OBJECTS LIST ===================\n");
    printf("%-5s | %-10s | %-40s\n", "ID", "Shape Type", "Coordinates / Parameters");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < shape_count; i++) {
        printf("%-5d | ", shapes[i].id);
        switch (shapes[i].type) {
            case SHAPE_RECTANGLE:
                printf("%-10s | P1: (%d, %d), P2: (%d, %d)\n", 
                       "Rectangle", 
                       shapes[i].data.rect.x1, shapes[i].data.rect.y1,
                       shapes[i].data.rect.x2, shapes[i].data.rect.y2);
                break;
            case SHAPE_LINE:
                printf("%-10s | Start: (%d, %d), End: (%d, %d)\n", 
                       "Line", 
                       shapes[i].data.line.x1, shapes[i].data.line.y1,
                       shapes[i].data.line.x2, shapes[i].data.line.y2);
                break;
            case SHAPE_TRIANGLE:
                printf("%-10s | V1: (%d, %d), V2: (%d, %d), V3: (%d, %d)\n", 
                       "Triangle", 
                       shapes[i].data.tri.x1, shapes[i].data.tri.y1,
                       shapes[i].data.tri.x2, shapes[i].data.tri.y2,
                       shapes[i].data.tri.x3, shapes[i].data.tri.y3);
                break;
            case SHAPE_CIRCLE:
                printf("%-10s | Center: (%d, %d), Radius: %d\n", 
                       "Circle", 
                       shapes[i].data.circle.cx, shapes[i].data.circle.cy,
                       shapes[i].data.circle.radius);
                break;
        }
    }
    if (shape_count == 0) {
        printf("(No shapes added yet)\n");
    }
    printf("===========================================================\n\n");
}

/**
 * @brief Reads an integer input from standard input, with bounds checking and buffer flushing.
 * @param prompt Text display query.
 * @param min Minimum allowable integer value.
 * @param max Maximum allowable integer value.
 * @return The validated integer input.
 */
int getIntegerInput(const char *prompt, int min, int max) {
    char buffer[128];
    long val;
    char *endptr;

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue;
        }

        // Strip trailing newline character
        buffer[strcspn(buffer, "\n")] = '\0';

        // Check if input is empty
        if (strlen(buffer) == 0) {
            printf("Empty input. Please enter a valid integer.\n");
            continue;
        }

        // Convert string to integer base 10
        val = strtol(buffer, &endptr, 10);

        // Check if parsing failed
        if (endptr == buffer) {
            printf("Invalid input. Please enter a valid integer.\n");
            continue;
        }

        // Skip any trailing spaces in the buffer
        while (*endptr != '\0' && isspace((unsigned char)*endptr)) {
            endptr++;
        }

        // Check if there are trailing invalid characters after digits
        if (*endptr != '\0') {
            printf("Invalid input. Please enter only numeric values.\n");
            continue;
        }

        // Validate range bounds
        if (val < min || val > max) {
            printf("Value out of range. Please enter a value between %d and %d.\n", min, max);
            continue;
        }

        break;
    }
    return (int)val;
}

/* --- Main Application --- */

/**
 * @brief Entry point of the 2D Graphics Editor.
 */
int main(void) {
    char canvas[CANVAS_SIZE][CANVAS_SIZE];
    Shape shapes[MAX_SHAPES];
    int shape_count = 0;
    int next_id = 1;

    initializeCanvas(canvas);

    printf("==================================================\n");
    printf("        WELCOME TO THE 2D GRAPHICS EDITOR        \n");
    printf("==================================================\n");
    printf("Note: Canvas coordinates range from (0,0) at the \n");
    printf("top-left to (%d,%d) at the bottom-right.\n", CANVAS_SIZE-1, CANVAS_SIZE-1);
    printf("==================================================\n");

    displayCanvas(canvas);

    while (1) {
        printf("-------------------- MENU --------------------\n");
        printf("1. Draw Rectangle\n");
        printf("2. Draw Line\n");
        printf("3. Draw Triangle\n");
        printf("4. Draw Circle\n");
        printf("5. Delete Object\n");
        printf("6. Modify Object\n");
        printf("7. Display Canvas\n");
        printf("8. Exit\n");
        printf("----------------------------------------------\n");

        int choice = getIntegerInput("Enter choice (1-8): ", 1, 8);

        if (choice == 8) {
            printf("\nExiting program. Thank you for using the 2D Graphics Editor!\n");
            break;
        }

        switch (choice) {
            case 1: { // Draw Rectangle
                if (shape_count >= MAX_SHAPES) {
                    printf("Error: Canvas shape capacity reached (%d shapes max).\n", MAX_SHAPES);
                    break;
                }
                printf("\n--- Draw Rectangle ---\n");
                Shape new_shape;
                new_shape.id = next_id++;
                new_shape.type = SHAPE_RECTANGLE;
                new_shape.data.rect.x1 = getIntegerInput("Enter X1 (0-49): ", 0, 49);
                new_shape.data.rect.y1 = getIntegerInput("Enter Y1 (0-49): ", 0, 49);
                new_shape.data.rect.x2 = getIntegerInput("Enter X2 (0-49): ", 0, 49);
                new_shape.data.rect.y2 = getIntegerInput("Enter Y2 (0-49): ", 0, 49);

                shapes[shape_count++] = new_shape;
                
                // Redraw and display
                redrawCanvas(canvas, shapes, shape_count);
                displayCanvas(canvas);
                printf("Rectangle added successfully as Object ID %d.\n\n", new_shape.id);
                break;
            }

            case 2: { // Draw Line
                if (shape_count >= MAX_SHAPES) {
                    printf("Error: Canvas shape capacity reached (%d shapes max).\n", MAX_SHAPES);
                    break;
                }
                printf("\n--- Draw Line ---\n");
                Shape new_shape;
                new_shape.id = next_id++;
                new_shape.type = SHAPE_LINE;
                new_shape.data.line.x1 = getIntegerInput("Enter X1 (0-49): ", 0, 49);
                new_shape.data.line.y1 = getIntegerInput("Enter Y1 (0-49): ", 0, 49);
                new_shape.data.line.x2 = getIntegerInput("Enter X2 (0-49): ", 0, 49);
                new_shape.data.line.y2 = getIntegerInput("Enter Y2 (0-49): ", 0, 49);

                shapes[shape_count++] = new_shape;

                // Redraw and display
                redrawCanvas(canvas, shapes, shape_count);
                displayCanvas(canvas);
                printf("Line added successfully as Object ID %d.\n\n", new_shape.id);
                break;
            }

            case 3: { // Draw Triangle
                if (shape_count >= MAX_SHAPES) {
                    printf("Error: Canvas shape capacity reached (%d shapes max).\n", MAX_SHAPES);
                    break;
                }
                printf("\n--- Draw Triangle ---\n");
                Shape new_shape;
                new_shape.id = next_id++;
                new_shape.type = SHAPE_TRIANGLE;
                new_shape.data.tri.x1 = getIntegerInput("Enter X1 (0-49): ", 0, 49);
                new_shape.data.tri.y1 = getIntegerInput("Enter Y1 (0-49): ", 0, 49);
                new_shape.data.tri.x2 = getIntegerInput("Enter X2 (0-49): ", 0, 49);
                new_shape.data.tri.y2 = getIntegerInput("Enter Y2 (0-49): ", 0, 49);
                new_shape.data.tri.x3 = getIntegerInput("Enter X3 (0-49): ", 0, 49);
                new_shape.data.tri.y3 = getIntegerInput("Enter Y3 (0-49): ", 0, 49);

                shapes[shape_count++] = new_shape;

                // Redraw and display
                redrawCanvas(canvas, shapes, shape_count);
                displayCanvas(canvas);
                printf("Triangle added successfully as Object ID %d.\n\n", new_shape.id);
                break;
            }

            case 4: { // Draw Circle
                if (shape_count >= MAX_SHAPES) {
                    printf("Error: Canvas shape capacity reached (%d shapes max).\n", MAX_SHAPES);
                    break;
                }
                printf("\n--- Draw Circle ---\n");
                Shape new_shape;
                new_shape.id = next_id++;
                new_shape.type = SHAPE_CIRCLE;
                new_shape.data.circle.cx = getIntegerInput("Enter Center X (0-49): ", 0, 49);
                new_shape.data.circle.cy = getIntegerInput("Enter Center Y (0-49): ", 0, 49);
                new_shape.data.circle.radius = getIntegerInput("Enter Radius (1-49): ", 1, 49);

                shapes[shape_count++] = new_shape;

                // Redraw and display
                redrawCanvas(canvas, shapes, shape_count);
                displayCanvas(canvas);
                printf("Circle added successfully as Object ID %d.\n\n", new_shape.id);
                break;
            }

            case 5: { // Delete Object
                if (shape_count == 0) {
                    printf("\nNo objects exist on the canvas to delete.\n\n");
                    break;
                }
                displayObjects(shapes, shape_count);
                int target_id = getIntegerInput("Enter Object ID to delete: ", 1, next_id - 1);
                
                int success = deleteObject(shapes, &shape_count, target_id, canvas);
                if (success) {
                    displayCanvas(canvas);
                    printf("Object ID %d deleted successfully.\n\n", target_id);
                } else {
                    printf("Error: Object ID %d not found or already deleted.\n\n", target_id);
                }
                break;
            }

            case 6: { // Modify Object
                if (shape_count == 0) {
                    printf("\nNo objects exist on the canvas to modify.\n\n");
                    break;
                }
                displayObjects(shapes, shape_count);
                int target_id = getIntegerInput("Enter Object ID to modify: ", 1, next_id - 1);
                
                int success = modifyObject(shapes, shape_count, target_id, canvas);
                if (success) {
                    displayCanvas(canvas);
                    printf("Object ID %d modified successfully.\n\n", target_id);
                } else {
                    printf("Error: Object ID %d not found.\n\n", target_id);
                }
                break;
            }

            case 7: { // Display Canvas
                displayCanvas(canvas);
                if (shape_count > 0) {
                    displayObjects(shapes, shape_count);
                }
                break;
            }
        }
    }

    return 0;
}
