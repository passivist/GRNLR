#include <math.h>
#include <iostream>

using namespace std;

/*
  SC implementation of sine window slices
  (
  var length = 6000;
  var offset = 1500;
  var thisBlock = 1024;
  thisBlock.collect{ |i| (((i + offset)/length)  * pi).sin }.plot
  )
*/

void sinSlice (int lengthInSamples, int samplesThisBlock, int position)
{
  float gain = 0;
  float angle = 0;
  const float float_Pi = 3.14159265;
  for(int i=0; i<samplesThisBlock; ++i)
    {
      angle = (float) ((i+position)%lengthInSamples)/lengthInSamples;
      gain = sin(angle * float_Pi);
      cout << gain << " ";
    }
  cout << endl;
}

int main(){
  int length, blockSize, startPos;

  cout << "Length: " << endl;
  cin >> length;

  cout << "BlockSize: " << endl;
  cin >> blockSize;

  cout << "StartPos: " << endl;
  cin >> startPos;

  sinSlice(length, blockSize, startPos);

  return 0;
}
