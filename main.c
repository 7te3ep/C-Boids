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
   const int boidsQty = 700;
   const int boidsSize = 10;
   const int boidsSpeed = 3;
   const int maxInfluence = 6;
   const int divergeRange = 20;
   const int convergeRange = 60;
   const int influenceStrenght = 5;

   const int rows = ceil(screenWidth / convergeRange);
   const int columns = ceil(screenHeight / convergeRange);
   int grid[columns][rows][boidsQty];
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
      drawBoids(boids, boidsQty, boidsSize);

      EndDrawing();
   }

   CloseWindow();
   return 0;
}

int fillGrid(double boids[][4], int size, int rows, int columns, int grid[][rows][size], int convergeRange)
{
   // reset grid
   for (int i = 0; i < columns; i++)
   {
      for (int j = 0; j < rows; j++)
      {
         for (int k = 0; k < size; k++)
         {
            grid[i][j][k] = 0;
         }
      }
   }

   int i;
   for (i = 0; i < size; i++)
   {
      // get grid pos
      const int posX = floor(boids[i][1] / convergeRange);
      const int posY = floor(boids[i][0] / convergeRange);

      // get last index in array
      int index = 0;
      int j;
      for (j = 0; j < size; j++)
      {
         if (grid[posX][posY][j] == 0)
            break;
         index += 1;
      }

      // append the index
      grid[posX][posY][index] = i;
      //printf("%d %d %d\n",columns, rows, size);
      //printf("%d %d\n",posX,posY);
      //printf("%d\n", index);
   }
}

int updateBoids(double boids[][4], int size, int maxInfluence, int divergeRange, int convergeRange, int influenceStrenght, int width, int height, int rows, int columns, int grid[][rows][size])
{
   int i;
   int j;
   for (i = 0; i < size; i++)
   {
      double influence = 0;
      int neighbors = 1;
      for (j = 0; j < size; j++)
      {
         if (neighbors == maxInfluence)
            break;

         if (boids[j][0] != boids[i][0] && boids[j][1] != boids[i][1])
         {

            double dist;
            dist = sqrt(pow(boids[j][0] - boids[i][0], 2) + pow(boids[j][1] - boids[i][1], 2));
            if (dist < divergeRange)
            {
               neighbors += 1;
               if (boids[j][2] < boids[i][2])
                  influence += influenceStrenght;
               else
                  influence -= influenceStrenght;
            }
            else if (dist < convergeRange)
            {
               neighbors += influenceStrenght;
               if (boids[j][2] < boids[i][2])
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
         boids[i][0] = width-1;
      else if (boids[i][0] > width-1)
         boids[i][0] = 1;
      if (boids[i][1] <= 0)
         boids[i][1] = height-1;
      else if (boids[i][1] > height-1)
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

int debugBoid(double boid[4])
{
   printf("x : %f \n y : %f \n a : %f \n v : %f \n", boid[0], boid[1], boid[2], boid[3]);
   printf("\n");
}