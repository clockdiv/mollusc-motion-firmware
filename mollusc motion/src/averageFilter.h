#ifndef AVERAGEFILTER_H
#define AVERAGEFILTER_H

class averageFilter
{
  int32_t maxValues;
  float values[8]; // how many values to take the average from
  float average;

public:
  averageFilter()
  {
    maxValues = sizeof(values) / sizeof(float);

    for (int i = 0; i < maxValues; i++)
    {
      values[i] = 0;
    }
  }

  float filter(float v)
  {
    // shift all values one to the left
    for (int i = 0; i < maxValues - 1; i++)
    {
      values[i] = values[i + 1];
    }

    // add current value to the latest index
    values[maxValues - 1] = v;

    // calculate the average
    average = 0;
    for (int i = 0; i < maxValues; i++)
    {
      average += values[i];
    }
    average /= maxValues;

    return average;
  }
};

#endif