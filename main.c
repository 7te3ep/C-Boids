#include <stdio.h>
#include <time.h>
#include <raylib.h>
#include <math.h>

int main(void)
{
   srand(time(NULL));
   SetTraceLogLevel(LOG_WARNING);
   const int screenWidth = 1600;
   const int screenHeight = 900;
   const int boidsQty = 3100;
   const int boidsSize = 10;
   const int boidsSpeed = 3;
   const int maxInfluence = 6;
   const int divergeRange = 20;
   const int convergeRange = 60;
   const int influenceStrenght = 5;

   const int columns = ceil((double)screenWidth / convergeRange);
   const int rows = ceil((double)screenHeight / convergeRange);
   printf("%d : %d \n\n", rows, columns);

   int grid[rows * columns * boidsQty];
   double boids[boidsQty][4];
   int i;
   for (i = 0; i < boidsQty; i++)
   {
      boids[i][0] = rand() % screenWidth;
      boids[i][1] = rand() % screenHeight;
      boids[i][2] = rand() % 360;
      boids[i][3] = boidsSpeed;
   }

   InitWindow(screenWidth, screenHeight, "Boids demo");
   SetTargetFPS(30);
   while (!WindowShouldClose())
   {
      fillGrid(boids, boidsQty, rows, columns, grid, convergeRange);
      updateBoids(boids, boidsQty, maxInfluence, divergeRange, convergeRange, influenceStrenght, screenWidth, screenHeight, rows, columns, grid);
      BeginDrawing();

      ClearBackground(RAYWHITE);

      // DRAW
      drawBoids(boids, boidsQty, boidsSize);
      //// debug
      //debugBoid(boids[0], convergeRange, boids, grid, boidsQty, rows, columns);
//
      //for (int i = 0; i < screenHeight; i += convergeRange)
      //{
      //   DrawLine(0, i, screenWidth, i, BLUE);
      //}
      //for (int i = 0; i < screenWidth; i += convergeRange)
      //{
      //   DrawLine(i, 0, i, screenHeight, BLUE);
      //}

      EndDrawing();
   }

   CloseWindow();
   return 0;
}

int fillGrid(double boids[][4], int size, int rows, int columns, int grid[], int convergeRange)
{
   // reset grid
   for (int i = 0; i < (columns * rows * size); i++)
   {
      grid[i] = -1;
   }

   for (int i = 0; i < size; i++)
   {
      // get grid pos
      const int posX = floor(boids[i][0] / convergeRange);
      const int posY = floor(boids[i][1] / convergeRange);

      // get last index in array
      int posInArr = (posY * columns * size) + (posX * size);
      for (int j = posInArr; j < posInArr + size; j++)
      {
         if (grid[j] == -1)
         {
            grid[j] = i;
            break;
         }
      }
   }
}

int updateBoids(double boids[][4], int size, int maxInfluence, int divergeRange, int convergeRange, int influenceStrenght, int width, int height, int rows, int columns, int grid[])
{
   for (int i = 0; i < size; i++)
   {
      double influence = 0;
      int neighbors = 1;
      int neighborsArr[size];
      int neighborsArrIndex = 0;

      for (int j = 0; j < size; j++)
      {
         neighborsArr[j] = -1;
      }

      const int posX = floor(boids[i][0] / convergeRange);
      const int posY = floor(boids[i][1] / (convergeRange + 1));
      int pos = (posY * columns * size) + (posX * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posY > 0)
         pos = ((posY - 1) * columns * size) + (posX * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posY < rows - 1)
         pos = ((posY + 1) * columns * size) + (posX * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posX > 0)
         pos = (posY * columns * size) + ((posX - 1) * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posX < columns - 1)
         pos = (posY * columns * size) + ((posX + 1) * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posY > 0 && posX > 0)
         pos = ((posY - 1) * columns * size) + ((posX - 1) * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posY < rows - 1 && posX < columns - 1)
         pos = ((posY + 1) * columns * size) + ((posX + 1) * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posY < rows - 1 && posX > 0)
         pos = ((posY + 1) * columns * size) + ((posX - 1) * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
      if (posY > 0 && posX < columns - 1)
         pos = ((posY - 1) * columns * size) + ((posX + 1) * size);
      neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);

      for (int j = 0; j < size; j++)
      {
         if (neighborsArr[j] == -1)
            break;

         const int index = neighborsArr[j];
         if (neighbors == maxInfluence)
            break;

         if (boids[index][0] != boids[i][0] && boids[index][1] != boids[i][1])
         {

            double dist;
            dist = sqrt(pow(boids[index][0] - boids[i][0], 2) + pow(boids[index][1] - boids[i][1], 2));
            if (dist < divergeRange)
            {
               neighbors += 1;
               if (boids[index][2] < boids[i][2])
                  influence += influenceStrenght;
               else
                  influence -= influenceStrenght;
            }
            else if (dist < convergeRange)
            {
               neighbors += influenceStrenght;
               if (boids[index][2] < boids[i][2])
                  influence -= influenceStrenght;
               else
                  influence += influenceStrenght;
            }
         }
      }

      boids[i][2] += influence / (neighbors);
      boids[i][0] += boids[i][3] * cos((boids[i][2]) * PI / 180);
      boids[i][1] += boids[i][3] * sin((boids[i][2]) * PI / 180);
      if (boids[i][0] <= 0)
         boids[i][0] = width - 1;
      else if (boids[i][0] > width - 1)
         boids[i][0] = 1;
      if (boids[i][1] <= 0)
         boids[i][1] = height - 1;
      else if (boids[i][1] > height - 1)
         boids[i][1] = 1;
   }
}

