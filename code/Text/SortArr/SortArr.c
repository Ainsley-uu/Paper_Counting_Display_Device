#include <SortArr.h>
void SortArr(float  Array[],int ArrayLength)
{
    unsigned char i=0;
    unsigned char j=0;
	
    for (i = 0; i <ArrayLength-1; i++)
    {
       for (j = 0; j < ArrayLength - i - 1; j++)
        {
          float ArrayValue=0;
          if (Array[j] > Array[j + 1])
          {
             ArrayValue = Array[j];
             Array[j] = Array[j+1];
             Array[j+1] = ArrayValue;
          }
        }
    }

}
