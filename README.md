# 2D Graphics Editor in C

A complete, interactive, menu-driven console application built in standard C (C99) to draw, modify, and delete 2D geometric shapes on a 50x50 character-based canvas.

## Features

1. **Discrete 2D Canvas**: 
   - Uses a 50x50 character array initialized with `'_'` (underscore) and draws pixels with `'*'` (asterisk).
   - Display grid uses column headers (aligned tens and units digits) and row headers (`00` to `49`) to help identify exact coordinates.
   - Spaces are printed between canvas characters in the console to achieve a 1:1 aspect ratio, preventing circle and square distortions.

2. **Accurate Drawing Algorithms**:
   - **Line Drawing**: Uses the classic integer-based **Bresenham's Line Algorithm** to rasterize lines between any two coordinates without floating-point math.
   - **Circle Drawing**: Uses the **Midpoint Circle Algorithm** (Bresenham's Circle) to draw perfect circular outlines using 8-way symmetry.
   - **Rectangle**: Drawn using 4 boundary line segments.
   - **Triangle**: Drawn using 3 line segments connecting 3 vertices.
   - **Clipping**: Built-in boundary checks block off-canvas pixels from causing out-of-bounds writes.

3. **Vector List & Canvas Redrawing**:
   - Maintains a structured list of drawn objects (up to 100 shapes) with unique IDs.
   - When an object is **deleted** or **modified**, the canvas is cleared and all remaining active shapes are redrawn. This robust approach prevents deletion of one shape from corrupting parts of other shapes it overlaps with.

4. **Input Validation**:
   - Employs a robust `getIntegerInput` utility that rejects empty values, alphabetical characters, trailing garbage, and out-of-range bounds, preventing infinite loop crashes standard in console menus.

---

## File Structure

- [graphics_editor.c](file:///C:/Users/prern/.gemini/antigravity/scratch/c-graphics-editor/graphics_editor.c): Main source code containing data structures, drawing logic, object management, and the interactive menu interface.
- [README.md](file:///C:/Users/prern/.gemini/antigravity/scratch/c-graphics-editor/README.md): Documentation on compilation and usage instructions (this file).

---

## Technical Specifications & Functions

The code implements the following functions conforming to the requirements:
- `initializeCanvas()`: Fills the 50x50 canvas array with `'_'`.
- `displayCanvas()`: Renders the canvas grid along with X/Y axis markings.
- `drawLine()`: Renders a line between two points using Bresenham's Line Algorithm.
- `drawRectangle()`: Draws a rectangular boundary.
- `drawTriangle()`: Connects three coordinate vertices to form a triangle.
- `drawCircle()`: Draws a circular boundary using the Midpoint Circle Algorithm.
- `deleteObject()`: Removes a shape from the list and triggers a canvas redraw.
- `modifyObject()`: Prompts for new coordinates to redraw a shape under its original ID.

---

## Compilation and Execution

### Prerequisites
- Any standard C compiler supporting the C99 standard (such as `gcc`, `clang`, or MSVC).

### Compiling on Windows/Linux/macOS with GCC
Run the following command in your terminal/command prompt:
```bash
gcc -Wall -Wextra -std=c99 -o graphics_editor graphics_editor.c
```

### Running the Application
```bash
./graphics_editor
```

---

## Interactive Menu Instructions

Upon launching, the editor presents the canvas and a menu:

1. **Draw Rectangle**: Prompts for `X1`, `Y1` (first corner) and `X2`, `Y2` (opposite corner) coordinates.
2. **Draw Line**: Prompts for start `(X1, Y1)` and end `(X2, Y2)` coordinates.
3. **Draw Triangle**: Prompts for three coordinates `(X1, Y1)`, `(X2, Y2)`, and `(X3, Y3)`.
4. **Draw Circle**: Prompts for center coordinates `(CX, CY)` and radius.
5. **Delete Object**: Displays active shapes and prompts for an ID to delete.
6. **Modify Object**: Displays active shapes and prompts for an ID to re-enter coordinates.
7. **Display Canvas**: Shows the current canvas and the active object list.
8. **Exit**: Exits the program.
