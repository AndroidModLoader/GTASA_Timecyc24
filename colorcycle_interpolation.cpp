// This file was used to interpolate value-by-value for colorcycle_24h.dat file.
// Use it in any CPP-online site or simply do your own magic ***
// I was kinda sleepy and had a headache so i did it like that.

#include <iostream>
#include <string>

struct RQVector
{
    void Print() { printf("%.3f %.3f %.3f %.3f ", r,g,b,a); };
    float r,g,b,a;
};

#define NUMWEATHER 23
#define NUMHOURS 24

void PrintHourComment(int hour)
{
    if(hour == 0) std::cout << " // 12 AM\n";
    else if(hour == 12) std::cout << " // 12 PM\n";
    else
    {
      if(hour >= 12)
      {
        hour -= 12;  
        std::cout << " // " << hour << " PM\n";
      }
      else
      {
        std::cout << " // " << hour << " AM\n";
      }
    }
}

// Yeah, i know.
const char* psz[8] = {
"2.124 0.000 0.000 -0.007 0.000 2.124 0.000 -0.007 0.000 0.000 2.124 -0.007",
"1.938 0.000 0.000 0.004 0.000 1.948 0.000 0.001 0.000 0.000 1.948 0.001",
"1.504 0.000 0.000 0.032 0.000 1.539 0.000 0.022 0.000 0.000 1.539 0.022",
"1.890 0.000 0.000 0.015 0.000 1.888 0.000 0.005 0.000 0.000 1.942 -0.001",
"2.471 0.000 0.000 -0.010 0.000 2.412 0.000 -0.020 0.000 0.000 2.547 -0.035",
"2.233 0.001 0.001 -0.025 -0.000 2.067 0.000 -0.018 0.000 0.000 2.198 -0.053",
"1.877 0.002 0.002 -0.049 -0.001 1.550 0.001 -0.016 0.000 0.000 1.674 -0.079",
"2.050 0.000 0.000 -0.020 -0.000 1.951 0.000 -0.010 0.000 0.000 1.989 -0.029",
};

#define INTERP_IT(__rqvec, __idx)  __rqvec[i].__idx = __rqvec[pszPos * 3].__idx - (i % 3) * (__rqvec[pszPos * 3].__idx - __rqvec[nextPos * 3].__idx) / 3.0f;
#define INTERP(__rqvec)  INTERP_IT(__rqvec, r); INTERP_IT(__rqvec, g); INTERP_IT(__rqvec, b); INTERP_IT(__rqvec, a);

RQVector Red[NUMHOURS];
RQVector Green[NUMHOURS];
RQVector Blue[NUMHOURS];

int main()
{
  for(int i = 0; i < 8; ++i)
  {
    sscanf(psz[i], "%f %f %f %f %f %f %f %f %f %f %f %f",
                   &Red[i * 3].r, &Red[i * 3].g, &Red[i * 3].b, &Red[i * 3].a,
                   &Green[i * 3].r, &Green[i * 3].g, &Green[i * 3].b, &Green[i * 3].a,
                   &Blue[i * 3].r, &Blue[i * 3].g, &Blue[i * 3].b, &Blue[i * 3].a);
  }
  for(int i = 0; i < 24; ++i)
  {
    if(i % 3 == 0)
    {
      std::cout << psz[i / 3];
      PrintHourComment(i);
    }
    else
    {
      int pszPos = i / 3;
      int nextPos = pszPos + 1;
      if(nextPos == 8) nextPos = 0;
      
      //Red[i].r = Red[pszPos].r - (i % 3) * (Red[pszPos].r - Red[nextPos].r) / 3.0f;
      INTERP(Red);
      INTERP(Green);
      INTERP(Blue);
      
      Red[i].Print();
      Green[i].Print();
      Blue[i].Print();
      PrintHourComment(i);
    }
  }
}