int drawBoids(double boids[][4], int size, int boidsSize)
{
   int i;
   for (i = 0; i < size; i++)
   {
      int x = boids[i][0];
      int y = boids[i][1];
      double angle = boids[i][2];
      Vector2 point1 = {boidsSize * cos(angle * PI / 180), boidsSize * sin(angle * PI / 180)};
      Vector2 point2 = {boidsSize / 2 * cos((angle - 90) * PI / 180), boidsSize / 2 * sin((angle - 90) * PI / 180)};
      Vector2 point3 = {boidsSize / 2 * cos((angle + 90) * PI / 180), boidsSize / 2 * sin((angle + 90) * PI / 180)};
      DrawTriangle((Vector2){x + point1.x, y + point1.y}, (Vector2){x + point2.x, y + point2.y}, (Vector2){x + point3.x, y + point3.y}, (Color){0, 0, 0, 255});
   }
}

int addNeighbors(int neighborsArr[], int pos, int index, int size, int grid[])
{
   for (int j = pos; j < pos + size; j++)
   {
      if (grid[j] == -1)
         break;
      neighborsArr[index] = grid[j];
      index += 1;
   }

   return index;
}

int debugBoid(double boid[4], int convergeRange, double boids[][4], int grid[], int size, int rows, int columns)
{
   printf("x : %f \n y : %f \n a : %f \n v : %f \n|n", boid[0], boid[1], boid[2], boid[3]);
   double influence = 0;
   int neighbors = 1;
   int neighborsArr[size];
   int neighborsArrIndex = 0;

   for (int j = 0; j < size; j++)
   {
      neighborsArr[j] = -1;
   }

   const int posX = floor(boid[0] / convergeRange);
   const int posY = floor(boid[1] / (convergeRange));

   DrawRectangle(posX * convergeRange, posY * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle((posX - 1) * convergeRange, posY * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle((posX + 1) * convergeRange, posY * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle(posX * convergeRange, (posY - 1) * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle(posX * convergeRange, (posY + 1) * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle((posX - 1) * convergeRange, (posY + 1) * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle((posX + 1) * convergeRange, (posY - 1) * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle((posX - 1) * convergeRange, (posY - 1) * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawRectangle((posX + 1) * convergeRange, (posY + 1) * convergeRange, convergeRange, convergeRange, (Color){100, 0, 100, 150});
   DrawCircle(boid[0], boid[1], convergeRange, (Color){0, 255, 0, 150});

   int x = boid[0];
   int y = boid[1];
   double angle = boid[2];
   Vector2 point1 = {20 * cos(angle * PI / 180),            20 * sin(angle * PI / 180)};
   Vector2 point2 = {20 / 2 * cos((angle - 90) * PI / 180), 20 / 2 * sin((angle - 90) * PI / 180)};
   Vector2 point3 = {20 / 2 * cos((angle + 90) * PI / 180), 20 / 2 * sin((angle + 90) * PI / 180)};
   DrawTriangle((Vector2){x + point1.x, y + point1.y}, (Vector2){x + point2.x, y + point2.y}, (Vector2){x + point3.x, y + point3.y}, RED);

   int pos = (posY * columns * size) + (posX * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posY > 0)
      pos = ((posY - 1) * columns * size) + (posX * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posY < rows - 1)
      pos = ((posY + 1) * columns * size) + (posX * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posX > 0)
      pos = (posY * columns * size) + ((posX - 1) * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posX < columns - 1)
      pos = (posY * columns * size) + ((posX + 1) * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posY > 0 && posX > 0)
      pos = ((posY - 1) * columns * size) + ((posX - 1) * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posY < rows - 1 && posX < columns - 1)
      pos = ((posY + 1) * columns * size) + ((posX + 1) * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posY < rows - 1 && posX > 0)
      pos = ((posY + 1) * columns * size) + ((posX - 1) * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);
   if (posY > 0 && posX < columns - 1)
      pos = ((posY - 1) * columns * size) + ((posX + 1) * size);
   neighborsArrIndex = addNeighbors(neighborsArr, pos, neighborsArrIndex, size, grid);

   for (int j = 0; j < size; j++)
   {
      if (neighborsArr[j] == -1)
         break;
      DrawCircle(boids[neighborsArr[j]][0], boids[neighborsArr[j]][1], 1, YELLOW);
   }
}